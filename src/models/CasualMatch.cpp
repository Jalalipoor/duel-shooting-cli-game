#include "models/CasualMatch.hpp"

#include <algorithm>
#include <format>
#include <optional>

#include "Constants.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"
#include "models/Match.hpp"
#include "models/Player.hpp"

using namespace std;

CasualMatch::CasualMatch(int id, Player* player1, Player* player2) : Match(id, player1, player2) {
    player1_bullets_ = INITIAL_BULLETS_VALUE;
    player2_bullets_ = INITIAL_BULLETS_VALUE;
}

int CasualMatch::calculateDeltaXp(int winnerXp, int loserXp) {
    double base_xp_gain = constants::BASE_XP_GAIN;
    double xp_difference_coefficient = constants::XP_DIFFERENCE_COEFFICIENT;
    double calculated_part = base_xp_gain - (xp_difference_coefficient * (winnerXp - loserXp));
    return max(constants::MINIMUM_XP_GAIN, static_cast<int>(calculated_part));
}

void CasualMatch::manageTurn() {
    if (!isTurnValid(pending_turn_)) return;

    std::string player1_action = pending_turn_.player1_action;
    std::string player2_action = pending_turn_.player2_action;

    actions_.emplace(turn_number_, pending_turn_);
    pending_turn_.clear();
    turn_number_++;

    if (player1_action == constants::SHOOT_ACTION && player2_action == constants::RELOAD_ACTION) {
        is_finished_ = true;
        winner_username_ = player1_->getUsername();
        int dXp = calculateDeltaXp(player1_->getXp(), player2_->getXp());
        player1_->recordMatchResult(true, dXp, nullopt);
        player2_->recordMatchResult(false, dXp, nullopt);
        return;
    }
    else if (player1_action == constants::RELOAD_ACTION &&
             player2_action == constants::SHOOT_ACTION) {
        is_finished_ = true;
        winner_username_ = player2_->getUsername();
        int dXp = calculateDeltaXp(player2_->getXp(), player1_->getXp());
        player2_->recordMatchResult(true, dXp, nullopt);
        player1_->recordMatchResult(false, dXp, nullopt);
        return;
    }

    if (player1_action == constants::SHOOT_ACTION) { player1_bullets_--; }
    else if (player1_action == constants::RELOAD_ACTION) { player1_bullets_++; }
    if (player2_action == constants::SHOOT_ACTION) { player2_bullets_--; }
    else if (player2_action == constants::RELOAD_ACTION) { player2_bullets_++; }
}

void CasualMatch::handleAction(const string& playerUsername, const string& actionType) {
    if (is_finished_) {
        throw PermissionDeniedException("You cannot sumbit action on a finished match.");
    }

    if (!isActionTypeValid(actionType)) {
        throw BadRequestException("Entered action type is not valid.");
    }

    if (player1_->getUsername() == playerUsername) {
        if (!pending_turn_.player1_action.empty()) {
            throw PermissionDeniedException("You've already submit your action in this turn.");
        }

        if (actionType == constants::SHOOT_ACTION && player1_bullets_ <= 0) {
            throw BadRequestException(
                format("Match '{}': You cannot shoot when your bullets number "
                       "is zero.",
                       id_));
        }
        pending_turn_.player1_action = actionType;
        manageTurn();
    }
    else if (player2_->getUsername() == playerUsername) {
        if (!pending_turn_.player2_action.empty()) {
            throw PermissionDeniedException("You've already submit your action in this turn.");
        }
        if (actionType == constants::SHOOT_ACTION && player2_bullets_ <= 0) {
            throw BadRequestException(
                format("Match '{}': You cannot shoot when your bullets number "
                       "is zero.",
                       id_));
        }
        pending_turn_.player2_action = actionType;
        manageTurn();
    }
    else { throw NotFoundException("The entered playerUsername is not playing in this match."); }
}

auto CasualMatch::getMatchStatus(const string& requestingUsername) const -> MatchStatus {
    bool is_player1_requesting;
    string your_latest_turn_action;
    string opp_latest_turn_action;
    int your_remaining_bullets;

    if (player1_->getUsername() == requestingUsername) {
        is_player1_requesting = true;
        your_remaining_bullets = player1_bullets_;
        if (pending_turn_.player1_action.empty()) { your_latest_turn_action = "pending"; }
        else { your_latest_turn_action = pending_turn_.player1_action; }
        if (pending_turn_.player2_action.empty()) { opp_latest_turn_action = "pending"; }
        else { opp_latest_turn_action = "played"; }
    }
    else if (player2_->getUsername() == requestingUsername) {
        is_player1_requesting = false;
        your_remaining_bullets = player2_bullets_;
        if (pending_turn_.player2_action.empty()) { your_latest_turn_action = "pending"; }
        else { your_latest_turn_action = pending_turn_.player2_action; }
        if (pending_turn_.player1_action.empty()) { opp_latest_turn_action = "pending"; }
        else { opp_latest_turn_action = "played"; }
    }
    else {
        throw NotFoundException("You are asking for a match status when the "
                                "requesting player isn't playing in it.");
    }

    return {MatchType::Casual,      turn_number_, is_player1_requesting, your_latest_turn_action,
            opp_latest_turn_action, actions_,     your_remaining_bullets};
}