#include <iostream>
#include <string>
#include <cstring>

#include "agent.hpp"
#include "montecarlo.hpp"

#include "environment.hpp"
#include "episode.hpp"
#include "state.hpp"
#include "verbose.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    
    cout << endl;

    // Use index to iterate through the cli args.
    int i = 1;

    // Parse whether we are using on or off policy MC.
    if (i > argc) return EXIT_FAILURE;
    if (strcmp(argv[i], "on") && strcmp(argv[i], "off")) return EXIT_FAILURE;
    bool isOnPolicy = (strcmp(argv[i], "on") == 0);
    cout << (isOnPolicy ? "On-Policy" : "Off-Policy") << endl;
    i++;

    // Parse policy and create the agent.
    if (i > argc) return EXIT_FAILURE;
    Policy* policy = NULL;
    if (strcmp(argv[i], "random") == 0) {
        cout << "Random Policy" << endl;
        policy = new RandomPolicy;
    } else if (strcmp(argv[i], "greedy") == 0) {
        cout << "Greedy Policy" << endl;
        policy = new GreedyPolicy;
    } else if (strcmp(argv[i], "egreedy") == 0) {
        i++;
        cout << "Epsilon Policy" << endl;
        cout << "e = " << argv[i] << endl;
        if (i > argc) return EXIT_FAILURE;
        policy = new EpsilonGreedyPolicy(atof(argv[i]));
    } else if (strcmp(argv[i], "ucb") == 0) {
        i++;
        cout << "Upper-Confidence Bound Policy" << endl;
        cout << "C = " << argv[i] << endl;
        if (i > argc) return EXIT_FAILURE;
        policy = new UpperConfidenceBoundPolicy(atof(argv[i]));
    } else {
        cerr << "Unrecognized policy!" << endl;
        return EXIT_FAILURE;
    }
    Agent* agent = new Agent(*policy);
    i++;

    // Parse number of policy iterations
    if(i > argc) return EXIT_FAILURE;
    unsigned long long niters = stoull(argv[i]);
    cout << "Iterations = " << argv[i] << endl;
    cout << endl;

    // Train the agent.
    isOnPolicy ? onPolicyLearner(agent, niters) : offPolicyLearner(agent, niters);

    // Print action preferences after training.
    if (!VERBOSE) {
        cout << endl;
        cout << "After training:" << endl;
        agent->printPolicyMatrix();
    }
    
    return 0;
}