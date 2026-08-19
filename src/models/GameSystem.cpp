#include "models/GameSystem.hpp"

#include <algorithm>
#include <format>
#include <functional>
#include <memory>
#include <stdexcept>

#include "Constants.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"
#include "models/Match.hpp"
#include "models/Player.hpp"

using namespace std;

auto GameSystem::getNextInvitationId() -> int { return next_invitation_id_++; }

auto GameSystem::getNextReportId() -> int { return next_report_id_++; }

void GameSystem::ensurePlayerLoggedIn() const {
    if (current_user_ == constants::USER_NOT_LOGGED_IN || is_admin_logged_in_) {
        throw PermissionDeniedException("No user logged in or the logged in user is admin.");
    }
}

void GameSystem::ensureAdminLoggedIn() const {
    if (current_user_ == constants::USER_NOT_LOGGED_IN || (!is_admin_logged_in_)) {
        throw PermissionDeniedException("No user logged in or the logged in user is player.");
    }
}

void GameSystem::initializePlayers(const vector<PlayerData>& data) {
    for (const auto& player : data) {
        if (player.username.empty() || players_.contains(player.username)) { continue; }
        players_.emplace(player.username, make_unique<Player>(player.username, player.password,
                                                              player.xp, player.rp));
    }
}

void GameSystem::initializeAdmins(const vector<AdminData>& data) {
    for (const auto& admin : data) {
        if (admin.username.empty() || admins_.contains(admin.username)) { continue; }
        admins_.emplace(admin.username, make_unique<Admin>(admin.username, admin.password));
    }
}

auto GameSystem::userExists(const std::string& username) const -> bool {
    return (players_.contains(username) || admins_.contains(username));
}

void GameSystem::registerPlayer(const string& username, const string& password) {
    if (current_user_ != constants::USER_NOT_LOGGED_IN) {
        throw PermissionDeniedException(
            format("User with username \"{}\" is already logged in.", current_user_));
    }

    if (username.empty()) { throw BadRequestException("Entered username is empty."); }

    if (userExists(username)) {
        throw BadRequestException(format("Username \"{}\" is already taken.", username));
    }

    players_.emplace(username,
                     make_unique<Player>(username, password, constants::DEFAULT_XP_VALUE_FOR_PLAYER,
                                         constants::DEFAULT_RP_VALUE_FOR_PLAYER));
    login(username, password);
}

void GameSystem::login(const string& username, const string& password) {
    if (current_user_ != constants::USER_NOT_LOGGED_IN) {
        throw PermissionDeniedException("Another user is already logged in.");
    }

    if (admins_.find(username) != admins_.end()) {
        if (!admins_.at(username)->checkPassword(password)) {
            throw PermissionDeniedException("Incorrect password.");
        }
        current_user_ = username;
        is_admin_logged_in_ = true;
        return;
    }
    else if (players_.find(username) != players_.end()) {
        if (!players_.at(username)->checkPassword(password)) {
            throw PermissionDeniedException("Incorrect password.");
        }
        current_user_ = username;
        is_admin_logged_in_ = false;
        return;
    }

    throw NotFoundException(format("User \"{}\" not found.", username));
}

void GameSystem::logout() {
    if (current_user_ == constants::USER_NOT_LOGGED_IN) {
        throw PermissionDeniedException("No user is currently logged in.");
    }

    current_user_ = constants::USER_NOT_LOGGED_IN;
    is_admin_logged_in_ = false;
}

void GameSystem::setCasualMatchReady(bool ready) {
    ensurePlayerLoggedIn();

    if (!players_.contains(current_user_)) {
        throw runtime_error("The current user in game system doesn't exists in players.");
    }

    players_.at(current_user_)->markReadyForCasual(ready);
    if (ready) { casual_ready_players_.emplace(current_user_, players_.at(current_user_).get()); }
    else { casual_ready_players_.erase(current_user_); }
}

void GameSystem::sendInvitation(const string& receiverUsername, MatchType targetedMatchType) {
    ensurePlayerLoggedIn();

    if (admins_.contains(receiverUsername)) {
        throw PermissionDeniedException("Receiver username for invitation belongs to a admin.");
    }

    if (!players_.contains(receiverUsername)) {
        throw NotFoundException("Couldn't find the receiver username for invitation in players.");
    }

    if (current_user_ == receiverUsername) {
        throw PermissionDeniedException("You cannot invite yourself for a match.");
    }

    if (players_.at(receiverUsername)->isBlocked(current_user_)) {
        throw NotFoundException("The receiver of invitation has blocked the sender.");
    }

    int invitationId = getNextInvitationId();
    invitations_.emplace(invitationId, make_unique<Invitation>(
                                           invitationId, players_.at(current_user_).get(),
                                           players_.at(receiverUsername).get(), targetedMatchType));
}

void GameSystem::rejectInvitation(int invitationId) {
    if (current_user_ == constants::USER_NOT_LOGGED_IN || is_admin_logged_in_) {
        throw PermissionDeniedException("No user logged in or logged in user is admin.");
    }

    if (!invitations_.contains(invitationId)) {
        throw NotFoundException(
            format("Couldn't find an invitation with invitation id '{}'.", invitationId));
    }

    invitations_.at(invitationId)->declineBy(current_user_);
}

void GameSystem::startMatch(int invitationId) {
    ensurePlayerLoggedIn();

    if (!invitations_.contains(invitationId)) {
        throw NotFoundException(format("Couldn't find an invitation with id '{}'.", invitationId));
    }

    try {
        auto created_match = invitations_.at(invitationId)->acceptBy(current_user_);
        matches_.emplace(invitationId, std::move(created_match));
    } catch (const LevelMismatchException& ex) {
        size_t num_removed = invitations_.erase(invitationId);
        if (num_removed != 1) {
            throw runtime_error(format("In sendInvitation in GameSystem: We want to start a "
                                       "ranked match but there aren't in same level. In "
                                       "removing the invitation, couldn't remove the "
                                       "invitation with id '{}'. (num_removed = {})",
                                       invitationId, num_removed));
        }
        throw ex;
    }
}

