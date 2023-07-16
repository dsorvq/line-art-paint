#include <iostream>

#include "matrix.hpp"

int main() {
    Matrix m(10, 10, 1, 5);
    Matrix b = m;
    std::cout << "m == b : " << (m == b) << '\n';
    
    Matrix c(10, 10, 1, 4);
    std::cout << "c == b : " << (c == b) << '\n';
    c = b;
    std::cout << "c == b : " << (c == b) << '\n';

    return 0;
}
