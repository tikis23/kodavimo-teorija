#pragma once

#include <unordered_map>

#include "math.h"

using Syndromes = std::unordered_map<vec, uint8_t>;

// Calculates control matrix from generator matrix.
// args:
//   g - generator matrix. It should have more than 1 row.
// returns:
//   matrix - control matrix.
matrix calculateControlMatrix(const matrix& g);

// Calculates syndromes used in decoding.
// args:
//   h - control matrix.
// returns:
//   Syndromes - map of syndromes and their associated weight.
Syndromes calculateSyndromes(const matrix& h);

// Encodes input vector using generator matrix.
// Uses transposed generator matrix for faster encoding.
// args:
//   input - vector to encode.
//   gTransposed - transposed generator matrix. If input is k bits long, gTransposed must have k columns.
// returns:
//   vec - encoded vector. It will be gTransposed.rows() bits long.
vec encode(vec input, const matrix& gTransposed);

// Decodes input vector using syndromes and control matrix.
// args:
//   input - vector to decode.
//   syndromes - syndromes used in decoding.
//   h - control matrix.
// returns:
//   vec - decoded vector.
vec decode(vec input, const Syndromes& syndromes, const matrix& h);
