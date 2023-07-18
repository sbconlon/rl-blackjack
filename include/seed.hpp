#pragma once

#include <chrono>
#include <random>

using namespace std;

// Initialize seed and generator for the program.
inline unsigned seed = chrono::system_clock::now().time_since_epoch().count();
inline default_random_engine generator(seed);