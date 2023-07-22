#include <gtest/gtest.h>

#include <matrix.hpp>

#include <gtest/gtest.h>

class MatrixTest : public ::testing::Test {
protected:

};

// Test the Matrix constructor with default value initialization
TEST_F(MatrixTest, DefaultConstructor) {
    Matrix<int> matrix;
    EXPECT_EQ(matrix.height(), 0);
    EXPECT_EQ(matrix.width(), 0);
    EXPECT_EQ(matrix.channels(), 0);
    EXPECT_TRUE(matrix.empty());
}

// Test the Matrix constructor with specified size and value initialization
TEST_F(MatrixTest, ConstructorWithSizeAndValue) {
    const size_t height = 3;
    const size_t width = 4;
    const size_t channels = 2;
    const int default_value = 42;

    Matrix<int> matrix(height, width, channels, default_value);

    EXPECT_EQ(matrix.height(), height);
    EXPECT_EQ(matrix.width(), width);
    EXPECT_EQ(matrix.channels(), channels);

    // Check that all elements have been initialized with the default value
    for (size_t h = 0; h < height; ++h) {
        for (size_t w = 0; w < width; ++w) {
            for (size_t c = 0; c < channels; ++c) {
                EXPECT_EQ(matrix(h, w, c), default_value);
            }
        }
    }
}

// Test the Matrix equality operator
TEST_F(MatrixTest, EqualityOperator) {
    const size_t height = 2;
    const size_t width = 2;
    const size_t channels = 1;
    std::vector<int> data = {1, 2, 3, 4};

    Matrix<int> matrix1(height, width, channels, std::move(data));
    Matrix<int> matrix2(height, width, channels, std::vector<int>{1, 2, 3, 4});

    // Both matrices have the same data, so they should be equal
    EXPECT_TRUE(matrix1 == matrix2);
}

// Test the Matrix copy constructor
TEST_F(MatrixTest, CopyConstructor) {
    const size_t height = 3;
    const size_t width = 4;
    const size_t channels = 2;
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    Matrix<int> original_matrix(height, width, channels, std::move(data));
    Matrix<int> copied_matrix(original_matrix);

    EXPECT_EQ(original_matrix.shape(), copied_matrix.shape());

    EXPECT_TRUE(original_matrix == copied_matrix);
}

// Test the Matrix assignment operator
TEST_F(MatrixTest, AssignmentOperator) {
    const size_t height = 2;
    const size_t width = 2;
    const size_t channels = 1;
    std::vector<int> data = {1, 2, 3, 4};

    Matrix<int> original_matrix(height, width, channels, std::move(data));
    Matrix<int> copied_matrix;
    copied_matrix = original_matrix;

    EXPECT_EQ(original_matrix.shape(), copied_matrix.shape());

    EXPECT_TRUE(original_matrix == copied_matrix);
}

// Test the Matrix element access operators
TEST_F(MatrixTest, ElementAccess) {
    const size_t height = 2;
    const size_t width = 2;
    const size_t channels = 1;
    std::vector<int> data = {1, 2, 3, 4};

    Matrix<int> matrix(height, width, channels, std::move(data));

    // Test element access and modification
    EXPECT_EQ(matrix(0, 0), 1);
    EXPECT_EQ(matrix(0, 1), 2);
    EXPECT_EQ(matrix(1, 0), 3);
    EXPECT_EQ(matrix(1, 1), 4);

    matrix(0, 0) = 10;
    EXPECT_EQ(matrix(0, 0), 10);
}

// Test the Matrix copy function
TEST_F(MatrixTest, CopyFunction) {
    const size_t height = 3;
    const size_t width = 3;
    const size_t channels = 1;
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    Matrix<int> original_matrix(height, width, channels, std::move(data));
    Matrix<int> copied_matrix = original_matrix.copy();

    EXPECT_EQ(original_matrix.shape(), copied_matrix.shape());

    EXPECT_TRUE(original_matrix == copied_matrix);
}
