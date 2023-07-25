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

unsigned int color_to_int(const std::array<u_char, 3>& color) {
    unsigned int red = color[0];
    unsigned int green = color[1];
    unsigned int blue = color[2];

    return (red << 16) | (green << 8) | blue;
}

void blend_color(
        Matrix<unsigned char>& img,
        std::vector<bool>& mask,
        std::array<u_char, 3> color
) {
    std::array<float, 3> color_ = {
        color[0]/255.f,
        color[1]/255.f,
        color[2]/255.f
    };
    auto* p = img.pt();
    auto size = img.size();
    for (int i = 0; i < size; i += 3) {
        if (!mask[i/3])
            continue;
        p[i] *= color_[0]; 
        p[i+1] *= color_[1]; 
        p[i+2] *= color_[2]; 
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

        blend_color(drawing_, partition, new_color);

        // unite new and old used pixels
        std::transform( 
                partition.begin(), partition.end(),
                used_pixels.begin(), used_pixels.begin(),
                [](bool a, bool b) { return a || b; }
        );
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
    int w, h, c;
    unsigned char *data = stbi_load(filename, &w, &h, &c, 0);
    if (!data) {
        std::cerr << "Image was not loaded\n" << std::endl;
        return false;
    }
    if (c != 3) {
        // or conver image to 3 channels 
        std::cerr << "Only accepting images with 3 channels\n" << std::endl;
        return false;
    }

    drawing_.reset(h, w, c);
    int size = drawing_.size();
    auto *pt = drawing_.pt();
    for (int i = 0; i != size; ++i) {
        pt[i] = data[i]; 
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
    if (extension == "TGA")
        return ImageType::TGA;
    if (extension == "jpg")
        return ImageType::JPG;

    return ImageType::NOT_IMG;
}

auto Painter::imwrite(const std::string& filename) -> bool {
    auto type = get_image_type_(filename);
    if (type == ImageType::NOT_IMG)
        return false;

    int success = 0;
    
    if (type == ImageType::PNG) {
        success = stbi_write_png(filename.data(), drawing_.width(), drawing_.height(), drawing_.channels(), drawing_.pt(), drawing_.width()*drawing_.channels());
    } 
    else if (type == ImageType::BMP) {
        success = stbi_write_bmp(filename.data(), drawing_.width(), drawing_.height(), drawing_.channels(), drawing_.pt());
    }
    else if (type == ImageType::TGA) {
        success = stbi_write_tga(filename.data(), drawing_.width(), drawing_.height(), drawing_.channels(), drawing_.pt());
    }
    else if (type == ImageType::JPG) {
        success = stbi_write_jpg(filename.data(), drawing_.width(), drawing_.height(), drawing_.channels(), drawing_.pt(), 100);
    }

    if (!success)
        return false;
    return true;
    
}

void Painter::init_gray(float gamma) {
    gray_.reset(drawing_.height(), drawing_.width(), 1);

    // only 3 channel img
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
