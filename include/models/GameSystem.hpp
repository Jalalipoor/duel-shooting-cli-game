#ifndef GAME_SYSTEM_HPP_
#define GAME_SYSTEM_HPP_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Admin.hpp"
#include "Constants.hpp"
#include "Invitation.hpp"
#include "Match.hpp"
#include "Player.hpp"
#include "Report.hpp"
#include "Types.hpp"

class GameSystem {
public:
    void initializePlayers(const std::vector<PlayerData>& data);

    void initializeAdmins(const std::vector<AdminData>& data);

    void registerPlayer(const std::string& username, const std::string& password);

    void login(const std::string& username, const std::string& password);

    void logout();

    void setCasualMatchReady(bool isReady);

    void changeBlockStatus(const std::string& username, BlockStatus status);

    void sendInvitation(const std::string& receiverUsername, MatchType targetedMatchType);

    void startMatch(int invitationId);

    void rejectInvitation(int invitationId);

    void submitAction(const std::string& actionType);

    void submitReport(const std::string& username, const std::string& reason);

    void dismissReport(int reportId);

    void applyPenalty(int reportId, PenaltyType type, int amount, int numberOfMatches);
    
    auto getCasualMatchOpponents(const SortOrder& order) const -> std::vector<CasualPlayerInfo>;
    
    auto getRankedMatchOpponents(const SortOrder& order) const -> std::vector<RankedPlayerInfo>;
    
    auto getReceivedInvitations() const -> std::vector<InvitationInfo>;
    
    auto getMatchStatus() const -> MatchStatus;
    
    auto getProfile(const std::string& username) const -> PlayerProfile;

    auto getReports() const -> std::vector<ReportInfo>;

private:
    std::map<std::string, std::unique_ptr<Player>> players_;
    std::map<std::string, std::unique_ptr<Admin>> admins_;
    std::map<int, std::unique_ptr<Invitation>> invitations_;
    std::map<int, std::unique_ptr<Match>> matches_;
    std::map<int, std::unique_ptr<Report>> reports_;

    std::map<std::string, Player*> casual_ready_players_;

    std::string current_user_{constants::USER_NOT_LOGGED_IN};
    bool is_admin_logged_in_{false};

    int next_invitation_id_{1};
    int next_report_id_{1};

    int getNextInvitationId();
    int getNextReportId();

    void ensurePlayerLoggedIn() const;
    void ensureAdminLoggedIn() const;

    bool userExists(const std::string& username) const;
};

#endif // GAME_SYSTEM_HPP_