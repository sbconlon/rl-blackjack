// This file declares policy functions.
#pragma once

#include "action.hpp"
#include "reward.hpp"
#include "state.hpp"

#include <map>
#include <utility>

using namespace std;

// Base class
//
class Policy {
    public:
        // Constructor
        Policy() {}
        // Action selection:
        // - Input: value estimates for state-action pairs
        // - Output: state-action pair from the input set.
        virtual Action select(map<Action, AvgReturn*> values) = 0;
        // Action probabilities:
        // - Returns the probability of choosing the action given the values.
        virtual double actionProbability(Action action, map<Action, AvgReturn*> values) = 0;
        // Prints policy stats.
        virtual void printStats() = 0;
};

// Select each action with equal probability.
//
class RandomPolicy : public virtual Policy {
    public:
        RandomPolicy() {}
        Action select(map<Action, AvgReturn*> values);
        double actionProbability(Action action, map<Action, AvgReturn*> values);
        void printStats() { cout << endl; }
};

// Select the policy with the highest average return.
//
class GreedyPolicy : public virtual Policy {
    public:
        GreedyPolicy() {}
        Action select(map<Action, AvgReturn*> values);
        double actionProbability(Action action, map<Action, AvgReturn*> values);
        void printStats() { cout << endl; }
};

// Epsilon-greedy policy:
// - With probability (1-e), select the optimal action.
// - With probability e, randomly select an action.
//
class EpsilonGreedyPolicy : public virtual Policy{
    private:
        double m_e; // epsilon
        unsigned long long n_total; // number of times select has been called.
        unsigned long long n_greedy; // number of times the greedy action has been selected.
    public:
        explicit EpsilonGreedyPolicy(double e) : m_e(e) {}
        Action select(map<Action, AvgReturn*> values);
        double actionProbability(Action action, map<Action, AvgReturn*> values);
        void printStats();
};

// Upper-Confidence Bound greedy policy.
// Two factors are used in determining an action:
// 1) How close a given action is to optimal.
// 2) Our uncertainty in that estimate.
//
// Note: here the number of 'select' function calls is being
// used as a proxy for the time parameter which should traditionally
// be incremented after every episode. The two are functionally
// equivalent as the scaling term, C, can be adjusted to correct
// for the difference.
//
class UpperConfidenceBoundPolicy : public GreedyPolicy {
    private:
        unsigned long long m_t; // time elapsed
        double m_C; // scaling constant
    public:
        // Initialize starting time to 1 because ln(0) = NaN
        explicit UpperConfidenceBoundPolicy(double C) : m_t(1), m_C(C) {}
        Action select(map<Action, AvgReturn*> values);
        double actionProbability(Action action, map<Action, AvgReturn*> values);
        void printStats() { cout << endl; }
};
