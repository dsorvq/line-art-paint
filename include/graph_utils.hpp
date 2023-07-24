#pragma once

#include "matrix.hpp"
#include "edmonds_karp.hpp"
#include "dinic.hpp"

#include <array>
#include <unordered_set>

void add_scribble_edges(
        Dinic<int>& graph,
        const Matrix<unsigned char>& scribbles,
        int s_cap,
        std::array<u_char, 3> s_color
);
void add_scribble_edges(
        Dinic<int>& graph,
        const Matrix<unsigned char>& scribbles,
        std::unordered_set<int>& source_locations,
        int s_cap
);

void add_img_edges(
        Dinic<int>& graph,
        const Matrix<unsigned char>& img
);
