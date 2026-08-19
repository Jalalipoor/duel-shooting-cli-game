#ifndef MATCH_HPP_
#define MATCH_HPP_

#include <string>

#include "Types.hpp"

class Player;

class Match {
protected:
    int id_;

    Player* player1_;
    Player* player2_;

    bool is_finished_{false};
    std::string winner_username_{""};

    int player1_bullets_{0};
    int player2_bullets_{0};

    int turn_number_{1};
    Turn pending_turn_;
    std::map<int, Turn> actions_;

    static bool isActionTypeValid(const std::string& actionType);
    bool isTurnValid(Turn turn);

    virtual void manageTurn() = 0;

public:
    Match(int id, Player* player1, Player* player2);
    virtual ~Match() = default;

    virtual void handleAction(const std::string& playerUsename, const std::string& actionType) = 0;

    virtual auto getMatchStatus(const std::string& requestingUsername) const -> MatchStatus = 0;

    int getId() const { return id_; }

    bool isFinished() const { return is_finished_; }
};

#endif // MATCH_HPP_