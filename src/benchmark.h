#pragma once

#include <print>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <iterator>
#include <format>

#include "math.h"
#include "encoder.h"
#include "channel.h"

struct batch {
    std::vector<double> successfulDecodeRates = {};
    double syndromeGenTimeMs = -1;
    double testRunTimeMs = -1;
    int64_t totalVecCount = -1;
};

batch runSingleNK(const std::vector<double>& errorRates, size_t n, size_t k, Channel c) {
    batch result{};
    size_t inputMatRows = k;
    size_t inputMatCols = n - k;
    matrix g = matrix(k, k, true).append(randomMatrix(inputMatRows, inputMatCols));
    matrix gTransposed = g.transpose();
    matrix h = calculateControlMatrix(g);

    std::print("N: {}, K: {}\n", n, k);
    std::print("  Generating syndromes... ");
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    Syndromes syndromes = calculateSyndromes(h);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    result.syndromeGenTimeMs = std::chrono::duration<double, std::milli>(end - begin).count();
    std::print("Syndromes generated ({}ms)\n", result.syndromeGenTimeMs);

    size_t vecCount = 125'000;
    size_t totalVecCount = vecCount * errorRates.size();
    result.totalVecCount = totalVecCount;
    begin = std::chrono::steady_clock::now();
    for (double p : errorRates) {
        size_t errors = 0;
        for (size_t i = 0; i < vecCount; i++) {
            vec original = std::rand() % (1ULL << k);
            vec r = encode(original, gTransposed);
            r = c.sendVector(r, n, p);
            vec out = decode(r, syndromes, h);
            if (original != out) errors++;
        }

        double errRate = static_cast<double>(errors) / vecCount;
        result.successfulDecodeRates.push_back((1.0 - errRate) * 100.0);
    }
    end = std::chrono::steady_clock::now();
    result.testRunTimeMs = std::chrono::duration<double, std::milli>(end - begin).count();
    std::print("  Tests ran ({}ms)\n", result.testRunTimeMs);
    return result;
}

void benchmark(const std::vector<double>& errorRates, size_t maxN, size_t maxK) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::vector<std::unordered_map<vec, std::unordered_map<vec, batch>>> results;
    results.resize(2);
    Channel c;
    for (auto& res : results) {
        for (size_t n = 2; n < maxN; n++) {
            for (size_t k = 1; k <= n && k < maxK; k++) {
                res[n][k] = runSingleNK(errorRates, n, k, c);
            }
        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::print("Benchmark finished in {}ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

    std::ofstream file("results.txt");
    std::ostream_iterator<char> fileOut(file);

    std::format_to(fileOut, "ERROR_RATES=");
    for (double p : errorRates) {
        std::format_to(fileOut, "{}{}", p, p == errorRates.back() ? "\n" : " ");
    }
    std::format_to(fileOut, " N | K | VEC_COUNT | SYNDROME_GEN_TIME_MS | TEST_RUN_TIME_MS | DECODE_RATES\n");
    for (size_t n = 2; n < maxN; n++) {
        for (size_t k = 1; k <= n && k < maxK; k++) {
            batch b = results[0][n][k];
            for (size_t i = 1; i < results.size(); i++) {
                for (size_t j = 0; j < results[i][n][k].successfulDecodeRates.size(); j++) {
                    b.successfulDecodeRates[j] += results[i][n][k].successfulDecodeRates[j];
                }
                b.syndromeGenTimeMs += results[i][n][k].syndromeGenTimeMs;
                b.testRunTimeMs += results[i][n][k].testRunTimeMs;
            }
            for (double& rate : b.successfulDecodeRates) {
                rate /= results.size();
            }
            b.syndromeGenTimeMs /= results.size();
            b.testRunTimeMs /= results.size();

            std::format_to(fileOut, "{} {} {} {:f} {:f} ", n, k, b.totalVecCount, b.syndromeGenTimeMs, b.testRunTimeMs);
            for (size_t i = 0; i < b.successfulDecodeRates.size(); i++) {
                std::format_to(fileOut, "{:f}{}", b.successfulDecodeRates[i], i == b.successfulDecodeRates.size() - 1 ? "\n" : " ");
            }
        }
    }
}