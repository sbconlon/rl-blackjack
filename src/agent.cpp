#include <cstdlib>
#include <iostream>
#include <map>


#include "agent.hpp"
#include "environment.hpp"
#include "verbose.hpp"

// Gets the value estimate corresponding to the given state-action pair.
// If the state-action pair hasn't been seen before, then a random value
// in the range [-1, 1] is returned.
// Input:
//    - state
//    - action
// Output:
//    - value, either averaged return from experience 
//             or a random initial value.
//
AvgReturn* Agent::getStateActionValue(const State* state, const Action action) {
    // If the state-action pair doesn't exist, then create it.
    if (m_values->find({*state, action}) == m_values->end()) {
        m_values->insert({{*state, action}, new AvgReturn()});
    }
    // Log.
    /*
    if (VERBOSE) {
        cout << "Get: (" << *state << ", " << action << ") -> " 
             << *(m_values->at({*state, action})) << endl;
    }
    */
    // Return the value in the hash associated to the state-action pair.
    return m_values->at({*state, action});
}

// This function updates the average return value for the state action pair.
//
// If the agent doesn't have a value estimate yet for the state action pair,
// then one is created in the values hash.
//
void Agent::updateStateActionValue(const State* state, const Action action, const double rtrn) {
    // Lookup the state-action pair in the agent's value map.
    // If the state-action pair doesn't exist, then create one.
    if (m_values->find({*state, action}) == m_values->end())
        m_values->insert({{*state, action}, new AvgReturn()});
    // Log before update
    if (VERBOSE) {
        cout << "Updating value est. w/ " << rtrn << endl;
        cout << "Before: (" << *state << ", " << action << ") -> " 
             << *(m_values->at({*state, action})) << endl;
    }
    // Update the value estimate for the state-action pair.
    m_values->at({*state, action})->update(rtrn);
    // Log after update
    if (VERBOSE) {
        cout << "After: (" << *state << ", " << action << ") -> " 
             << *(m_values->at({*state, action})) << endl;
    }
}

// This function updates the value estimate corresponding to the given state-action pair.
// The 'strength' of the update is scaled by the weight parameter alpha.
//
// Input:
//  - state-action pair to be updated.
//  - the return associated with the sa-pair.
//  - the weight parameter 'alpha' which acts as a learning rate.
//
void Agent::weightedUpdateStateActionValue(const State* state, const Action action, 
                                           const double rtrn, const double alpha) {
    // Look up the state-action pair in the agent's value map.
    // If the state-action pair doesn't exist, then create one.
    if (m_values->find({*state, action}) == m_values->end()) {
        if (VERBOSE) cout << "Couldnt find " << *state << endl;
        m_values->insert({{*state, action}, new AvgReturn()});
    }
    // Update the value estimate for the state-action pair.
    double q = m_values->at({*state, action})->value();
    m_values->at({*state, action})->setValue(q + alpha*(rtrn - q));
    // Log after update
    if (VERBOSE) {
        cout << "q=" << q << " alpha=" << alpha << " rtrn=" << rtrn << endl;
        cout << "Updated value=" << m_values->at({*state, action})->value() << endl;
    }
}

map<Action, AvgReturn*> Agent::getActionValues(const State* state) {
    // Map the agent's value estimates onto the set of valid actions.
    map<Action, AvgReturn*> values;
    set<Action> actions({Hit, Stay});
    set<Action>::iterator itr;
    for (itr=actions.begin(); itr!=actions.end(); itr++) {
        values[*itr] = getStateActionValue(state, *itr);
    }
    return values;
}


// This function returns the probability of the agent taking a given action in a given state.
//
double Agent::actionProbability(const Action action, const State* state) {
    // Use the policy to select an action.
    return m_policy.actionProbability(action, getActionValues(state));
}

// Given a state, the agent must use its policy and state-action value
// estimates to select a valid action.
//
// Input:
//    - state
// Output:
//    - action to be taken determined by the agent's policy.
//
Action Agent::getAction(const State* state) {
    // Use the policy to select an action.
    return m_policy.select(getActionValues(state));
}

// Given a state, the agent must use its policy to select an action.
// This action then transfoms the state to the next state and a reward
// is recieved.
//
// Input:
//    - state: the current state
// Output:
//    - _action: the action taken by the agent
//    - _reward: the reward recieved
//    - Returns the next state after the action has been taken
//
Reward Agent::act(const State* state, Action& _action, State** _state) {
    // Use the action to transform the state to the next state.
    State* p_next_state = nullptr;
    Action action = getAction(state);
    Reward reward = transform(state, action, &p_next_state);
    // Populate the return values
    _action = action;
    *_state = p_next_state;
    return reward;
}

// Get the gready action based on the agent's state-action value estimates.
// This is intended for policy evaluation after training has completed.
// Input:
//    - current state that requires an action from the agent
// Output:
//    - the output that maximizes the agent's expected return.
//
Action Agent::getGreedyAction(const State* state) {
    // Use a greedy policy to select an action.
    GreedyPolicy greedy;
    return greedy.select(getActionValues(state));
}

// This function prints a table of the agent's greedy action for every
// valid state.
//
//
void Agent::printPolicyMatrix() {
    // Table dimensions
    // Dealer face up card values: [2, 11]
    // Player count values: [2, 20]
    int MAX_COUNT = 20; int MAX_DEALER = 11;
    int count; int dealer;
    //
    // Print hard table
    //
    // Print column labels
    cout << "--> Hard" << endl;
    cout << "   ";
    for (dealer=2; dealer<=MAX_DEALER; dealer++) {
        if (dealer == 10) {
            cout << "T";
        } else if (dealer == 11) {
            cout << "A";
        } else {
            cout << dealer;
        }
        cout << " ";
    }
    cout << endl;
    // Print rows
    State state;
    Action action;
    for (count=4; count<=MAX_COUNT; count++) {
        // Print the row label
        if (count < 10) {cout << " ";}
        cout << count << " ";
        // Print the action preferences for the row
        for (dealer=2; dealer<=MAX_DEALER; dealer++) {
            // Get the agent's greedy action
            state = State(count, dealer, 0);
            action = getGreedyAction(&state);
            //cout << "HERE" << endl;
            // Print the action abbreviation.
            if (action == Hit) {
                cout << "H ";
            } else if (action == Stay) {
                cout << "S ";
            } else {
                cout << "? ";
            }
        }
        cout << endl;
    }
    cout << endl;
    //
    // Print soft table
    //
    // Print column labels
    cout << "--> Soft" << endl;
    cout << "   ";
    for (dealer=2; dealer<=MAX_DEALER; dealer++) {
        if (dealer == 10) {
            cout << "T";
        } else if (dealer == 11) {
            cout << "A";
        } else {
            cout << dealer;
        }
        cout << " ";
    }
    cout << endl;
    // Print rows
    for (count=12; count<=MAX_COUNT; count++) {
        // Print the row label
        if (count < 10) {cout << " ";}
        cout << count << " ";
        // Print the action preferences for the row
        for (dealer=2; dealer<=MAX_DEALER; dealer++) {
            // Get the agent's greedy action
            state = State(count, dealer, 1);
            action = getGreedyAction(&state);
            // Print the action abbreviation.
            if (action == Hit) {
                cout << "H ";
            } else if (action == Stay) {
                cout << "S ";
            } else {
                cout << "? ";
            }
        }
        cout << endl;
    }
}