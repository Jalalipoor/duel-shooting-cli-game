#ifndef CONSOLE_VIEW_HPP_
#define CONSOLE_VIEW_HPP_

#include "Types.hpp"
#include <optional>
#include <string>
#include <vector>

class ConsoleView {
public:
    static auto readLine() -> std::optional<std::string>;

    static void printOK();

    static void printCasualMatchOpponents(const std::vector<CasualPlayerInfo>& opponents);

    static void printRankedMatchOpponents(const std::vector<RankedPlayerInfo> opponents);

    static void printCasualMatchStatus(const MatchStatus& status);

    static void printPlayerProfile(const PlayerProfile& profile);

    static void printReceivedInvitations(const std::vector<InvitationInfo>& invitations);

    static void printReports(const std::vector<ReportInfo>& reports);

    static void printNotFound();

    static void printBadRequest();

    static void printPermissionDenied();

    static void printLevelMismatch();
};

#endif // CONSOLE_VIEW_HPP_