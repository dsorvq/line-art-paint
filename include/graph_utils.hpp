#pragma once

#include "matrix.hpp"
#include "edmonds_karp.hpp"
#include "dinic.hpp"

#include <array>

void add_scribble_edges(
        //EdmondsKarp<int>& graph,
        Dinic<int>& graph,
        const Matrix<unsigned char>& scribbles,
        int s_cap,
        std::array<u_char, 3> s_color
);

void add_img_edges(
        //EdmondsKarp<int>& graph,
        Dinic<int>& graph,
        const Matrix<unsigned char>& img
);
