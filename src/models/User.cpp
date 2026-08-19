#include "models/User.hpp"

#include <utility>

using namespace std;

User::User(string username, string password)
    : username_(std::move(username)), password_(std::move(password)) {}

auto User::checkPassword(const string& password) const -> bool { return password == password_; }