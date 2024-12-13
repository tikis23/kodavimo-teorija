#include "io.h"

#include "encoder.h"

std::string printVec(vec v, size_t bits) {
    std::string str(bits, '0');
    for (size_t i = 0; i < bits; i++) {
        str[bits - i - 1] = (v & 1) + '0';
        v >>= 1;
    }
    return str;
}
std::string printMatrix(const matrix& m) {
    std::string str;
    for (size_t r = 0; r < m.rows(); r++) {
        str += printVec(m.data()[r], m.cols());
        if (r != m.rows() - 1) str += '\n';
    }
    return str;
}

int32_t userInputChoiceArray(std::string_view prompt, const std::vector<std::string_view>& choices) {
    while (true) {
        std::print("{}:\n", prompt);
        for (size_t i = 0; i < choices.size(); i++) {
            std::print("  ({}) {}\n", i + 1, choices[i]);
        }
        std::print("Iveskite pasirinkima (1-{}): ", choices.size());

        std::string input;
        std::getline(std::cin, input);
        try {
            int32_t num = std::stoi(input);
            if (num >= 1 && num <= static_cast<int32_t>(choices.size())) return num;
            else std::print("Klaida! Ivestas skaicius turi buti tarp pasirinkimu.\n"); 
        } catch (std::invalid_argument const &e) {
            std::print("Klaida! Ivestas ne skaicius.\n");
        } catch (std::out_of_range const &e) {
            std::print("Klaida! Ivestas per didelis skaicius.\n");
        }
    }
}

bool userInputChoice(std::string_view prompt) {
    while (true) {
        std::print("{} (t/n): ", prompt);

        std::string input;
        std::getline(std::cin, input);
        if (input == "t" || input == "T") return true;
        if (input == "n" || input == "N") return false;
        std::print("Klaida! Iveskite 't' ('T') arba 'n' ('N').\n");
    }
}

matrix userInputMatrix(std::string_view prompt, size_t rows, size_t cols) {
    while (true) {
        std::print("{} ({}x{}):\n", prompt, rows, cols);
        std::vector<std::string> lines(rows);
        for (size_t i = 0; i < rows; i++) std::getline(std::cin, lines[i]);

        matrix result(rows, cols);
        for (size_t i = 0; i < rows; i++) {
            if (lines[i].size() != cols) {
                std::print("Klaida! Eilutes ilgis turi buti {}. (eilute {})\n", cols, i + 1);
                continue;
            }
            if (lines[i].find_first_not_of("01") != std::string::npos) {
                std::print("Klaida! Ivesti simboliai turi buti 0 arba 1. (eilute {})\n", i + 1);
                continue;
            }
            for (size_t j = 0; j < cols; j++) result.setVal(i, j, lines[i][j] - '0');
            if (i == rows - 1) return result;
        }
    }
}

vec userInputVector(std::string_view prompt, size_t len) {
    while (true) {
        std::print("{} (ilgis {}): ", prompt, len);

        std::string input;
        std::getline(std::cin, input);
        if (input.size() != len) {
            std::print("Klaida! Ivesto vektoriaus ilgis turi buti {}.\n", len);
            continue;
        }
        if (input.find_first_not_of("01") != std::string::npos) {
            std::print("Klaida! Ivesti simboliai turi buti 0 arba 1.\n");
            continue;
        }
        vec result = 0;
        for (char c : input) result = (result << 1) | (c - '0');
        return result;
    }
}

std::string userInputString(std::string_view promt) {
    std::string result;
    std::print("{}: ", promt);
    std::getline(std::cin, result);
    return result;
}

std::string userInputMultilineString(std::string_view promt) {
#ifdef _WIN32
    std::string eof_button = "CTRL-D + Enter";
#else
    std::string eof_button = "CTRL-D";
#endif

    std::string result;
    std::print("{} (baigti su {}):\n", promt, eof_button);
    std::string temp;
    while (std::getline(std::cin, temp)) {
        size_t eof_pos = temp.find('\x04');
        if (eof_pos != std::string::npos) {
            temp.erase(eof_pos);
            result += temp;
            return result;
        }
        result += temp + '\n';
    }
    result.pop_back(); // remove last newline
    return result;
}


CommonParams userInputCommonParameters() {
    CommonParams p;

    // parameters input
    p.n = userInputNumber<size_t>("Iveskite kodo ilgi n: ", 2, 64);
    p.k = userInputNumber<size_t>("Iveskite kodo dimensija k (k<=n): ", 1, p.n);

    // g matrix input
    size_t inputMatRows = p.k;
    size_t inputMatCols = p.n - p.k;
    matrix in(inputMatRows, inputMatCols);
    if (userInputChoice("Ar norite ivesti generuojancia matrica ranka?")) {
        in = userInputMatrix("Iveskite generuojancios matricos G dali A", inputMatRows, inputMatCols);
    } else {
        in = randomMatrix(inputMatRows, inputMatCols);
    }
    matrix identity = matrix(p.k, p.k, true);
    p.g = identity.append(in);
    p.gTransposed = p.g.transpose();
    // show g matrix
    std::print("Generuojanti matrica G:\n{}\n", printMatrix(p.g));

    // calculate control matrix and syndromes
    p.h = calculateControlMatrix(p.g);
    std::print("Generuojami sindromai ...\n");
    p.syndromes = calculateSyndromes(p.h);

    return p;
}