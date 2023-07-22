#pragma once

#include "matrix.hpp"
#include "edmonds_karp.hpp"

void add_scribble_edges(
        EdmondsKarp<int>& graph,
        const Matrix<unsigned char>& scribbles,
        int s_cap
);

void add_img_edges(
        EdmondsKarp<int>& graph,
        const Matrix<unsigned char>& img
);
