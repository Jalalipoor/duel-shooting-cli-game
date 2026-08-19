#ifndef RANKED_MATCH_HPP_
#define RANKED_MATCH_HPP_

#include "Match.hpp"
#include "Player.hpp"
#include "Types.hpp"

class RankedMatch : public Match {
public:
    RankedMatch(int id, Player* player1, Player* player2, int player1HealthPenalty,
                int player2HealthPenalty, int player1BulletsPenalty, int player2BulletsPenalty);

    void handleAction(const std::string& playerUsername, const std::string& actionType);

    auto getMatchStatus(const std::string& requestingUsername) const -> MatchStatus;

private:
    static constexpr int INITIAL_BULLETS_VALUE = 3;
    static constexpr int INITIAL_LIVES_VALUE = 3;

    int player1_health_;
    int player2_health_;

    void manageTurn();

    auto calculateNewRps(bool isPlayer1Win) const -> std::pair<int, int>;

    int findWinner();
};

#endif // RANKED_MATCH_HPP_