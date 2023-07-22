#include <iostream>

#include "matrix.hpp"
#include "matrix_utils.hpp"
// #include "graph.hpp"
// #include "graph_utils.hpp"

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
    // 1. read image && convert it to gray
    // 2. create resulting matrix all white
    // 3. read scribbles, where scribbles:
    // 3.1 have some white and non white scribbles
    // all white scribbles will be used for sink terminals
    // non white pixels will be used as source terminals
    // after performing min-cut algorithm get all pixels reachable from source
    // paint corresponding pixels with *non white* scribble color

    auto img = imread("squares/img_gray.png");

    /*
    std::cout << "image:";
    print_image(img);
    std::cout << '\n';
    */

    auto scribbles = imread("squares/scribbles.png");
    /*
    std::cout << "scribbles:"; 
    print_image(scribbles);
    std::cout << '\n';
    */

    int width = img.width();
    int height = img.height();
    auto size = img.size();

    EdmondsKarp<int> graph(size + 2);
    add_img_edges(graph, img);
    add_scribble_edges(graph, scribbles, 10000000);

    std::cout << "max flow : " << graph.max_flow(size, size + 1);
    auto partition = graph.partition(size);
    print_partition(partition, height, width);

    return 0;
}

