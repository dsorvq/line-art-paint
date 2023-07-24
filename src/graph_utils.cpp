#include "graph_utils.hpp"

void add_img_edges(
        //EdmondsKarp<int>& graph,
        Dinic<int>& graph,
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

// zero alpha scribbles are skipped
// source -> pixels with color == s_color
// all other pixels -> sink
void add_scribble_edges(
        //EdmondsKarp<int>& graph,
        Dinic<int>& graph,
        const Matrix<unsigned char>& scribbles,
        int s_cap,
        std::array<u_char, 3> s_color
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
        
        if (pt[i] == s_color[0] and pt[i+1] == s_color[1] and pt[i+2] == s_color[2]) {
            graph.add_directional_edge(source, i / 4, s_cap);
        }
        else {
            graph.add_directional_edge(i / 4, sink, s_cap);
        }
    }
}