void GameSystem::submitAction(const string& actionType) {
    ensurePlayerLoggedIn();

    auto current_player = players_.at(current_user_).get();
    if (!current_player->isInMatch()) {
        throw NotFoundException("Current user is not playing in any matches.");
    }

    current_player->getPlayingMatch()->handleAction(current_user_, actionType);
}

void GameSystem::submitReport(const string& username, const string& reason) {
    ensurePlayerLoggedIn();

    if (!players_.contains(username)) {
        throw NotFoundException("The username entered to report it, doesn't exists.");
    }

    int report_id = getNextReportId();
    reports_[report_id] = make_unique<Report>(report_id, current_user_, username, reason);
}

void GameSystem::changeBlockStatus(const string& username, BlockStatus status) {
    ensurePlayerLoggedIn();

    if (admins_.contains(username)) {
        throw BadRequestException("You cannot block or unblock an admin user.");
    }

    if (!players_.contains(username)) {
        throw NotFoundException(format("Couldn't find the player with username "
                                       "'{}' to block or ublock it.",
                                       username));
    }

    auto current_player = players_.at(current_user_).get();
    (status == BlockStatus::Blocked) ? current_player->blockPlayer(username)
                                     : current_player->unblockPlayer(username);
}

void GameSystem::applyPenalty(int reportId, PenaltyType type, int amount, int numberOfMatches) {
    ensureAdminLoggedIn();

    if (!reports_.contains(reportId)) { throw NotFoundException("Report doesn't exists."); }

    Player* target_user = players_.at(reports_.at(reportId)->getReported()).get();

    admins_.at(current_user_)
        ->applyPenalty(reports_.at(reportId).get(), target_user, type, amount, numberOfMatches);
}

void GameSystem::dismissReport(int reportId) {
    ensureAdminLoggedIn();

    if (!reports_.contains(reportId)) { throw NotFoundException("Report doesn't exists."); }

    admins_.at(current_user_)->dismissReport(reports_.at(reportId).get());
}

auto GameSystem::getCasualMatchOpponents(const SortOrder& order) const -> vector<CasualPlayerInfo> {
    ensurePlayerLoggedIn();

    vector<CasualPlayerInfo> result;
    for (const auto& opp : casual_ready_players_) {
        if (opp.first == current_user_) continue;
        result.emplace_back(opp.first, opp.second->getXp());
    }

    ranges::sort(result, [order](const CasualPlayerInfo& o1, const CasualPlayerInfo& o2) {
        if (o1.xp == o2.xp) { return o1.username < o2.username; }
        return (order == SortOrder::Ascending) ? o1.xp < o2.xp : o1.xp > o2.xp;
    });

    return result;
}

auto GameSystem::getMatchStatus() const -> MatchStatus {
    ensurePlayerLoggedIn();
    if (!players_.at(current_user_)->isInMatch()) {
        throw NotFoundException("Logged in player is not playing any matches.");
    }
    Match* playing_match = players_.at(current_user_)->getPlayingMatch();
    return playing_match->getMatchStatus(current_user_);
}

auto GameSystem::getProfile(const std::string& username) const -> PlayerProfile {
    if (current_user_ == constants::USER_NOT_LOGGED_IN) {
        throw PermissionDeniedException("No user is logged in.");
    }

    if (username.empty()) {
        if (is_admin_logged_in_) {
            throw PermissionDeniedException("You cannot get an admin's profile.");
        }
        return players_.at(current_user_)->getProfile();
    }

    if (admins_.contains(username)) {
        throw PermissionDeniedException("You cannot get an admin's profile.");
    }

    if (!players_.contains(username)) {
        throw NotFoundException(format("Couldn't find a player with username '{}'.", username));
    }

    return players_.at(username)->getProfile();
}

auto GameSystem::getReceivedInvitations() const -> vector<InvitationInfo> {
    ensurePlayerLoggedIn();

    vector<InvitationInfo> result;
    for (const auto& invitation : invitations_) {
        if (invitation.second->getReceiver() == current_user_ &&
            invitation.second->getStatus() == InvitationStatus::Pending) {
            int id = invitation.first;
            string sender = invitation.second->getSenderUsername();
            MatchType match_type = invitation.second->getTargetedMatchType();
            result.emplace_back(id, sender, match_type);
        }
    }

    ranges::sort(result, std::less{}, &InvitationInfo::id);
    return result;
}

auto GameSystem::getReports() const -> vector<ReportInfo> {
    ensureAdminLoggedIn();

    vector<ReportInfo> result;
    for (const auto& report : reports_) {
        if (report.second->isClosed()) continue;
        result.push_back(report.second->getReportInfo());
    }

    ranges::sort(result, std::less{}, &ReportInfo::id);

    return result;
}

auto GameSystem::getRankedMatchOpponents(const SortOrder& order) const -> vector<RankedPlayerInfo> {
    ensurePlayerLoggedIn();

    vector<RankedPlayerInfo> result;
    for (const auto& [username, player] : players_) {
        if (username == current_user_) continue;
        if (!player->hasSameRank(*players_.at(current_user_))) continue;
        result.emplace_back(username, player->getRp());
    }

    ranges::sort(result, [order](const RankedPlayerInfo& p1, const RankedPlayerInfo& p2) {
        if (p1.rp == p2.rp) { return p1.username < p2.username; }
        return (order == SortOrder::Descending) ? p1.rp > p2.rp : p1.rp < p2.rp;
    });

    return result;
}