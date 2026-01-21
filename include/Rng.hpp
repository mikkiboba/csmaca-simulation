#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <iostream>

class RNG {
public:

    RNG() : gen(std::random_device{}()) {}

    explicit RNG(unsigned seed) : gen(seed) {}

    int uniform_int(int a, int b) {
        std::uniform_int_distribution<int> d(a, b);
        return d(gen);
    }

    float uniform_real() {
        std::uniform_real_distribution<float> d(0.0f, 1.0f);
        return d(gen);
    }

private:

    std::mt19937 gen;
};