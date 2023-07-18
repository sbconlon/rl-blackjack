#pragma once

#include <chrono>
#include <random>
#include <iostream>

#include "seed.hpp"

using namespace std;

// The possible rewards in the game.
enum Reward {
    Win = 1,
    Loss = -1,
    None = 0
};

// This class is used to store the average return
// observed for a given state-action pair.
class AvgReturn {
    private:
        // Store the numerator (n) and denominator (total)
        // for the average.
        long long m_total_returns;
        unsigned long long m_samples;
        double m_value;
    public:
        // Constructor
        //
        // Before we have any observations, initialize the value to
        // a random number in our value range, [-1, 1].
        //
        AvgReturn() {
            // Initialize observations to zeros.
            m_total_returns = 0; m_samples = 0;
            // Generate a random number for the initial value estimate
            uniform_real_distribution<double> distribution(-1, 1);
            m_value = distribution(generator);
        }
        // Setter
        //
        void update(int sample_return) {
            m_total_returns += sample_return;
            m_samples++;
            m_value = (double) m_total_returns/ m_samples;
        }
        void setValue(double value) {
            m_value = value;
        }
        // Getters
        //
        double value() {
            return m_value;
        }
        long long samples() {
            return m_samples;
        }
        // Print
        //
        friend ostream& operator<<(ostream& os, AvgReturn& ar) {
            os << ar.m_value << " = (" << ar.m_total_returns << "/" << ar.m_samples << ")";
            return os;
        }
};