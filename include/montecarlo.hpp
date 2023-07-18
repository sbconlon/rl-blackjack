#pragma once

#include "agent.hpp"

// This function performs on-policy monte carlo policy evaluation and improvement.
void onPolicyLearner(Agent* agent, unsigned long long niters, double gamma=1);

// This function performs off-policy monte carlo policy evaluation and improvement.
void offPolicyLearner(Agent* agent, unsigned long long niters, double gamma=1);