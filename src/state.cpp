#include "state.hpp"

bool State::operator<(const State& s) const {
    // Compare player counts
    if (this->m_count != s.m_count)
        return this->m_count < s.m_count;
    // Compare dealer counts
    if (this->m_dealer != s.m_dealer)
        return this->m_dealer < s.m_dealer;
    // Compare whether or not the states are hard.
    return ((this->m_usable_aces > 0) < (s.m_usable_aces > 0));
}

// Note - states are defined by player count, dealer card,
//        and whether or not the player count is hard.
//        This means two states with different usable ace counts
//        are considered to be equivalent.
//
bool State::operator==(const State& s) const {
    return (
        this->m_count == s.m_count &&
        this->m_dealer == s.m_dealer &&
        (this->m_usable_aces > 0) == (s.m_usable_aces > 0)
    );
}

ostream& operator<<(ostream& os, const State& s) {
    os << "(" << s.m_count << ", " 
              << s.m_dealer << ", " 
              << s.m_usable_aces << ")";
    return os;
}