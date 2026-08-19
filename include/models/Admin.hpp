#ifndef ADMIN_HPP_
#define ADMIN_HPP_

#include <vector>

#include "Types.hpp"
#include "User.hpp"

class Report;
class Player;

class Admin : public User {
public:
    Admin(const std::string& username, const std::string& password);

    void applyPenalty(Report* report, Player* player, PenaltyType type, int amount,
                      int numOfMatches);
    void dismissReport(Report* report);

private:
    std::vector<int> processed_reports_;
};

#endif // ADMIN_HPP_