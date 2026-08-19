#ifndef CASUAL_MATCH_HPP_
#define CASUAL_MATCH_HPP_

#include "Match.hpp"
#include "Types.hpp"

class CasualMatch : public Match {
public:
    CasualMatch(int id, Player* player1, Player* player2);

    void handleAction(const std::string& playerUsername, const std::string& actionType);
    auto getMatchStatus(const std::string& requestingUsername) const -> MatchStatus;

private:
    static constexpr int INITIAL_BULLETS_VALUE = 1;

    void manageTurn();
    static int calculateDeltaXp(int winnerXp, int loserXp);
    bool isMatchFinished() const;
};

#endif // CASUAL_MATCH_HPP_