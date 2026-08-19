#ifndef RESPONSE_FORMATTER_HPP_
#define RESPONSE_FORMATTER_HPP_

#include <string>
#include <vector>

#include "Types.hpp"

class ResponseFormatter {
public:
    static auto formatCasualOpponents(const std::vector<CasualPlayerInfo>& opponents) -> std::string;

    static auto formatRankedOpponents(const std::vector<RankedPlayerInfo>& opponents) -> std::string;

    static auto formatMatchStatus(const MatchStatus& status) -> std::string;

    static auto formatPlayerProfile(const PlayerProfile& profile) -> std::string;

    static auto formatReceivedInvitations(const std::vector<InvitationInfo>& invitations) -> std::string;

    static auto formatReports(const std::vector<ReportInfo>& reports) -> std::string;
};

#endif