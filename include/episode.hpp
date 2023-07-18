#pragma once

#include <stack>
#include <tuple>

#include "agent.hpp"
#include "action.hpp"
#include "reward.hpp"
#include "state.hpp"

using namespace std;

// Each timestep, t, in the episode is represented by the triplet,
// (S_t, A_t, R_t+1) where R_t+1 is the reward experienced after
// taking the action, A_t, in the state, S_t. 
using Timestep = tuple<State*, Action, Reward>;
// Episodes are represented by the (state, action, reward) tuple
// for each timestep:
// (S_0, A_0, R_1), ..., (S_T-1, A_T-1, R_T)
using Episode = stack<Timestep>;
// Generate a episode trajectory with the given agent.
Episode* generateEpisode(Agent& agent);