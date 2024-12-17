#pragma once

#include <iostream>
#include <print>
#include <string_view>
#include <string>
#include <vector>

#include "math.h"
#include "encoder.h"

// prints vector to string.
// args:
//   v - vector to print.
//   bits - number of bits to print.
// returns:
//   std::string - vector printed to string, e.g. "1010".
std::string printVec(vec v, size_t bits);

// prints matrix to string.
// args:
//   m - matrix to print.
// returns:
//   std::string - matrix printed to string.
std::string printMatrix(const matrix& m);

// Promts user to choose an option from a list.
// args:
//   prompt - message to show before choices.
//   choices - array of choices.
// returns:
//   int32_t - index of choice in 'choices' argument. Starts from 1.
int32_t userInputChoiceArray(std::string_view prompt, const std::vector<std::string_view>& choices);

// Promts user to choose between 't' and 'n' options.
// args:
//   prompt - message to show before choices.
// returns:
//   bool - true if 't' was chosen, false if 'n' was chosen.
bool userInputChoice(std::string_view prompt);

// Promts user to input a matrix.
// args:
//   prompt - message to show before input.
//   rows - number of rows in matrix.
//   cols - number of columns in matrix.
// returns:
//   matrix - matrix entered by user.
matrix userInputMatrix(std::string_view prompt, size_t rows, size_t cols);

// Promts user to input a vector.
// args:
//   prompt - message to show before input.
//   len - number of bits in vector.
// returns:
//   vec - vector entered by user.
vec userInputVector(std::string_view prompt, size_t len);

// Promts user to input a string.
// args:
//   prompt - message to show before input.
// returns:
//   std::string - string entered by the user.
std::string userInputString(std::string_view promt);

// Promts user to input a multiline string.
// args:
//   prompt - message to show before input.
// returns:
//   std::string - multiline string entered by the user.
std::string userInputMultilineString(std::string_view promt);

// this struct stores common parameters used in all scenarios.
struct CommonParams {
    size_t n, k;
    matrix g, h, gTransposed;
    Syndromes syndromes;
};

// Promts user to input common to all scenarios parameters.
// returns:
//   CommonParams - common parameters entered by user.
CommonParams userInputCommonParameters();

// Promts user to input a number.
// template args:
//   T - type of number to input. Should be passed explicitly to avoid ambiguity.
// args:
//   prompt - message to show before input.
//   min - minimum number allowed.
//   max - maximum number allowed.
// returns:
//   T - number entered by user.
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
