#include <iostream>

#include "agent.hpp"
#include "environment.hpp"
#include "episode.hpp"
#include "verbose.hpp"

// This function generates a episode using the given agent.
Episode* generateEpisode(Agent& agent) {
    // Log function call.
    if (VERBOSE) cout << "Generating a new episode." << endl;
    // Declare episode variables.
    Episode* episode = new Episode;
    Timestep t;
    State* state = new State; 
    State* next_state = NULL;
    Action action;
    Reward reward;
    // Initialize state to a valid starting state.
    // Note: excluding dealt blackjacks as starting states.
    do {
        setStartingState(state);
    } while (state->count() == 21);
    // While the state is non-terminal.
    while(state != NULL) {
        // Log state.
        if (VERBOSE) cout << "State: " << *state << endl;
        // Make an action
        reward = agent.act(state, action, &next_state);
        // Log the reward.
        if (VERBOSE) cout << "Reward: " << reward << endl;
        // Add the (state, action, reward) tuple to the episode.
        t = make_tuple(state, action, reward);
        episode->push(t);
        // Advance to the next state.
        state = next_state;
    }
    return episode;
}