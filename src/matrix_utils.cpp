#include <matrix_utils.hpp>

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

// Assumes that vertices graph.V & graph.V + 1 are source and sink
// s := rgb value of pixel on scribbles that will be assigned to source
// all other pixels *with non zero alpha* to sink 
void add_scribble_edges(
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

// TODO:
// for more then 2 sources after that we interested only on
// sink pixels
// color image pixels  with color 
/*
Matrix<unsigned char> color_from_segment(
        EdmondsKarp<int>& graph,
        Matrix<unsigned char>& m,
        unsigned char color
) {
} 
*/
