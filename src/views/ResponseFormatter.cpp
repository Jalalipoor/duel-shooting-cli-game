#include "views/ResponseFormatter.hpp"

#include <format>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;

auto ResponseFormatter::formatCasualOpponents(const vector<CasualPlayerInfo>& opponents) -> string {
    if (opponents.empty()) { return "Empty"; }

    string result = "";
    for (size_t index = 0; index < opponents.size(); index++) {
        result += format("{}. {} with {} XP\n", (index + 1), opponents[index].username,
                         opponents[index].xp);
    }

    if (!result.empty() && result.back() == '\n') { result.pop_back(); }

    return result;
}

auto ResponseFormatter::formatRankedOpponents(const std::vector<RankedPlayerInfo>& opponents)
    -> std::string {
    if (opponents.empty()) { return "Empty"; }

    string result{""};
    for (size_t index = 0; index < opponents.size(); index++) {
        result += format("{}. {} with {} RP\n", (index + 1), opponents[index].username,
                         opponents[index].rp);
    }

    if (!result.empty() && result.back() == '\n') { result.pop_back(); }

    return result;
}

auto ResponseFormatter::formatMatchStatus(const MatchStatus& status) -> string {
    stringstream ss;

    ss << "Turn " << status.turn_number << "\n";
    ss << "You: " << status.your_latest_turn_action << "\n";
    ss << "Your opponent: " << status.opp_latest_turn_action << "\n";

    ss << "History:\n";
    ss << left << setw(20) << "Opponent's moves:" << "Your moves:\n";
    if (status.turn_number != 1) {
        for (int index = 1; index < status.turn_number; index++) {
            ss << left << setw(20);
            const auto& turn_history = status.actions_history.at(index);

            const string& your_move = (status.is_player1_requesting) ? turn_history.player1_action
                                                                     : turn_history.player2_action;
            const string& opp_move = (status.is_player1_requesting) ? turn_history.player2_action
                                                                    : turn_history.player1_action;
            ss << opp_move << your_move << "\n";
        }
    }
    ss << "Your remaining bullets: " << status.your_remaining_bullets << "\n";
    if (status.type == MatchType::Ranked) {
        ss << "Your remaining health: " << status.your_remaining_health;
    }
    return ss.str();
}

auto ResponseFormatter::formatPlayerProfile(const PlayerProfile& profile) -> string {
    stringstream ss;
    ss << format("username: \"{}\"\n", profile.username);
    ss << "Level: ";
    switch (profile.rank_level) {
        case RankLevel::Bronze:
            ss << "Bronze\n";
            break;
        case RankLevel::Silver:
            ss << "Silver\n";
            break;
        case RankLevel::Golden:
            ss << "Golden\n";
            break;
        case RankLevel::Platinum:
            ss << "Platinum\n";
            break;
    }
    ss << "RP: " << profile.rp << "\n";
    ss << "XP: " << profile.xp << "\n";
    ss << "Total wins: " << profile.total_wins << "\n";
    ss << "Total losses: " << profile.total_losses;
    return ss.str();
}

auto ResponseFormatter::formatReceivedInvitations(const vector<InvitationInfo>& invitations)
    -> string {
    if (invitations.empty()) return "Empty";
    string result = "";
    for (const auto& inv : invitations) {
        const string& match_type = (inv.match_type == MatchType::Casual) ? "casual" : "ranked";
        result += format("{}: Invitation from \"{}\" for a \"{}\" match\n", inv.id, inv.sender,
                         match_type);
    }
    result.pop_back();
    return result;
}

auto ResponseFormatter::formatReports(const vector<ReportInfo>& reports) -> string {
    if (reports.empty()) return "Empty";

    string result{""};
    for (const auto& report : reports) {
        result += format("{}: \"{}\" reported \"{}\" for: \"{}\"\n", report.id, report.reporter,
                         report.reported, report.reason);
    }
    result.pop_back();
    return result;
}