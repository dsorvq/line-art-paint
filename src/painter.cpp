#include "painter.hpp"

#include "dinic.hpp"

#include "stb_image.h"
#include "stb_image_write.h"
#include <array>
#include <sys/types.h>

Painter::Painter(const char* filename, int terminal_capacity)
    : terminal_capacity_{terminal_capacity}
{ 
    if (!imread(filename)) {
        return;
    }
    init_gray(1/2.0);
}

auto Painter::drawing() const -> const Matrix<u_char>& {
    return drawing_painted_;
}

unsigned int color_to_int(const std::array<u_char, 3>& color) {
    unsigned int red = color[0];
    unsigned int green = color[1];
    unsigned int blue = color[2];

    return (red << 16) | (green << 8) | blue;
}

void Painter::blend_color(
        std::vector<bool>& mask,
        std::array<u_char, 3> color
) {
    assert(drawing_painted_.channels() == 4);
    assert(drawing_.channels() == 3);

    std::array<float, 3> color_ = {
        color[0]/255.f,
        color[1]/255.f,
        color[2]/255.f
    };
    auto* p_pt = drawing_painted_.pt();
    auto* o_pt = drawing_.pt();
    auto size = drawing_.size();
    int p_id = 0;
    for (int i = 0, j = 0; i < size; i += 3, j += 4) {
        if (!mask[i/3])
            continue;
        p_pt[j] = o_pt[i] * color_[0];
        p_pt[j+1] = o_pt[i+1] * color_[1];
        p_pt[j+2] = o_pt[i+2] * color_[2];
        p_pt[j+3] = 255;
    }
}

void Painter::paint(Matrix<unsigned char>& scribbles) {
    const std::array<u_char, 3> default_color {255, 255, 255};

    auto pixels = gray_.size();
    std::vector<bool> used_pixels(pixels);
    std::unordered_set<unsigned int> used_colors;
    used_colors.insert(color_to_int(default_color));
    std::array<u_char, 3> new_color {};

    while (true) {
        Dinic<int> graph(pixels + 2);

        if (!add_drawing_edges(graph, used_pixels)) {
            break;
        }
        if (!add_scribbles_edges(graph, used_colors, new_color, scribbles)) {
            break;
        }
        used_colors.insert(color_to_int(new_color));

        auto flow = graph.max_flow(pixels, pixels + 1);
        std::cout << "flow=" << flow << '\n';
        auto partition = graph.partition(pixels);

        blend_color(partition, new_color);

        // unite new and old used pixels
        // Warning: vector<bool> may cause valgrind errors
        /*
        std::transform( 
                partition.begin(), partition.end(),
                used_pixels.begin(), used_pixels.begin(),
                [](bool a, bool b) { return a || b; }
        ); */
        for (int i = 0; i != pixels; ++i) {
            used_pixels[i] = used_pixels[i] || partition[i];
        }
    }
}

bool Painter::add_drawing_edges(
        Dinic<int>& graph, 
        std::vector<bool>& used_pixels)
{
    assert(!gray.empty());
    assert(gray.channels() == 1);
    assert(used_pixels.size() == gray.size());
    assert(gray.size() + 2 == graph.V());
    
    const auto size = gray_.size();
    const auto width = gray_.width();

    unsigned char zero_cancel = 1;
    auto* pt = gray_.pt();
    bool new_edge_added = false;
    for (int i = 0; i != size; ++i) {
        if (used_pixels[i])
            continue;
        if (i % width and !used_pixels[i-1]) {
            new_edge_added = true;
            unsigned char new_val = std::min(pt[i], pt[i-1]);
            graph.add_bidirectional_edge(
                    i, i-1, 
                    std::max(zero_cancel, new_val));
        }
        if (i >= width and !used_pixels[i-width]) {
            new_edge_added = true;
            unsigned char new_val = std::min(pt[i], pt[i-width]);
            graph.add_bidirectional_edge(
                    i, i-width, 
                    std::max(zero_cancel, new_val));
        }        
    }

    return new_edge_added;
}

