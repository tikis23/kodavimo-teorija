#pragma once

#include <unordered_map>

#include "math.h"

using Syndromes = std::unordered_map<vec, uint8_t>;

matrix calculateControlMatrix(const matrix& g);
Syndromes calculateSyndromes(const matrix& h);

vec encode(vec input, const matrix& gTransposed);
vec decode(vec input, const Syndromes& syndromes, const matrix& h);
