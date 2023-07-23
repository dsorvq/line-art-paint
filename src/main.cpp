#include <iostream>
#include <array>

#include "matrix.hpp"
#include "matrix_utils.hpp"
#include "graph_utils.hpp"

template <class scalar_t>
void print_image(Matrix<scalar_t>& m) {
    auto m_size = m.size();
    auto m_width = m.width();
    auto* m_pt = m.pt();
    
    for (int i = 0; i != m_size; ++i) {
        if (i % m_width == 0)
            std::cout << '\n';
        std::cout << +m_pt[i] << ' ';
        if (m_pt[i] < 10)
            std::cout << ' ';
        if (m_pt[i] < 100)
            std::cout << ' ';
    }
    std::cout << '\n';
}

void print_partition(std::vector<int>& partition, int height, int width) {
    auto size = width * height;
    for (int i = 0; i != size; ++i) {
        if (i % width == 0) {
            std::cout << '\n';
        }
        std::cout << partition[i] << ' ' ;
        if (partition[i] < 10)
            std::cout << ' ';
        if (partition[i] < 100)
            std::cout << ' ';
    }
    std::cout << '\n';
}

int main() {
    auto img = imread("box/drawing.png");
    //TODO: check is gamma correction correct
    auto gray = to_gray_gamma(img, 1/2.0);

    /*
    std::cout 
        << "\nchannels : " << gray.channels()
        << "  height : " << gray.height()  
        << "  width : " << gray.width();

    std::cout << '\n';
    std::cout << "image gray:";
    print_image(gray);
    std::cout << '\n';
    return 0;  
    */

    auto scribbles = imread("box/scribbles.png");

    int width = gray.width();
    int height = gray.height();
    auto size = gray.size();

    // 23 works best so far
    constexpr int terminal_capacity = 23;

    //EdmondsKarp<int> graph(size + 2);
    Dinic<int> graph(size + 2);
    add_img_edges(graph, gray);
    add_scribble_edges(graph, scribbles, terminal_capacity);
   
    std::cout << "max flow : " << graph.max_flow(size, size + 1);
    auto partition = graph.partition(size);

    /*
    std::array<u_char, 3> color = {0, 255, 0};
    Matrix<u_char> res(gray.height(), gray.width(), 3, 255);
    auto* res_p = res.pt();
    for (int i = 0; i != size; ++i) {
        if (!partition[i])
            continue;
        int id = i * 3;
        res_p[id] = color[0];
        res_p[id+1] = color[1];
        res_p[id+2] = color[2];
    }
    imwrite("result.png", res);
    */

    // if image has 3 pixels
    std::array<float, 3> color = {0, 1, 0};
    auto* img_p = img.pt();
    auto img_size = img.size();
    for (int i = 0; i < img_size; i += 3) {
        if (!partition[i/3])
            continue;
        img_p[i] *= color[0]; 
        img_p[i+1] *= color[1]; 
        img_p[i+2] *= color[2]; 
    }
    imwrite("result.png", img); 

    return 0;
}

