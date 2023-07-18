#include <cstdlib>
#include <stack>
#include <tuple>
#include <unordered_set>
#include <map>

#include "environment.hpp"
#include "episode.hpp"
#include "montecarlo.hpp"
#include "policy.hpp"
#include "verbose.hpp"

using namespace std;

// This function performs on-policy monte carlo 
// policy evaluation and improvement.
//
// Input:
//    - agent: the agent to be trained
//
//      Note - the function assumes the agent's policy has coverage
//             of the action space.
//
//    - niters: the number of policy evaluation-improvement loops to perform
//              before returning.
//
//    - gamma: the discount rate.
//
//      Note - this should always be 1 because blackjack is an episodic game.
//             It's included as a parameter for the sake of generality.
//
// Output:
//    - The given function's state-action values have been improved.
//
void onPolicyLearner(Agent* agent, unsigned long long niters, double gamma) {
    // Declare variables
    double rtrn;
    Timestep t;
    State* state;
    Action action;
    Reward reward;
    // Policy evaluation and iteration loop.
    Episode* episode;
    for (unsigned long long int i=0; i<niters; i++) {
        // Log percentage completion in 10% increments
        if (!VERBOSE && i % (niters/10) == 0) {
            cout << "--> " << 100*((double)i/(double)niters) << "%" << endl;
            agent->printPolicyMatrix();
            //agent->policy().printStats();
            cout << endl;
            for (int dealer=2; dealer <= 11; dealer++) {
                State test_state(17, dealer, 0);
                cout << test_state << endl;
                cout << "Hit:  " << *(agent->getStateActionValue(&test_state, Hit)) << endl;
                cout << "Stay: " << *(agent->getStateActionValue(&test_state, Stay)) << endl;
                cout << endl;
            }
        }
        // Log start of loop
        if (VERBOSE) cout << "-----------------------------------" << endl;
        // Generate an episode
        episode = generateEpisode(*agent);
        // Iteratate through the episode timesteps.
        unordered_set<pair<State, Action>, SAHashFunction> seen_before;
        rtrn = 0;
        while(!episode->empty()) {
            // Unpack timestep
            t = episode->top();
            state  = get<0>(t);
            action = get<1>(t);
            reward = get<2>(t);
            // Update average return if this is the first visit to 
            // the state-action pair.
            if (seen_before.find({*state, action}) == seen_before.end()) {
                rtrn = gamma*rtrn + reward;
                agent->updateStateActionValue(state, action, rtrn);
                seen_before.insert({*state, action});
            } else if(VERBOSE) {
                // Log seen before state
                cout << "State seen before: " << state << endl;
            }
            // Deallocate the state memory
            delete(state);
            // Remove the timestep from the episode.
            episode->pop();
        }
        delete(episode);
    }
}

// This function performs off-policy monte carlo 
// policy evaluation and improvement.
//
// The behavior policy is agent.m_policy.
// The target policy is the greedy policy.
//
// Input:
//    - agent: the agent to be trained
//
//      Note - the function assumes the agent's policy has coverage
//             of the action space.
//
//    - niters: the number of policy evaluation-improvement loops to perform
//              before returning.
//
//    - gamma: the discount rate.
//
//      Note - this should always be 1 because blackjack is an episodic game.
//             It's included as a parameter for the sake of generality.
//
// Output:
//    - The given function's state-action values have been improved.
//
void offPolicyLearner(Agent* agent, unsigned long long niters, double gamma) {
    // Declare variables
    double rtrn;
    Timestep t;
    State* state;
    Action action;
    Reward reward;
    double weight;
    // Total weight from the first n returns of the episode,
    // accumulated over all episodes experienced.
    map<pair<State, Action>, int> cum_weight;
    // Policy evaluation and iteration loop.
    Episode* episode;
    for (unsigned long long int i=0; i<niters; i++) {
        // Log percentage completion in 10% increments
        if (!VERBOSE && i % (niters/10) == 0) {
            cout << "--> " << 100*((double)i/(double)niters) << "%" << endl;
            agent->printPolicyMatrix();
        }
        // Log start of loop
        if (VERBOSE) cout << "-----------------------------------" << endl;
        // Generate an episode
        episode = generateEpisode(*agent);
        // Log.
        if (VERBOSE) { 
            cout << endl;
            cout << "Updating value estimates." << endl;
        }
        // Iteratate through the episode timesteps.
        rtrn = 0; weight = 1;
        while(!episode->empty()) {
            // Unpack timestep
            t = episode->top();
            state = get<0>(t);
            action = get<1>(t);
            reward = get<2>(t);
            // Update return
            rtrn = gamma*rtrn + reward;
            // Log.
            if (VERBOSE) {
                cout << "State: " << *state << "  Action: " << action << "  Reward: " << reward << endl;
                cout << "Return: " << rtrn << endl; 
            }
            // Update cummulative reward for the state-action pair.
            if (cum_weight.find({*state, action}) == cum_weight.end())
                cum_weight[{*state, action}] = weight;
            else
                cum_weight[{*state, action}] += weight;
            // Perform a weighted update on the agent's state-action value.
            agent->weightedUpdateStateActionValue(state, 
                                                  action, 
                                                  rtrn, 
                                                  weight/cum_weight[{*state, action}]);
            // Exit this episode if the action taken by the behavior policy
            // doesn't match the greedy (target) policy.
            if (action != agent->getGreedyAction(state)) {
                if(VERBOSE)
                    cout << "Exiting early: " << action << " != " << agent->getGreedyAction(state) << endl;
                delete(state);
                break;
            }
            // Update weight according to the probability of the behavior policy
            // taking the action in the given state.
            weight /= agent->actionProbability(action, state);
            // Deallocate the state memory
            delete(state);
            // Remove the timestep from the episode.
            episode->pop();
        }
        delete(episode);
    }
}