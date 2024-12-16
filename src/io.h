#pragma once

#include <iostream>
#include <print>
#include <string_view>
#include <string>
#include <vector>

#include "math.h"
#include "encoder.h"

std::string printVec(vec v, size_t bits);
std::string printMatrix(const matrix& m);

int32_t userInputChoiceArray(std::string_view prompt, const std::vector<std::string_view>& choices);
bool userInputChoice(std::string_view prompt);
matrix userInputMatrix(std::string_view prompt, size_t rows, size_t cols);
vec userInputVector(std::string_view prompt, size_t len);
std::string userInputString(std::string_view promt);
std::string userInputMultilineString(std::string_view promt);

struct CommonParams {
    size_t n, k;
    matrix g, h, gTransposed;
    Syndromes syndromes;
};
CommonParams userInputCommonParameters();

template <typename T>
T userInputNumber(std::string_view prompt, T min, T max) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);

        // replace ',' with '.'
        auto it = input.find(',');
        if (it != std::string::npos) input.replace(input.find(','), 1, ".");

        // check if there are more than 1 '.'
        if (std::count(input.begin(), input.end(), '.') > 1) {
            std::print("Klaida! Ivestas ne skaicius.\n");
            continue;
        }
        // check if there are non-digit characters
        if (std::find_if(input.begin(), input.end(), [](char c) {
            return !std::isdigit(c) && c != '.';
        }) != input.end()) {
            std::print("Klaida! Ivestas ne skaicius.\n");
            continue;
        }
        
        // convert to number
        try {
            T num;
            if constexpr (std::is_floating_point_v<T>) num = std::stold(input);
            else if constexpr (std::is_unsigned_v<T>) num = std::stoull(input);
            else num = std::stoll(input);
            if (num >= min && num <= max) return num;
            else std::print("Klaida! Ivestas skaicius turi priklausyti [{};{}].\n", min, max); 
        } catch (std::invalid_argument const &e) {
            std::print("Klaida! Ivestas ne skaicius.\n");
        } catch (std::out_of_range const &e) {
            std::print("Klaida! Ivestas per didelis skaicius.\n");
        }
    }
}
