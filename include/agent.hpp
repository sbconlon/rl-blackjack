#pragma once

#include <functional>
#include <map>
#include <utility>

#include "action.hpp"
#include "policy.hpp"
#include "state.hpp"
#include "reward.hpp"

using namespace std;

// The agent classes model the player in the blackjack game.
// They maintain estimates of state-action values based on experience,
// and choose the best actions accordingly.
class Agent {
    private:
        // Comparator for state-action hash
        // Estimates the value for each state action pair by
        // averaging the returns experienced.
        map<pair<State, Action>, AvgReturn*>* m_values;
        // Policy used by the agent to generate episodes.
        Policy& m_policy;
    public:
        // Constructors
        explicit Agent(Policy& policy) : m_policy(policy) {
            m_values = new map<pair<State, Action>, AvgReturn*>;
        }
        explicit Agent(map<pair<State, Action>, AvgReturn*>* values, 
                        Policy& policy) : m_policy(policy), m_values(values) {}
        // Deconstructor
        ~Agent() {
            // Deallocate the value estimates.
            for (auto item : *m_values)
                delete(item.second);
            // Deallocate the value map
            delete(m_values);
        }
        // Gets the agent's policy.
        Policy& policy() {return m_policy;}
        // Gets the value estimate corresponding to the given
        // state-action pair.
        AvgReturn* getStateActionValue(const State* state, const Action action);
        // Updates the value estimate corresponding to the given state-action pair.
        void updateStateActionValue(const State* state, const Action action, const double rtrn);
        // Updates the value estimate corresponding to the given state-action pair.
        // The 'strength' of the update is scaled by the weight parameter alpha.
        void weightedUpdateStateActionValue(const State* state, 
                                      const Action action, 
                                      const double rtrn,
                                      const double alpha);
        // Gets the values for action in the given state.
        map<Action, AvgReturn*> getActionValues(const State* state);
        // Gets the probability of the agent taking a given action in a given state.
        double actionProbability(const Action action, const State* state);
        // Use the agent's policy and state-action value estimates to select an action.
        Action getAction(const State* state);
        // Function through which the agent interacts with the environment.
        Reward act(const State* state, Action& _action, State** _state);
        // Gets the greedy action based on the agent's state-action value estimates
        Action getGreedyAction(const State* state);
        // Print policy preferences in a matrix format.
        void printPolicyMatrix();
};