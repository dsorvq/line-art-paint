#include <iostream>

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
    auto img = imread("squares/img.png");
    auto gray = to_gray(img);

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

    auto scribbles = imread("squares/scribbles.png");

    int width = gray.width();
    int height = gray.height();
    auto size = gray.size();

    EdmondsKarp<int> graph(size + 2);
    add_img_edges(graph, gray);
    add_scribble_edges(graph, scribbles, 10000000);

    std::cout << "max flow : " << graph.max_flow(size, size + 1);
    auto partition = graph.partition(size);
    print_partition(partition, height, width);

    return 0;
}

