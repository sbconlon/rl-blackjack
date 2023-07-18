#include <chrono>
#include <cmath>
#include <float.h>
#include <random>

#include "seed.hpp"
#include "policy.hpp"


// This selection function randomly selects randomly from 
// the action set with uniform probability.
//
Action RandomPolicy::select(map<Action, AvgReturn*> values) {
    // Pick a random index
    uniform_int_distribution<> distribution(0, values.size()-1);
    int idx = distribution(generator);
    // Get the element from the map w/ the index
    int cnt = 0;
    map<Action, AvgReturn*>::iterator it = values.begin();
    for (int cnt=0; cnt < idx; cnt++) {
        ++it;
    }
    // Return the element's action
    return it->first;
}

// The random policy selects actions with equal probability.
//
double RandomPolicy::actionProbability(Action action, map<Action, AvgReturn*> values) {
    return 1.0 / values.size();
}

// This selection function selects the action with the greatest
// value.
//
Action GreedyPolicy::select(map<Action, AvgReturn*> values) {
    // Perform argmax on the given map of value estimates
    auto it =  max_element(values.begin(), values.end(),
        [](const pair<Action, AvgReturn*> p1, const pair<Action, AvgReturn*> p2) {
            return p1.second->value() < p2.second->value();
        }
    );
    return it->first;
}

// The greedy policy selects the greedy action 100% of the time.
//
double GreedyPolicy::actionProbability(Action action, map<Action, AvgReturn*> values) {
    return (action == select(values));
}

// This selection function selects...
//    - the greedy action w/ prob = 1-e
//    - a random action w/ prob = e
//
Action EpsilonGreedyPolicy::select(map<Action, AvgReturn*> values) {
    n_total++;
    // Generate a random number between 0 and 1
    uniform_real_distribution<double> distribution(0, 1);
    double x = distribution(generator);
    // Follow the greedy action with prob 1-e
    if (x > m_e) {
        n_greedy++;
        GreedyPolicy greedy;
        return greedy.select(values);
    } else {
        RandomPolicy random;
        return random.select(values);
    }
}

// e-Greedy selects:
//  - the greedy action w/ probability = (1 - e + e/size)
//  - the non-greedy action w/ probability = (e/size)
//  Where 'size' is the size of the valid action set.
//
double EpsilonGreedyPolicy::actionProbability(Action action, map<Action, AvgReturn*> values) {
    GreedyPolicy greedy;
    if (action == greedy.select(values))
        return (1-m_e+(m_e/values.size()));
    else
        return m_e/values.size();
}

// Prints the percentage of time the greedy action has been selected.
// Helpful when debugging.
//
void EpsilonGreedyPolicy::printStats() {
    cout << "Greedy frequency: " << (double) n_greedy / n_total << endl;
}

// Upper-confidence bound greedy policy.
// Two factors are used in determining an action:
// 1) How close a given action is to optimal.
// 2) Our uncertainty in that estimate.
//
Action UpperConfidenceBoundPolicy::select(map<Action, AvgReturn*> values) {
    // Get the action corresponding to the highest value.
    Action best_action;
    double max_value = -DBL_MAX;
    double value;
    for (auto const& [action, rtrn] : values) {
        if (m_t % 20000 == 0)
            cout << "value: " << rtrn->value() << "   adjustment: " <<  m_C*sqrt(log(m_t)/((double)rtrn->samples()+1)) << endl;
        value = rtrn->value() + m_C*sqrt(log(m_t)/((double)rtrn->samples()+1));
        if (value > max_value) {
            max_value = value;
            best_action = action;
        }
    }
    // Increment the time after each select call.
    m_t++;
    // Return the best action.
    return best_action;
}

// UCB is a deterministic policy so it selects the best action 100% of the time.
//
double UpperConfidenceBoundPolicy::actionProbability(Action action, 
                                                     map<Action, AvgReturn*> values) {
    return action == select(values);
}