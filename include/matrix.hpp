#pragma once

#include <string_view>
#include <iostream>
#include <vector>
#include <algorithm>

struct Shape {
    size_t height {};
    size_t width {};
    size_t channels {};

    Shape() = default;

    auto operator==(const Shape& b) const -> bool {
        return height == b.height and width == b.width and channels == b.channels;
    }
    
    auto operator=(const Shape& b) -> Shape& {
        height = b.height;
        width = b.width;
        channels = b.channels;
        return *this;
    }
};

template <class scalar_t>
class Matrix {
public:
    Matrix() = default;
    Matrix(size_t height, size_t width, size_t channels, scalar_t val = 0);
    Matrix(const Matrix<scalar_t>& b);
    Matrix(size_t height, size_t width, size_t channels, std::vector<scalar_t>&& data);

    ~Matrix() = default;

    auto operator=(const Matrix<scalar_t>& b) -> Matrix<scalar_t>&;
    auto copy() const -> Matrix<scalar_t>;

    auto shape() const -> Shape;
    auto size() const -> size_t;
    auto height() const -> size_t;
    auto width() const -> size_t;
    auto channels() const -> size_t;
    bool empty() {return data_.empty();}

    auto operator()(size_t row, size_t col, size_t channel = 0) -> scalar_t&;
    auto operator()(size_t row, size_t col, size_t channel = 0) const -> const scalar_t&;
    auto pt() -> scalar_t*;
    auto pt() const -> const scalar_t*;

    auto operator==(const Matrix<scalar_t>& b) const -> bool;

private:
    std::vector<scalar_t> data_;
    Shape shape_ {};
};


// IMPLEMENTATION //
template <class scalar_t>
Matrix<scalar_t>::Matrix(size_t height, size_t width, size_t channels, scalar_t val) 
    : shape_{height, width, channels}
{
    data_.assign(size(), val);
}

template <class scalar_t>
Matrix<scalar_t>::Matrix(const Matrix<scalar_t>& b) 
    : data_ {b.data_}
    , shape_ {b.shape()}
{ }

template <class scalar_t>
Matrix<scalar_t>::Matrix(
        size_t height, 
        size_t width, 
        size_t channels, 
        std::vector<scalar_t>&& data)
    : data_(std::move(data))
    , shape_{height, width, channels}
    
{ }

template <class scalar_t>
auto Matrix<scalar_t>::operator=(const Matrix<scalar_t>& b) -> Matrix<scalar_t>& {
    data_ = b.data_;
    shape_ = b.shape();
    return *this;
}

template <class scalar_t>
auto Matrix<scalar_t>::copy() const -> Matrix<scalar_t> {
    return {*this};
}

template <class scalar_t>
auto Matrix<scalar_t>::shape() const -> Shape {
    return shape_;    
}

template <class scalar_t>
auto Matrix<scalar_t>::size() const -> size_t {
    return shape_.height * shape_.width * shape_.channels;    
}

template <class scalar_t>
auto Matrix<scalar_t>::height() const -> size_t {
    return shape_.height;
}

template <class scalar_t>
auto Matrix<scalar_t>::width() const -> size_t {
    return shape_.width;
}

template <class scalar_t>
auto Matrix<scalar_t>::channels() const -> size_t {
    return shape_.channels;
}

template <class scalar_t>
auto Matrix<scalar_t>::operator()(size_t row, size_t col, size_t channel) -> scalar_t& {
    return data_[row*width()*channels() + col*channels() + channel];
}

template <class scalar_t>
auto Matrix<scalar_t>::operator()(size_t row, size_t col, size_t channel) const -> const scalar_t& {
    return data_[row*width()*channels() + col*channels() + channel];
}

template <class scalar_t>
auto Matrix<scalar_t>::pt() -> scalar_t* {
    return data_.data();
}

template <class scalar_t>
auto Matrix<scalar_t>::pt() const -> const scalar_t* {
    return data_.data();
}

template <class scalar_t>
auto Matrix<scalar_t>::operator==(const Matrix<scalar_t>& b) const -> bool {
    return 
        shape() == b.shape() 
        and std::equal(begin(data_), end(data_), begin(b.data_), end(b.data_));
}
