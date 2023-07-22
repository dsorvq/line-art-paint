#pragma once

#include "matrix.hpp"
#include "edmonds_karp.hpp"

bool imwrite(const char* filename, Matrix<unsigned char>& m);
Matrix<unsigned char> imread(const char* filename);

EdmondsKarp<int> matrix_to_graph(const Matrix<unsigned char>& m);

void add_scribble_edges(
        EdmondsKarp<int>& graph,
        const Matrix<unsigned char>& scribbles,
        unsigned char s,
        int s_cap
);

void add_img_edges(
        EdmondsKarp<int>& graph,
        const Matrix<unsigned char>& img
);


