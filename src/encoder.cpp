#include "encoder.h"

#include <bit>

matrix calculateControlMatrix(const matrix& g) {
    // calculate transposed A matrix
    matrix a = g.extract(g.rows(), g.cols() - g.rows(), 0, g.rows()).transpose();

    // calculate control matrix
    matrix identity = matrix(a.rows(), a.rows(), true);
    matrix control = a.append(identity);
    return control;
}

// modified to use std
// http://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation
vec nextCombination(vec v) {
    vec t = v | (v - 1); // t gets v's least significant 0 bits set to 1
    // Next set to 1 the most significant bit to change, 
    // set to 0 the least significant ones, and add the necessary 1 bits.
    vec w = (t + 1) | (((~t & -~t) - 1) >> (std::countr_zero(v) + 1));
    return w;
}

Syndromes calculateSyndromes(const matrix& h) {
    size_t n = h.cols();
    size_t syndromeCount = 1ULL << h.rows(); // 2^(n-k)
    Syndromes syndromes;
    syndromes.reserve(syndromeCount);
    syndromes[0] = 100; // 0 always exists. For now set it to something so its not default value

    if (syndromeCount == 1) { // only 1 syndrome exists, so stop now because loop will not exit early
        syndromes[0] = 0;
        return syndromes;
    }

    // iterate bit count from 1 to n
    size_t permutations = n;
    for (uint8_t i = 1; i <= n; i++) {
        vec v = (1 << i) - 1; // first combination

        // iterate all combinations
        for (size_t j = 0; j < permutations; j++) {
            // compute syndrome
            vec syndrome = h.multVectorOnRight(v);

            // add syndrome if not already present
            auto& weight = syndromes[syndrome];
            if (weight == 0) {
                weight = i;
                if (syndromes.size() >= syndromeCount) {
                    syndromes[0] = 0; // set weight for 0
                    return syndromes; // all syndromes found
                }
            }

            // get next combination
            v = nextCombination(v);
        }

        // get next permutation count
        permutations *= n - i;
        permutations /= i + 1;
    }

    return syndromes;
}

vec encode(vec input, const matrix& gTransposed) {
    return gTransposed.multVectorOnRight(input);
}

uint8_t getSyndromeWeight(const Syndromes& syndromes, vec syndrome) {
    auto it = syndromes.find(syndrome);
    if (it == syndromes.end()) return 0;
    return it->second;
}

vec decode(vec input, const Syndromes& syndromes, const matrix& h) {
    size_t n = h.cols();
    size_t k = n - h.rows();
    vec r = input;
    // algorithm in the paper loops over N bits, but because we throw out last n-k bits, we can just not proccess them,
    // because they don't affect previous bits, so we loop over K bits
    for (size_t i = 0; i < k; i++) {
        // compute H * r (syndrome)
        vec rSyndrome = h.multVectorOnRight(r);
        uint8_t rWeight = getSyndromeWeight(syndromes, rSyndrome);

        // if weight is 0, error fixed
        if (rWeight == 0) break;

        // compute H * (r + e_i) (syndrome with bit flipped)
        vec rFlipped = r ^ (1ULL << (n - i - 1));
        vec rFlippedSyndrome = h.multVectorOnRight(rFlipped);
        uint8_t rFlippedWeight = getSyndromeWeight(syndromes, rFlippedSyndrome);

        // if flipped weight is smaller, set r to r + e_i
        if (rFlippedWeight < rWeight) r = rFlipped;
    }

    // throw out n-k bits
    r >>= n - k;
    return r;
}
