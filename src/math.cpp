#include "math.h"

#include <random>
#include <chrono>
#include <bit>
#include <assert.h>

std::vector<vec> vectorsFromData(std::span<const uint8_t> data, size_t vecSize, size_t& lastVectorPadding) {
    // reserve memory for vectors
    std::vector<vec> vectors;
    size_t vectorCount = data.size() * 8 / vecSize;
    if (vectorCount * vecSize < data.size() * 8) vectorCount++;
    vectors.reserve(vectorCount);

    vec current = 0;
    size_t currentSize = 0;
    for (uint8_t byte : data) {
        for (size_t i = 0; i < 8; i++) {
            // get bit
            uint8_t bit = (byte >> (7 - i)) & 1;

            // set bit
            current = (current << 1) | bit;
            currentSize++;
            if (currentSize == vecSize) {
                vectors.push_back(current);
                current = 0;
                currentSize = 0;
            }
        }
    }

    lastVectorPadding = 0;
    if (currentSize != 0) {
        lastVectorPadding = vecSize - currentSize;
        vectors.push_back(current << lastVectorPadding);
    }
    return vectors;
}
std::vector<vec> vectorsFromString(std::string_view data, size_t vecSize, size_t& lastVectorPadding) {
    std::span<const uint8_t> span = { reinterpret_cast<const uint8_t*>(data.data()), data.size() };
    return vectorsFromData(span, vecSize, lastVectorPadding);
}
std::vector<uint8_t> vectorsToData(std::span<const vec> vecs, size_t vecSize, size_t lastVectorPadding) {
    // reserve memory for data
    std::vector<uint8_t> data;
    size_t byteCount = (vecs.size() * vecSize - lastVectorPadding) / 8;
    data.reserve(byteCount);

    uint8_t current = 0;
    size_t currentSize = 0;
    for (size_t vecIndex = 0; vecIndex < vecs.size(); vecIndex++) {
        vec vector = vecs[vecIndex];
        if (vecIndex == vecs.size() - 1) {
            vector >>= lastVectorPadding;
            vecSize -= lastVectorPadding;
        }
        for (size_t i = 0; i < vecSize; i++) {
            // get bit
            uint8_t bit = (vector >> (vecSize - 1 - i)) & 1;

            // set bit
            current = (current << 1) | bit;
            currentSize++;
            if (currentSize == 8) {
                data.push_back(current);
                current = 0;
                currentSize = 0;
            }
        }
    }
    return data;
}
std::string vectorsToString(std::span<const vec> vecs, size_t vecSize, size_t lastVectorPadding) {
    std::vector<uint8_t> data = vectorsToData(vecs, vecSize, lastVectorPadding);
    return std::string(reinterpret_cast<const char*>(data.data()), data.size());
}

matrix::matrix() : m_rows(0), m_cols(0), m_bitOffset(0), m_data() {}
matrix::matrix(size_t rows, size_t cols, bool identity)
    : m_rows(rows), m_cols(cols), m_bitOffset(64 - cols), m_data() {
    assert(rows <= 64);
    assert(cols <= 64);
    if (!identity) return;

    size_t minVal = std::min(rows, cols);
    for (size_t i = 0; i < minVal; i++) {
        setVal(i, i, 1);
    }
}

uint8_t matrix::getVal(size_t row, size_t col) const {
    assert(m_rows != 0);
    assert(m_cols != 0);
    assert(row < m_rows);
    assert(col < m_cols);
    col = 63 - (col + m_bitOffset);
    assert(col < 64);
    return (m_data[row] >> col) & 1;
}
void matrix::setVal(size_t row, size_t col, uint8_t val) {
    assert(m_rows != 0);
    assert(m_cols != 0);
    assert(row < m_rows);
    assert(col < m_cols);
    assert(val < 2);
    col = 63 - (col + m_bitOffset);
    assert(col < 64);
    m_data[row] &= ~(1ULL << col); // clear bit
    m_data[row] |= val << col; // set to val
}

vec matrix::multVectorOnRight(vec inputVec) const {
    // AND every row with vec (multiply every bit)
    // num of on bits AND 1  (sum modulo 2 all bits) 
    // set coresponding bit in result
    vec result = 0;
    for (size_t r = 0; r < m_rows; r++) {
        vec multiplied = m_data[r] & inputVec;
        vec sum = std::popcount(multiplied) & 1;
        result = (result << 1) | sum;
    }
    return result;
}

matrix matrix::extract(size_t rows, size_t cols, size_t rowOffset, size_t colOffset) const {
    assert(m_rows != 0);
    assert(m_cols != 0);
    assert(rows <= m_rows);
    assert(cols <= m_cols);
    assert(rowOffset + rows <= m_rows);
    assert(colOffset + cols <= m_cols);

    matrix m(rows, cols);
    for (size_t r = 0; r < rows; r++) {
        vec row = m_data[r + rowOffset] >> (m_cols - cols - colOffset); 
        m.m_data[r] = row & ((1ULL << cols) - 1);
    }

    return m;
}

matrix matrix::append(const matrix& other) const {
    assert(m_rows == other.m_rows);
    assert(m_cols + other.m_cols <= 64);

    matrix m(m_rows, m_cols + other.m_cols);

    // copy left
    m.m_data = m_data;

    // copy right
    for (size_t r = 0; r < m_rows; r++) {
        m.m_data[r] <<= other.m_cols;
        m.m_data[r] |= other.m_data[r];
    }

    return m;
}

matrix matrix::transpose() const {
    matrix m(m_cols, m_rows);
    for (size_t i = 0; i < m_rows; i++) {
        for (size_t j = 0; j < m_cols; j++) {
            m.setVal(j, i, getVal(i, j));
        }
    }
    return m;
}

matrix randomMatrix(size_t rows, size_t cols) {
    static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<uint8_t> distribution(0, 1);

    matrix m(rows, cols);
    for (size_t i = 0; i < m.rows(); i++) {
        for (size_t j = 0; j < m.cols(); j++) {
            m.setVal(i, j, distribution(generator));
        }
    }
    return m;
}
