#pragma once

#include <stdint.h>
#include <array>
#include <vector>
#include <string_view>
#include <string>
#include <span>

using vec = uint64_t;

// if data size is not multiple of vecSize, last vector will be smaller
std::vector<vec> vectorsFromData(std::span<const uint8_t> data, size_t vecSize, size_t& lastVectorPadding);
std::vector<vec> vectorsFromString(std::string_view data, size_t vecSize, size_t& lastVectorPadding);
std::vector<uint8_t> vectorsToData(std::span<const vec> vecs, size_t vecSize, size_t lastVectorPadding);
std::string vectorsToString(std::span<const vec> vecs, size_t vecSize, size_t lastVectorPadding);

class matrix {
public:
    // rows,cols MUST be <= 64
    matrix();
    matrix(size_t rows, size_t cols, bool identity = false);

    size_t rows() const { return m_rows; }
    size_t cols() const { return m_cols; }

    uint8_t getVal(size_t row, size_t col) const;
    void setVal(size_t row, size_t col, uint8_t val);
    std::array<vec, 64>& data() { return m_data; }
    const std::array<vec, 64>& data() const { return m_data; }

    // make sure yourself that if matrix has M rows and N cols,
    // input vector must have N bits (left-most unused bits MUST be 0).
    // returns vector with M bits
    vec multVectorOnRight(vec inputVec) const;

    matrix append(const matrix& other) const;
    matrix extract(size_t rows, size_t cols, size_t rowOffset, size_t colOffset) const;
    matrix transpose() const;
private:
    size_t m_rows, m_cols;
    size_t m_bitOffset;
    std::array<vec, 64> m_data;
};

matrix randomMatrix(size_t rows, size_t cols);