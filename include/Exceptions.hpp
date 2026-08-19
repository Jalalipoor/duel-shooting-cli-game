#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <stdexcept>
#include <string>

class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string &message,
                           const std::string &reason = "")
        : std::runtime_error(message), reason_(reason) {}

    std::string getReason() const { return reason_; }

private:
    std::string reason_;
};

class PermissionDeniedException : public GameException {
public:
    explicit PermissionDeniedException(
        const std::string &reason = "",
        const std::string &message = "Permission Denied")
        : GameException(message, reason) {}
};

class NotFoundException : public GameException {
public:
    explicit NotFoundException(const std::string &reason = "",
                               const std::string &message = "Not Found")
        : GameException(message, reason) {}
};

class BadRequestException : public GameException {
public:
    explicit BadRequestException(const std::string &reason = "",
                                 const std::string &message = "Bad Request")
        : GameException(message, reason) {}
};

class LevelMismatchException : public GameException {
public:
    explicit LevelMismatchException(std::string reason = "",
                                    std::string message = "Level Mismatch")
        : GameException(std::move(message), std::move(reason)) {}
};

#endif // EXCEPTIONS_HPP_