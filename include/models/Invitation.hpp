#ifndef INVITATION_HPP_
#define INVITATION_HPP_

#include <memory>
#include <string>

#include "Player.hpp"
#include "Types.hpp"

class Match;

class Invitation {
public:
    Invitation(int id, Player* sender, Player* receiver, MatchType targetedMatchType);

    auto acceptBy(const std::string& username) -> std::unique_ptr<Match>;

    void declineBy(const std::string& username);

    auto getId() const -> int { return id_; }

    auto getSenderUsername() const -> std::string { return sender_->getUsername(); }

    auto getReceiver() const -> std::string { return receiver_->getUsername(); }

    auto getStatus() const -> InvitationStatus { return status_; }

    auto getTargetedMatchType() const -> MatchType { return targeted_match_type_; }

private:
    int id_;
    Player* sender_;
    Player* receiver_;
    MatchType targeted_match_type_;
    InvitationStatus status_;
};

#endif // INVITATION_HPP_