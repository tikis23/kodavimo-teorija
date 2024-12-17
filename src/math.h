#pragma once

#include <stdint.h>
#include <array>
#include <vector>
#include <string_view>
#include <string>
#include <span>

using vec = uint64_t;


// Converts byte array to vectors.
// if data size is not multiple of vecSize, last vector will be smaller, so it will be padded.
// args:
//   data - byte array to convert.
//   vecSize - size of each vector in bits.
//   lastVectorPadding - gets set to number of padding bits in last vector.
// returns:
//   std::vector<vec> - data divided into vectors.
std::vector<vec> vectorsFromData(std::span<const uint8_t> data, size_t vecSize, size_t& lastVectorPadding);

// Converts vector array to byte array.
// args:
//   vecs - vectors to convert to bytes.
//   vecSize - size of each vector in bits.
//   lastVectorPadding - number of padding bits in last vector.
// returns:
//   std::vector<uint8_t> - vectors converted to bytes.
std::vector<uint8_t> vectorsToData(std::span<const vec> vecs, size_t vecSize, size_t lastVectorPadding);

// Converts string to vectors.
// if data size is not multiple of vecSize, last vector will be smaller, so it will be padded.
// args:
//   data - string to convert.
//   vecSize - size of each vector in bits.
//   lastVectorPadding - gets set to number of padding bits in last vector.
// returns:
//   std::vector<vec> - data divided into vectors.
std::vector<vec> vectorsFromString(std::string_view data, size_t vecSize, size_t& lastVectorPadding);

// Converts vectors to string.
// args:
//   vecs - vectors to convert to string.
//   vecSize - size of each vector in bits.
//   lastVectorPadding - number of padding bits in last vector.
// returns:
//   std::string - vectors converted to string.
std::string vectorsToString(std::span<const vec> vecs, size_t vecSize, size_t lastVectorPadding);

class matrix {
public:
    // constructs an empty matrix (rows = 0, cols = 0).
    matrix();

    // constructs a matrix with given dimensions.
    // args:
    //   rows - number of rows in matrix.
    //   cols - number of columns in matrix.
    //   identity - if true, creates an identity matrix.
    matrix(size_t rows, size_t cols, bool identity = false);


    // Returns number of rows.
    // returns:
    //   size_t - number of rows in matrix.
    size_t rows() const { return m_rows; }
    
    // Returns number of columns.
    // returns:
    //   size_t - number of columns in matrix.
    size_t cols() const { return m_cols; }


    // Get bit at given row and column.
    // args:
    //   row - row index.
    //   col - column index.
    // returns:
    //   uint8_t - value at specified indices (0 or 1).
    uint8_t getVal(size_t row, size_t col) const;

    // Set bit at given row and column.
    // args:
    //   row - row index.
    //   col - column index.
    //   val - value to set (0 or 1).
    void setVal(size_t row, size_t col, uint8_t val);

    // Returns a reference to matrix data.
    // returns:
    //   std::array<vec, 64>& - reference to matrix data.
    std::array<vec, 64>& data() { return m_data; }

    // Returns a const reference to matrix data.
    // returns:
    //   const std::array<vec, 64>& - reference to matrix data.
    const std::array<vec, 64>& data() const { return m_data; }

    // Multiplies matrix by vector on the right (M*v).
    // make sure yourself that if matrix has M rows and N cols, input vector has N bits (left-most unused bits MUST be 0).
    // args:
    //   inputVec - vector to multiply.
    // returns:
    //  vec - result of multiplication. Has M bits.
    vec multVectorOnRight(vec inputVec) const;

    // Combines two matrices horizontally by 'attaching' other matrix to the right.
    // Only appends columns, so number of rows must match.
    // Does not modify this matrix.
    // args:
    //   other - matrix to attach.
    // returns:
    //   matrix - new matrix with combined data.
    matrix append(const matrix& other) const;

    // Extracts a submatrix from matrix.
    // Does not modify this matrix.
    // args:
    //   rows - number of rows in submatrix.
    //   cols - number of columns in submatrix.
    //   rowOffset - row index to start from.
    //   colOffset - column index to start from.
    // returns:
    //   matrix - submatrix.
    matrix extract(size_t rows, size_t cols, size_t rowOffset, size_t colOffset) const;

    // Transposes matrix.
    // Does not modify this matrix.
    // returns:
    //   matrix - matrix with rows and columns swapped.
    matrix transpose() const;
private:
    size_t m_rows, m_cols;
    size_t m_bitOffset;
    std::array<vec, 64> m_data;
};

// Generates a random matrix.
// args:
//   rows - number of rows in matrix.
//   cols - number of columns in matrix.
// returns:
//   matrix - random matrix with given dimensions.
matrix randomMatrix(size_t rows, size_t cols);