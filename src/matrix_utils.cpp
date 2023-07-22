#include "matrix_utils.hpp"

#include "stb_image.h"
#include "stb_image_write.h"

enum ImageType {
    NOT_IMG, PNG, BMP, TGA, JPG 
};

ImageType get_image_type(const std::string_view filename) {
    const auto dot = filename.rfind('.');
    if (dot == std::string::npos) 
        return NOT_IMG;

    std::string_view extension = filename.substr(dot);
    if (extension == ".png")
        return PNG;
    if (extension == ".bmp")
        return BMP;
    if (extension == "TGA")
        return TGA;
    if (extension == "jpg")
        return JPG;

    return NOT_IMG;
}

auto imread(const char* filename) -> Matrix<unsigned char> {
    int w, h, c;
    unsigned char *data = stbi_load(filename, &w, &h, &c, 0);
    if (!data) {
        std::cerr << "Image was not loaded\n" << std::endl;
        return {};
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

auto imwrite(const std::string& filename, Matrix<unsigned char>& m) -> bool {
    auto type = get_image_type(filename);
    if (type == NOT_IMG)
        return false;

    int success = 0;

    if (type == PNG) {
        success = stbi_write_png(filename.data(), m.width(), m.height(), m.channels(), m.pt(), m.width()*m.channels());
    } 
    else if (type == BMP) {
        success = stbi_write_bmp(filename.data(), m.width(), m.height(), m.channels(), m.pt());
    }
    else if (type == TGA) {
        success = stbi_write_tga(filename.data(), m.width(), m.height(), m.channels(), m.pt());
    }
    else if (type == JPG) {
        success = stbi_write_jpg(filename.data(), m.width(), m.height(), m.channels(), m.pt(), 100);
    }

    if (!success)
        return false;
    return true;
}

auto to_gray(const Matrix<unsigned char>& m) -> Matrix<unsigned char> {
    if (m.channels() == 1) {
        return m;
    }
    if (m.channels() != 3) {
        // TODO: 
        std::cerr << "matrux must have 3 channels to be converted to gray";
        return m;
    }
    
    Matrix <unsigned char> gray(m.height(), m.width(), 1);

    // 3 channels version
    auto p_orig = m.pt();
    auto p_gray = gray.pt();
    auto size = m.size();
    for (int i = 0, j = 0; i < size; ++j, i += 3) {
        auto gray_value = p_orig[i]*0.2126 + p_orig[i+1]*0.7152 + p_orig[i+2]*0.0722 + 0.3;  
        p_gray[j] = (gray_value < 0.0f) ? 0 : ((gray_value > 255.0f) ? 255 : (unsigned char)gray_value);
    }

    return gray;
}

