#include "matrix_utils.hpp"

#include "stb_image.h"
#include "stb_image_write.h"

Matrix<unsigned char> imread(const char* filename) {
    int w, h, c;
    unsigned char *data = stbi_load(filename, &w, &h, &c, 0);
    if (!data) {
        std::cerr << "Image was not loaded\n" << std::endl;
    }

    Matrix<unsigned char> m(h, w, c);
    int size = m.size();
    auto *pt = m.pt();
    for (int i = 0; i != size; ++i) {
        pt[i] = data[i]; 
    }
    
    stbi_image_free(data);
    return m;
}

// TODO: output format
bool imwrite(const char* filename, Matrix<unsigned char>& m) {
    return stbi_write_png(filename, int(m.width()), int(m.height()), int(m.channels()), m.pt(), int(m.width()));
}

void add_img_edges(
        EdmondsKarp<int>& graph,
        const Matrix<unsigned char>& img

) {
    assert(!img.empty());
    assert(img.channels() == 1);
    assert(img.size() + 2 == graph.V());

    const auto size = img.size();
    const auto width = img.width();

    unsigned char zero_cancel = 1;
    auto* pt = img.pt();
    for (int i = 0; i != size; ++i) {
        if (i % width) {
            unsigned char new_val = std::min(pt[i], pt[i-1]);
            graph.add_bidirectional_edge(
                    i, i-1, 
                    std::max(zero_cancel, new_val));
        }
        if (i >= width) {
            unsigned char new_val = std::min(pt[i], pt[i-width]);
            graph.add_bidirectional_edge(
                    i, i-width, 
                    std::max(zero_cancel, new_val));
        }        
    }
}

// source -> non white scribbles
// white scribbles -> sink
// zero alpha scribbles are skipped
void add_scribble_edges(
        EdmondsKarp<int>& graph,
        const Matrix<unsigned char>& scribbles,
        int s_cap
) { 
    assert(!scribbles.empty());
    assert(scribbles.channels() == 4);
    assert(scribbles.size()/scribbles.channels() + 2 == graph.V());
    
    int source = graph.V()-2;
    int sink = source + 1;

    const auto size = scribbles.size();
    auto* pt = scribbles.pt();

    for (int i = 0; i < size; i += 4) {
        if (pt[i+3] == 0) 
            continue; 

        if (pt[i] == 255 and pt[i+1] == 255 and pt[i+2] == 255) {
            graph.add_directional_edge(i / 4, sink, s_cap);
        }
        else {
            graph.add_directional_edge(source, i / 4, s_cap);
        }
    }
}


// Assumes that vertices graph.V & graph.V + 1 are source and sink
// s := rgb value of pixel on scribbles that will be assigned to source
// all other pixels *with non zero alpha* to sink 
// TODO: DELETE IT
void add_scribble_edges_old(
        EdmondsKarp<int>& graph,
        const Matrix<unsigned char>& scribbles,
        unsigned char s,
        int s_cap
) { 
    assert(!scribbles.empty());
    assert(scribbles.size() + 2 == graph.V());
    
    const auto size = scribbles.size();
    int source = size;
    int sink = size + 1;

    auto* pt = scribbles.pt();
    for (int i = 0; i != size; ++i) {
        if (pt[i] == s) {
            graph.add_directional_edge(source, i, s_cap);
        }
        else if (pt[i]) {
            graph.add_directional_edge(i, sink, s_cap);
        }
    }
}

