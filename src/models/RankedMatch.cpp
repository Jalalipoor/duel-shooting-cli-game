#include "models/RankedMatch.hpp"

#include <format>
#include <stdexcept>

#include "Constants.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"

using namespace std;

RankedMatch::RankedMatch(int id, Player* player1, Player* player2, int player1HealthPenalty,
                         int player2HealthPenalty, int player1BulletsPenalty,
                         int player2BulletsPenalty)
    : Match(id, player1, player2), player1_health_(INITIAL_LIVES_VALUE - player1HealthPenalty),
      player2_health_(INITIAL_LIVES_VALUE - player2HealthPenalty) {
    player1_bullets_ = INITIAL_BULLETS_VALUE - player1BulletsPenalty;
    player2_bullets_ = INITIAL_BULLETS_VALUE - player2BulletsPenalty;
}

auto RankedMatch::calculateNewRps(bool isPlayer1Win) const -> pair<int, int> {
    if (player1_->getRankLevel() != player2_->getRankLevel()) {
        throw runtime_error("A match has been started with different rps.");
    }

    auto rankLevel = player1_->getRankLevel();
    int deltaRp = constants::RANK_TO_DELTA_RP.at(rankLevel);

    if (isPlayer1Win) {
        int healthBonus = player1_health_ * constants::HEALTH_BONUS_COEFFICIENT;
        int player1_new_rp = player1_->getRp() + deltaRp + healthBonus;
        int player2_new_rp = player2_->getRp() - deltaRp;
        return {player1_new_rp, player2_new_rp};
    }
    else {
        int healthBonus = player2_health_ * constants::HEALTH_BONUS_COEFFICIENT;
        int player1_new_rp = player1_->getRp() - deltaRp;
        int player2_new_rp = player2_->getRp() + deltaRp + healthBonus;
        return {player1_new_rp, player2_new_rp};
    }
}

int RankedMatch::findWinner() {
    if (player1_health_ == 0) {
        is_finished_ = true;
        winner_username_ = player2_->getUsername();
        return 2;
    }
    else if (player2_health_ == 0) {
        is_finished_ = true;
        winner_username_ = player1_->getUsername();
        return 1;
    }
    return 0;
}

void RankedMatch::manageTurn() {
    if (!isTurnValid(pending_turn_)) return;

    string player1_action = pending_turn_.player1_action;
    string player2_action = pending_turn_.player2_action;

    actions_.emplace(turn_number_, pending_turn_);
    pending_turn_.clear();
    turn_number_++;

    if (player1_action == constants::SHOOT_ACTION && player2_action == constants::RELOAD_ACTION) {
        player2_health_--;
    }
    if (player1_action == constants::RELOAD_ACTION && player2_action == constants::SHOOT_ACTION) {
        player1_health_--;
    }

    if (player1_action == constants::SHOOT_ACTION) { player1_bullets_--; }
    else if (player1_action == constants::RELOAD_ACTION) { player1_bullets_++; }
    if (player2_action == constants::SHOOT_ACTION) { player2_bullets_--; }
    else if (player2_action == constants::RELOAD_ACTION) { player2_bullets_++; }

    int winnerNumber = findWinner();
    if (winnerNumber == 1) {
        auto const newRps = calculateNewRps(true);
        player1_->recordMatchResult(true, nullopt, newRps.first);
        player2_->recordMatchResult(false, nullopt, newRps.second);
    }
    else if (winnerNumber == 2) {
        auto const newRps = calculateNewRps(false);
        player1_->recordMatchResult(false, nullopt, newRps.first);
        player2_->recordMatchResult(true, nullopt, newRps.second);
    }
}

void RankedMatch::handleAction(const string& playerUsername, const string& actionType) {
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

auto RankedMatch::getMatchStatus(const string& requestingUsername) const -> MatchStatus {
    bool is_player1_requesting;
    string your_latest_turn_action;
    string opp_latest_turn_action;
    int your_remaining_bullets;
    int your_remaining_health;

    if (player1_->getUsername() == requestingUsername) {
        is_player1_requesting = true;
        your_remaining_bullets = player1_bullets_;
        your_remaining_health = player1_health_;
        if (pending_turn_.player1_action.empty()) { your_latest_turn_action = "pending"; }
        else { your_latest_turn_action = pending_turn_.player1_action; }
        if (pending_turn_.player2_action.empty()) { opp_latest_turn_action = "pending"; }
        else { opp_latest_turn_action = "played"; }
    }
    else if (player2_->getUsername() == requestingUsername) {
        is_player1_requesting = false;
        your_remaining_bullets = player2_bullets_;
        your_remaining_health = player2_health_;
        if (pending_turn_.player2_action.empty()) { your_latest_turn_action = "pending"; }
        else { your_latest_turn_action = pending_turn_.player2_action; }
        if (pending_turn_.player1_action.empty()) { opp_latest_turn_action = "pending"; }
        else { opp_latest_turn_action = "played"; }
    }
    else {
        throw NotFoundException("You are asking for a match status when the "
                                "requesting player isn't playing in it.");
    }

    return {MatchType::Ranked,      turn_number_, is_player1_requesting,  your_latest_turn_action,
            opp_latest_turn_action, actions_,     your_remaining_bullets, your_remaining_health};
}