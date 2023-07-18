#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#include "seed.hpp"
#include "environment.hpp"
#include "verbose.hpp"

// Deals a card from the deck with replacement
// And returns the value associated with the card.
//
int dealCard() {
    vector<int> deck{2, 3, 4, 5, 6, 7, 8, 9,
                    10, 10, 10, 10, 11};
    uniform_int_distribution<> deck_idx_dist(0, deck.size()-1);
    int card = deck[deck_idx_dist(generator)];
    return card;
}

// Checks if the current state is busted.
// If the count is greater than 21, but there are usable aces,
// then the state is modified to be under 21.
//
// Input: state
//
// Output: whether or not the state is busted.
//
bool checkTerminal(State* state) {
    // If the count is less than 21 then we haven't busted.
    if (state->count() < 21) return false;
    // Check for 21.
    if (state->count() == 21) return true;
    // Else, count has gone over 21. 
    // Check if we have a usable ace.
    if (state->usableAces() > 0) {
        // Count the ace as a 1 instead of 11.
        state->setCount(state->count()-10);
        state->decUsableAces();
    }
    // If we are still at or over 21 after accounted for usable aces,
    // then return true.
    return state->count() >= 21;
}

// Initialize the given state to a starting blackjack state.
// This means dealing two cards to the player (agent) and
// one face up card to the dealer.
// Aces dealt are also accounted for in the state.
//
void setStartingState(State* state) {
    /*
    // Declare distributions.
    uniform_int_distribution<> ace_dist(1, 169);
    uniform_int_distribution<> hard_cnt_dist(4, 20);
    uniform_int_distribution<> soft_cnt_dist(12, 20);
    uniform_int_distribution<> dealer_dist(2, 11);
    // Set hard or soft state.
    // Note: this is scaled according to the probability
    // of being dealt atleast one ace, 48/169.
    state->setUsableAces(ace_dist(generator) <= 48);
    // Set player count depending on hard or soft state.
    if (state->usableAces())
        state->setCount(soft_cnt_dist(generator));
    else
        state->setCount(hard_cnt_dist(generator));
    // Set dealer count
    state->setDealer(dealer_dist(generator));
    */
    /*
    // Deal the first card to the player.
    int card1 = dealCard();
    state->setCount(card1);
    if (card1 == 11)
        state->incUsableAces();
    // Deal the second card to the player.
    int card2 = dealCard();
    state->setCount(state->count()+card2);
    if (card2 == 11)
        state->incUsableAces();
    checkTerminal(state);
    // Deal the card to the dealer.
    state->setDealer(dealCard());
    */
    state->setCount(17);
    state->setUsableAces(0);
    state->setDealer(8);
}

// Returns the set of valid actions for a given state
//
set<Action> validActions(const State* state) {
    // Note: for now there are only two actions which are always valid,
    // hit and stay. This function is included to enable support for
    // double and split actions in the future. 
    return {Hit, Stay};
}

// Return the winner
//
// Input:
// - The player and dealer's final counts
//
// Output:
// - The player's reward
Reward determineWinner(int player, int dealer) {
    //
    // Draws
    //    - player and dealer are equal
    //    - both bust
    if (
        player == dealer ||
        (player > 21 && dealer > 21)
    ) {
        return None;
    }
    //
    // Losses
    //    - player busted and dealer did not
    //    - dealer didn't bust and is closer to 21
    if (
        (player > 21 && dealer <= 21) ||
        (dealer <= 21 && dealer > player)
    ) {
        return Loss;
    }
    //
    // Wins
    //    - dealer busted and player did not
    //    - player didn't bust is closer to 21
    if (
        (dealer > 21 && player <= 21) ||
        (player <= 21 && player > dealer)
    ) {
        return Win;
    }
    // Handle error
    cerr << "Unrecognized outcome." << endl;
    abort();
}

// This function determines the final reward at the end of the episode.
// Given the player's count after completing its actions, this function
// takes the dealers count and performs the dealer's actions to completion.
// Then compares the player and dealer final counts to determine the final
// reward.
//
// Input:
// - Final player count
// - Dealer count (single face up card)
//
// Output:
// - Outcome of the blackjack hand (win, lose, or draw)
//
Reward endGame(int player_count, int dealer_count) {
    // Initialize the dealer's state using the face up card.
    State state(dealer_count, 0, dealer_count==11);
    State* p_state = &state;
    // The dealer must hit while its count is less than 17.
    int card;
    while (p_state->count() < 17) {
        // Log state.
        if (VERBOSE) cout << "Dealer state: " << *p_state << endl;
        // Draw a card.
        card = dealCard();
        // Log dealt card.
        if (VERBOSE) cout << "Card dealt: " << card << endl;
        // Update count.
        p_state->setCount(p_state->count()+card);
        // Update usable ace count.
        if (card == 11)
            p_state->incUsableAces();
        // Account for usable aces.
        checkTerminal(p_state);
    }
    // Log final player and dealer count.
    if (VERBOSE) cout << "Player: " << player_count << "  Dealer: " << p_state->count() << endl;
    // Return the reward based on who won the game.
    return determineWinner(player_count, p_state->count());
}

// This function conveys the dynamics of the environment by applying the
// given action to the given state to get the reward signal and a resulting
// next state.
// 
// Input:
//    - State for timestep t
//    - Action for timestep t
// Output:
//    - Reward recieved after taking action in state.
//    - _state points to the resultant next state
//
Reward transform(const State* state, const Action& action, State** _state) {
    // Initialize the pointer to the next state.
    State* p_next_state = NULL;
    Reward reward;
    // Apply the given action to the state
    switch(action) {
        case Hit:
            {
                // Log action.
                if (VERBOSE) cout << "Action: Hit" << endl;
                // Initialize the next state object as a shallow copy of
                // the current state.
                p_next_state = new State(state);
                // Add a card from the deck to the player's count.
                int card = dealCard();
                p_next_state->setCount(p_next_state->count()+card);
                // Log the dealt card.
                if (VERBOSE) cout << "Dealt card: " << card << endl;
                // Update next state if an ace was dealt.
                if (card == 11)
                    p_next_state->incUsableAces();
                // If we aren't in a terminal state, then set the output state ptr
                // to the next state and return no reward.
                if (!checkTerminal(p_next_state)) {
                    *_state = p_next_state;
                    return None;
                }
                // Log busted
                if (VERBOSE) cout << "Terminal state." << endl;
                // Else, we busted and must determine if we lost or got a draw.
                reward = endGame(p_next_state->count(), p_next_state->dealer());
                delete(p_next_state);
                *_state = NULL;
                return reward;
            }   
        case Stay:
            // Log action.
            if (VERBOSE) cout << "Action: Stay" << endl;
            // Determine an outcome.
            reward = endGame(state->count(), state->dealer());
            // Terminal state, next state is NULL.
            *_state = NULL;
            return reward;
        // Handle unrecognized actions
        default:
            cout << "Unrecognized action!" << endl;
            abort();
    }
}