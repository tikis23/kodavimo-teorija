#pragma once

#include <random>

#include "math.h"

class Channel {
public:
    Channel();
    vec sendVector(vec input, size_t vecSize, double p);\

private:
    std::default_random_engine m_generator;
    std::uniform_real_distribution<double> m_distribution;
};