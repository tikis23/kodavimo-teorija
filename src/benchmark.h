#pragma once

#include <print>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "math.h"
#include "encoder.h"
#include "channel.h"

double runSingleNK(size_t n, size_t k, size_t maxK, Channel c) {
    size_t inputMatRows = k;
    size_t inputMatCols = n - k;
    matrix g = matrix(k, k, true).append(randomMatrix(inputMatRows, inputMatCols));
    matrix gTransposed = g.transpose();
    matrix h = calculateControlMatrix(g);

    std::print("N: {}, K: {}\n", n, k);
    std::print("  Generating syndromes... ");
    Syndromes syndromes = calculateSyndromes(h);
    std::print("Syndromes generated\n");

    size_t errors = 0;
    size_t vecCount = 0;

    double ps[] = { 0.01, 0.02, 0.05, 0.1, 0.15, 0.25, 0.4, 0.5 };
    for (double p : ps) {
        for (size_t i = 0; i < (1ULL << k); i++) {
            for (size_t j = (1ULL << (maxK - k)); j > 0; j--) {
                vec r = encode(i, gTransposed);
                r = c.sendVector(r, n, p);
                vec out = decode(r, syndromes, h);
                if (i != out) errors++;
                vecCount++;
            }
        }
    }
    double errRate = static_cast<double>(errors) / vecCount;
    std::print("  {} vectors tested, error rate: {}\n", vecCount, errRate);
    return errRate;
}

void benchmark(size_t maxN, size_t maxK) {
    std::unordered_map<vec, std::unordered_map<vec, double>> results;
    Channel c;
    for (size_t n = 2; n < maxN; n++) {
        for (size_t k = 1; k < maxK; k++) {
            results[n][k] = -0.01;
        }
        for (size_t k = 1; k < n && k < maxK; k++) {
            if (n == k) continue;
            results[n][k] = runSingleNK(n, k, maxK, c);
        }
    }


    std::print("  # K ");
    for (size_t k = 1; k < maxK; k++) {
        std::print("{:3} ", k);
    }
    std::print("\n  N |");
    for (size_t k = 1; k < maxK; k++) std::print("----");
    std::print("\n");
    for (size_t n = 2; n < maxN; n++) {
        std::print("{:3} | ", n);
        for (size_t k = 1; k < maxK; k++) {
            std::print("{:3} ", static_cast<int>(results[n][k] * 100));
        }
        std::print("\n");
    }
}