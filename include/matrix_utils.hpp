#pragma once

#include "matrix.hpp"

auto imwrite(const char* filename, Matrix<unsigned char>& m) -> bool;
auto imread(const char* filename) -> Matrix<unsigned char>;

auto to_gray(const Matrix<unsigned char>& m) -> Matrix<unsigned char>;

