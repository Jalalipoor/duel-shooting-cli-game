#include "models/Match.hpp"

#include "Constants.hpp"

using namespace std;

Match::Match(int id, Player* player1, Player* player2)
    : id_(id), player1_(player1), player2_(player2), pending_turn_("", "") {}

bool Match::isActionTypeValid(const string& actionType) {
    return actionType == constants::SHOOT_ACTION || actionType == constants::DEFEND_ACTION ||
           actionType == constants::RELOAD_ACTION;
}

bool Match::isTurnValid(Turn turn) {
    return isActionTypeValid(turn.player1_action) && isActionTypeValid(turn.player2_action);
}