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

void write_two_rectangles_image(int h, int w) {
    Matrix<unsigned char> m(h, w, 1, 0);
    auto rw = m.width();
    auto rh = m.height() / 2;
    auto* m_pt = m.pt();
    float val = 255;
    for (int i = 0; i != rh; ++i) {
        for (int j = 0; j != rw; ++j) {
            m(i, j) = val;        
        }
    }
 
    imwrite("two_rectangles.png", m);
}

void write_grad_image(int h, int w) {
    Matrix<unsigned char> m(h, w, 1, 0);
    auto m_size = m.size();
    auto m_width = m.width();
    auto* m_pt = m.pt();
    float val = 255;
    float step = val / m_size;
    for (int i = 0; i != m_size; ++i) {
        m_pt[i] = static_cast<unsigned char>(val);
        val -= step;
        if (val <= 0)
            break;
    }
 
    char filename[] = {"grad.png"};
    imwrite(filename, m);
}

void write_scribbles_image(int h, int w, u_char fg, u_char bg, int boxlen = 3) {
    Matrix<unsigned char> m(h, w, 1, 0); 
    int size = m.size();

    for (int i = 0; i != boxlen; ++i) {
        for (int j = 0; j != boxlen; ++j) {
            m(i, j) = fg;        
        }
    }
    for (int i = h - 1; i > h - boxlen - 1; --i) {
        for (int j = w - 1; j > w - boxlen - 1; --j) {
            m(i, j) = bg;
        }
    }
    char filename[] = {"scribbles.png"};
    imwrite(filename, m);
}

int main() {
    auto img = imread("two_rectangles.png");
    std::cout << "image:";
    print_image(img);
    std::cout << '\n';

    unsigned char bg = 150;
    unsigned char fg = 255;

    int width = 15;
    int height = 15;
    //write_two_rectangles_image(height, width);
    //write_grad_image(height, width);
    //write_scribbles_image(height, width, fg, bg);

    auto scribbles = imread("scribbles.png");
    std::cout << "scribbles:";
    print_image(scribbles);
    std::cout << '\n';

    return 0;
}

