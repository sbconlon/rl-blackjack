// This file defines the dynamics of the environment
#pragma once

#include <set>
#include <stack>

#include "action.hpp"
#include "agent.hpp"
#include "reward.hpp"
#include "state.hpp"

using namespace std;

int dealCard();
bool checkTerminal(State* state);
void setStartingState(State* state);
set<Action> validActions(const State* state);
Reward endEpisode(int player_count, int dealer_count);
Reward transform(const State* state, const Action& action, State** _state);
Reward determineWinner(int player, int dealer);
Reward endGame(int player_count, int dealer_count);