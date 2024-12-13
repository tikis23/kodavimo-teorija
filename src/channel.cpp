#include "channel.h"

#include <random>
#include <chrono>

Channel::Channel()
    :   m_generator(std::chrono::system_clock::now().time_since_epoch().count()),
        m_distribution(0.0, 1.0) {}

vec Channel::sendVector(vec input, size_t vecSize, double p) {
    for (size_t i = 0; i < vecSize; i++) {
        if (m_distribution(m_generator) < p) input ^= vec{1} << i;
    }
    return input;
}
