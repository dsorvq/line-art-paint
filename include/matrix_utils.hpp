#pragma once

#include "matrix.hpp"

auto imwrite(const std::string& filename, Matrix<unsigned char>& m) -> bool;
auto imread(const char* filename) -> Matrix<unsigned char>;

auto to_gray(const Matrix<unsigned char>& m) -> Matrix<unsigned char>;
auto to_gray_gamma(const Matrix<unsigned char>& m, float gamma) -> Matrix<unsigned char>;

void blend_color(
        Matrix<unsigned char>& img,
        std::array<float, 3> color,
        std::vector<bool>& mask
);
void blend_color(
        Matrix<unsigned char>& img,
        std::array<u_char, 3> color,
        std::vector<bool>& mask
);
