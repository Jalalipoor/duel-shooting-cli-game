#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <optional>
#include <string>
#include <vector>

#include "Types.hpp"
#include "User.hpp"

class Match;

class Player : public User {
public:
    Player(const std::string& username, const std::string& password, int xp, int rp);

    int getXp() const { return xp_; }

    int getRp() const { return rp_; }

    auto getRankLevel() const -> RankLevel;

    bool hasSameRank(const Player& other) const;

    auto getCasualMatchReady() const -> bool { return casual_match_ready_; }
   
    void markReadyForCasual(bool ready) { casual_match_ready_ = ready; }
   
    void enterMatch(Match* match) { current_playing_match_ = match; }
   
    auto getPlayingMatch() const -> Match* { return current_playing_match_; }
   
    auto isInMatch() const -> bool { return current_playing_match_ != nullptr; }

    auto getProfile() const -> PlayerProfile;

    void blockPlayer(const std::string& username);

    void unblockPlayer(const std::string& username);

    bool isBlocked(const std::string& username) const;

    void recordMatchResult(bool isWin, std::optional<int> deltaXp, std::optional<int> newRp);

    void winPlayingCasualMatch(int dXp);
    
    void losePlayingCasualMatch(int dXp);

    void winPlayingRankedMatch(int newRp);
    
    void losePlayingRankedMatch(int newRp);

    void setPenalty(const Penalty& penalty, PenaltyType type);
    
    int consumeHealthPenalty();
    
    int consumeBulletPenalty();

private:
    int xp_;
    int rp_;
    bool casual_match_ready_{false};
    Match* current_playing_match_{nullptr};
    int total_wins_;
    int total_losses_;
    std::vector<std::string> blocked_players_;

    Penalty health_penalty_;
    Penalty bullet_penalty_;

    void finishMatch();
};

#endif // PLAYER_HPP_