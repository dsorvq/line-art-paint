#pragma once

#include "matrix.hpp"
#include "dinic.hpp"

#include <unordered_set>
#include <vector>
#include <array>
#include <iostream>
#include <cmath>
#include <cstdint>

class Painter {
public:   
    Painter() = delete;
    Painter(const char* filename, int terminal_capacity = 23);
    ~Painter() = default;

    void paint(Matrix<unsigned char>& scribbles);
    auto imread(const char* filename) -> bool;
    auto imwrite(const std::string& filename) -> bool;

private:
    void init_gray(float gamma);
    bool add_scribbles_edges(
            Dinic<int>& graph,
            std::unordered_set<unsigned int>& used_colors,
            std::array<u_char, 3>& new_color,
            Matrix<unsigned char>& scribbles);
    bool add_drawing_edges(
            Dinic<int>& graph,
            std::vector<bool>& used_pixels);

private:
    Matrix<unsigned char> drawing_;
    Matrix<unsigned char> gray_;
    const int terminal_capacity_ {23};
};

