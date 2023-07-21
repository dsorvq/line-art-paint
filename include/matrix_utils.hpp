#pragma once

#include "matrix.hpp"

#include "stb_image.h"
#include "stb_image_write.h"

Matrix<unsigned char> imread(char* filename);
Matrix<unsigned char> imwrite(char* filename);

template <class scalar_t>
Matrix<scalar_t> imread(
        char* filename, 
        double alpha = 1, double beta = 0);


Matrix<unsigned char> imread(const char* filename) {
    int w, h, c;
    unsigned char *data = stbi_load(filename, &w, &h, &c, 0);

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