bool Painter::add_scribbles_edges(
        Dinic<int>& graph,
        std::unordered_set<unsigned int>& used_colors,
        std::array<u_char, 3>& new_color,
        Matrix<unsigned char>& scribbles) 
{
    assert(!scribbles.empty());
    assert(scribbles.channels() == 4);
    assert(scribbles.size()/scribbles.channels() + 2 == graph.V());

    int source = graph.V()-2;
    int sink = source + 1;

    const auto size = scribbles.size();
    auto* pt = scribbles.pt();

    int id = 0;
    for (; id < size; id += 4) {
        if (pt[id+3] == 0) 
            continue; 
        auto color = color_to_int({pt[id+0], pt[id+1], pt[id+2]});
        if (used_colors.find(color) != used_colors.end()) {
            graph.add_directional_edge(id / 4, sink, terminal_capacity_);
            continue;
        }

        new_color = {pt[id+0], pt[id+1], pt[id+2]};
        break;
    }

    // all colors are alread in used_colors 
    if (id >= size) {
        return false;
    }

    for (int i = id; i < size; i += 4) {
        if (pt[i+3] == 0) 
            continue; 
        
        if (pt[i] == new_color[0] and pt[i+1] == new_color[1] and pt[i+2] == new_color[2]) {
            graph.add_directional_edge(source, i / 4, terminal_capacity_);
        }
        else {
            graph.add_directional_edge(i / 4, sink, terminal_capacity_);
        }
    }

    return true;
}

auto Painter::imread(const char* filename) -> bool {
    // as a result: 
    //  drawing_ have 3 channels
    //  drawing_painted_ have 4 channels
    
    int w, h, c;
    unsigned char *data = stbi_load(filename, &w, &h, &c, 3);
    if (!data) {
        std::cerr << "Image was not loaded\n" << std::endl;
        return false;
    }
    if (c != 3) {
        c = 3;
        // stbi should handle this
        std::cerr << "Image was converted to 3 channels\n" << std::endl;
    }

    drawing_painted_.reset(h, w, 4);
    drawing_.reset(h, w, c);
    int size = drawing_.size();
    auto *o_pt = drawing_.pt();
    auto *p_pt = drawing_painted_.pt();
    int o_id = 0, p_id = 0;

    // data should have 3 channels
    for (int i = 0; i < size; i += 3) {
        p_pt[p_id++] = o_pt[o_id++] = data[i]; 
        p_pt[p_id++] = o_pt[o_id++] = data[i+1]; 
        p_pt[p_id++] = o_pt[o_id++] = data[i+2]; 
        p_pt[p_id++] = 255;
    }

    stbi_image_free(data);
    return true;
}

enum class ImageType {
    NOT_IMG, PNG, BMP, TGA, JPG 
};

ImageType get_image_type_(const std::string_view filename) {
    const auto dot = filename.rfind('.');
    if (dot == std::string::npos) 
        return ImageType::NOT_IMG;

    std::string_view extension = filename.substr(dot);
    if (extension == ".png")
        return ImageType::PNG;
    if (extension == ".bmp")
        return ImageType::BMP;
    if (extension == ".tga")
        return ImageType::TGA;
    if (extension == ".jpg")
        return ImageType::JPG;

    return ImageType::NOT_IMG;
}

auto Painter::imwrite(const std::string& filename) -> bool {
    auto type = get_image_type_(filename);
    if (type == ImageType::NOT_IMG)
        return false;

    auto& img = drawing_painted_;
    int success = 0;
    
    if (type == ImageType::PNG) {
        success = stbi_write_png(filename.data(), img.width(), img.height(), img.channels(), img.pt(), img.width()*img.channels());
    } 
    else if (type == ImageType::BMP) {
        success = stbi_write_bmp(filename.data(), img.width(), img.height(), img.channels(), img.pt());
    }
    else if (type == ImageType::TGA) {
        success = stbi_write_tga(filename.data(), img.width(), img.height(), img.channels(), img.pt());
    }
    else if (type == ImageType::JPG) {
        success = stbi_write_jpg(filename.data(), img.width(), img.height(), img.channels(), img.pt(), 100);
    }

    if (!success)
        return false;
    return true;
}

void Painter::init_gray(float gamma) {
    assert(drawing_.channels() == 3);

    gray_.reset(drawing_.height(), drawing_.width(), 1);

    auto p_orig = drawing_.pt();
    auto p_gray = gray_.pt();
    auto size = drawing_.size();
    for (int i = 0, j = 0; i < size; ++j, i += 3) {
        auto gray_value = (p_orig[i]*0.2126 + p_orig[i+1]*0.7152 + p_orig[i+2]*0.0722 + 0.3) / 255.0;
        auto gray_value_corrected = std::pow(gray_value, 1/gamma);
        auto gray_value_scaled = static_cast<unsigned char>(gray_value_corrected * 255.0);
        p_gray[j] = gray_value_scaled;
    }
}
