#ifndef USER_HPP_
#define USER_HPP_

#include <string>

class User {
public:
    User(std::string username, std::string password);
    virtual ~User() = default;

    auto getUsername() const -> std::string { return username_; };

    auto checkPassword(const std::string& password) const -> bool;

private:
    std::string username_;
    std::string password_;
};

#endif