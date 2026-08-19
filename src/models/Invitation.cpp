#include "models/Invitation.hpp"

#include <format>
#include <memory>

#include "Exceptions.hpp"
#include "Types.hpp"
#include "models/CasualMatch.hpp"
#include "models/Player.hpp"
#include "models/RankedMatch.hpp"

using namespace std;

Invitation::Invitation(int id, Player* sender, Player* receiver, MatchType targetedMatchType)
    : id_(id), sender_(sender), receiver_(receiver), targeted_match_type_(targetedMatchType),
      status_(InvitationStatus::Pending) {}

void Invitation::declineBy(const string& username) {
    if (status_ != InvitationStatus::Pending) {
        throw NotFoundException(format("Invitation with invitation id '{}' had "
                                       "been already accepted or declined.",
                                       id_));
    }

    if (receiver_->getUsername() != username) {
        throw PermissionDeniedException(
            "You cannot reject an invitation which you aren't its receiver.");
    }

    status_ = InvitationStatus::Declined;
}

auto Invitation::acceptBy(const string& username) -> unique_ptr<Match> {
    if (status_ != InvitationStatus::Pending) {
        throw NotFoundException(
            format("Invitation with id '{}' had been already accepted or declined.", id_));
    }

    if (receiver_->getUsername() != username) {
        throw PermissionDeniedException(
            "You cannot accept an invitation which you aren't its receiver.");
    }

    if (sender_->isInMatch() || receiver_->isInMatch()) {
        throw PermissionDeniedException(
            format("One or both players are already in a match. [Invitation ID "
                   "= '{}']",
                   id_));
    }

    if (targeted_match_type_ == MatchType::Casual) {
        auto playing_match = make_unique<CasualMatch>(id_, sender_, receiver_);
        sender_->enterMatch(playing_match.get());
        receiver_->enterMatch(playing_match.get());
        status_ = InvitationStatus::Accepted;
        return playing_match;
    }
    else {
        if (!sender_->hasSameRank(*receiver_)) { throw LevelMismatchException(); }

        auto playing_match = make_unique<RankedMatch>(
            id_, sender_, receiver_, sender_->consumeHealthPenalty(), receiver_->consumeHealthPenalty(),
            sender_->consumeBulletPenalty(), receiver_->consumeBulletPenalty());
        sender_->enterMatch(playing_match.get());
        receiver_->enterMatch(playing_match.get());
        status_ = InvitationStatus::Accepted;
        return playing_match;
    }
}