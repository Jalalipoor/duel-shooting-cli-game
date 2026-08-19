#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <map>
#include <string>

enum class MatchType { Casual, Ranked };

enum class SortOrder { Ascending, Descending };

enum class RankLevel { Bronze, Silver, Golden, Platinum };

enum class InvitationStatus { Pending, Accepted, Declined };

enum class BlockStatus { Blocked, Unblocked };

enum class PenaltyType { Health, Bullet };

struct PlayerData {
    std::string username;
    std::string password;
    int xp;
    int rp;

    PlayerData(std::string username, std::string password, int xp, int rp)
        : username(std::move(username)), password(std::move(password)), xp(xp), rp(rp) {}
};

struct AdminData {
    std::string username;
    std::string password;

    AdminData(std::string username, std::string password)
        : username(std::move(username)), password(std::move(password)) {}
};

struct Command {
    std::string method, cmd;
    std::map<std::string, std::string> args;
};

struct CasualPlayerInfo {
    std::string username;
    int xp;

    CasualPlayerInfo(std::string username, int xp) : username(std::move(username)), xp(xp) {}
};

struct RankedPlayerInfo {
    std::string username;
    int rp;

    RankedPlayerInfo(std::string username, int rp) : username(std::move(username)), rp(rp) {}
};

struct Turn {
    std::string player1_action;
    std::string player2_action;

    void clear() {
        player1_action.clear();
        player2_action.clear();
    }

    Turn(std::string player1Action, std::string player2Action)
        : player1_action(std::move(player1Action)), player2_action(std::move(player2Action)) {}
};

struct MatchStatus {
    MatchType type;
    int turn_number;
    bool is_player1_requesting;
    std::string your_latest_turn_action;
    std::string opp_latest_turn_action;
    std::map<int, Turn> actions_history;
    int your_remaining_bullets;
    int your_remaining_health{-1};

    MatchStatus(MatchType matchType, int turnNumber, bool isPlayer1Requesting,
                std::string yourLatestTurnAction, std::string oppLatestTurnAction,
                std::map<int, Turn> actions, int yourRemainingBullets, int yourRemainingHealth = -1)
        : type(matchType), turn_number(turnNumber), is_player1_requesting(isPlayer1Requesting),
          your_latest_turn_action(std::move(yourLatestTurnAction)),
          opp_latest_turn_action(std::move(oppLatestTurnAction)),
          actions_history(std::move(actions)), your_remaining_bullets(yourRemainingBullets),
          your_remaining_health(yourRemainingHealth) {}
};

struct InvitationInfo {
    int id;
    std::string sender;
    MatchType match_type;

    InvitationInfo(int invitationId, std::string senderUsername, MatchType matchType)
        : id(invitationId), sender(std::move(senderUsername)), match_type(matchType) {}
};

struct PlayerProfile {
    std::string username;
    int xp;
    int rp;
    RankLevel rank_level;
    int total_wins;
    int total_losses;

    PlayerProfile(std::string username, int xp, int rp, RankLevel rankLevel, int totalWins,
                  int totalLosses)
        : username(std::move(username)), xp(xp), rp(rp), rank_level(rankLevel),
          total_wins(totalWins), total_losses(totalLosses) {}
};

struct ReportInfo {
    int id;
    std::string reporter;
    std::string reported;
    std::string reason;

    ReportInfo(int reportId, std::string reporterPlayer, std::string reportedPlayer,
               std::string reportReason)
        : id(reportId), reporter(std::move(reporterPlayer)), reported(std::move(reportedPlayer)),
          reason(std::move(reportReason)) {}
};

struct Penalty {
    int amount;
    int number_of_matches;
    int current_remaining_matches;

    Penalty(int penaltyAmount, int numberOfMatches)
        : amount(penaltyAmount), number_of_matches(numberOfMatches),
          current_remaining_matches(numberOfMatches) {}
};

#endif // TYPES_HPP_