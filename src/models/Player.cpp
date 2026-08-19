#include "models/Player.hpp"

#include <algorithm>
#include <stdexcept>

#include "Types.hpp"

using namespace std;

Player::Player(const string& username, const string& password, int xp, int rp)
    : User(username, password), xp_(xp), rp_(rp), total_wins_(0), total_losses_(0),
      health_penalty_(0, 0), bullet_penalty_(0, 0) {}

auto Player::getRankLevel() const -> RankLevel {
    if (rp_ < 1400) return RankLevel::Bronze;
    if (rp_ < 1750) return RankLevel::Silver;
    if (rp_ < 2250) return RankLevel::Golden;
    else return RankLevel::Platinum;
}

auto Player::hasSameRank(const Player& other) const -> bool {
    return this->getRankLevel() == other.getRankLevel();
}

void Player::finishMatch() { current_playing_match_ = nullptr; }

void Player::recordMatchResult(bool isWin, optional<int> deltaXp, optional<int> newRp) {
    if (deltaXp.has_value()) {
        if (isWin) {
            xp_ += deltaXp.value();
            total_wins_++;
        }
        else {
            xp_ -= deltaXp.value();
            total_losses_++;
        }
        finishMatch();
    }
    else if (newRp.has_value())
    {
        rp_ = newRp.value();
        (isWin) ? total_wins_++ : total_losses_++;
        finishMatch();
    }
    else {
        throw runtime_error("Record match result called without data in new rp or delta xp.");
    }
}

void Player::winPlayingRankedMatch(int newRp) {
    rp_ = newRp;
    total_wins_++;
    current_playing_match_ = nullptr;
}

void Player::losePlayingRankedMatch(int newRp) {
    rp_ = newRp;
    total_losses_++;
    current_playing_match_ = nullptr;
}

auto Player::getProfile() const -> PlayerProfile {
    return {getUsername(), xp_, rp_, getRankLevel(), total_wins_, total_losses_};
}

void Player::blockPlayer(const string& username) {
    auto it = ranges::find(blocked_players_, username);
    if (it == blocked_players_.end()) { blocked_players_.push_back(username); }
}

void Player::unblockPlayer(const string& username) { erase(blocked_players_, username); }

bool Player::isBlocked(const string& username) const {
    auto it = ranges::find(blocked_players_, username);
    return it != blocked_players_.end();
}

int Player::consumeHealthPenalty() {
    if (health_penalty_.amount == 0) return 0;

    if (health_penalty_.current_remaining_matches <= 0) {
        health_penalty_ = Penalty(0, 0);
        return 0;
    }

    health_penalty_.current_remaining_matches--;
    return health_penalty_.amount;
}

int Player::consumeBulletPenalty() {
    if (bullet_penalty_.amount == 0) return 0;

    if (bullet_penalty_.current_remaining_matches <= 0) {
        bullet_penalty_ = Penalty(0, 0);
        return 0;
    }

    bullet_penalty_.current_remaining_matches--;
    return bullet_penalty_.amount;
}

void Player::setPenalty(const Penalty& penalty, PenaltyType type) {
    switch (type) {
        case PenaltyType::Health:
            health_penalty_ = penalty;
            break;
        case PenaltyType::Bullet:
            bullet_penalty_ = penalty;
            break;
    }
}