#include "models/Admin.hpp"

#include "Exceptions.hpp"
#include "models/Player.hpp"
#include "models/Report.hpp"

using namespace std;

Admin::Admin(const string& username, const string& password) : User(username, password) {}

void Admin::applyPenalty(Report* report, Player* player, PenaltyType type, int amount,
                         int numOfMatches) {
    if (type == PenaltyType::Health && (amount < 1 || amount > 2)) {
        throw BadRequestException("The amount value is invalid.");
    }
    else if (type == PenaltyType::Bullet && (amount < 1 || amount > 3)) {
        throw BadRequestException("The amount value is invalid.");
    }

    if (numOfMatches < 1) { throw BadRequestException("The number of matches value is invalid."); }

    if (report->isClosed()) { throw NotFoundException("The report is already closed."); }

    player->setPenalty(Penalty(amount, numOfMatches), type);
    report->close();
}

void Admin::dismissReport(Report* report) {
    if (report->isClosed()) { throw NotFoundException("The report is already closed."); }
    report->close();
}
