#pragma once

#include <iostream>

#include "action.hpp"

using namespace std;

class State {
    private:
        // States are uniquely defined by the triplet:
        //    * count  - sum of cards in the player's hand
        //    * dealer - the card the dealer has face up
        //    * hard   - whether or not the player is holding a usable ace.
        //
        // Note - m_usable_aces is helpful for handling state transitions,
        //        but is not considered as part of the state for the monte
        //        carlo algorithm.
        int m_count;
        int m_dealer;
        int m_usable_aces;
    public:
        // Constructors
        explicit State() :
            m_count(0),
            m_dealer(0),
            m_usable_aces(0) {}
        explicit State(int count, int dealer, int usable_aces) : 
            m_count(count), 
            m_dealer(dealer), 
            m_usable_aces(usable_aces) {}
        explicit State(const State* s) :
            m_count(s->count()),
            m_dealer(s->dealer()),
            m_usable_aces(s->usableAces()) {}
        // Getters
        int count() const {return m_count;}
        int dealer() const {return m_dealer;}
        bool hard() const {return m_usable_aces == 0;}
        int usableAces() const {return m_usable_aces;}
        // Setters
        void setCount(int count) {m_count = count;}
        void setDealer(int dealer) {m_dealer = dealer;}
        void setUsableAces(int usable_aces) {m_usable_aces = usable_aces;}
        void incUsableAces() {m_usable_aces++;}
        void decUsableAces() {m_usable_aces--;}
        // Operators
        bool operator<(const State& s) const;
        bool operator==(const State& s) const;
        friend ostream& operator<<(ostream& os, const State& s);
};

// Hash function for state-action pairs.
struct SAHashFunction {
    size_t operator()(const pair<State, Action>& sa_pair) const {
        size_t count_hash = std::hash<int>()(sa_pair.first.count());
        size_t dealer_hash = std::hash<int>()(sa_pair.first.dealer());
        size_t ace_hash = std::hash<int>()(sa_pair.first.hard());
        size_t act_hash = std::hash<int>()(sa_pair.second);
        return count_hash ^ dealer_hash ^ ace_hash ^ act_hash;
    }
};