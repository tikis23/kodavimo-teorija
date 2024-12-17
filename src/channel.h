#pragma once

#include <random>

#include "math.h"

class Channel {
public:
    // Constructs a channel with default a random seed.
    Channel();

    // Sends a vector through the channel and flips bits with probability p.
    // args:
    //   input - vector to send.
    //   vecSize - size of vector in bits.
    //   p - probability of errors.
    // returns:
    //   vec - received vector.
    vec sendVector(vec input, size_t vecSize, double p);\

private:
    std::default_random_engine m_generator;
    std::uniform_real_distribution<double> m_distribution;
};