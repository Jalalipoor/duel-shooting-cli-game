#ifndef COMMAND_HANDLER_HPP_
#define COMMAND_HANDLER_HPP_

#include <initializer_list>
#include <map>
#include <optional>
#include <string>

#include "Types.hpp"

enum class CommandCheckResult { OK, BadRequest, NotFound };

class CommandHandler {
public:
    static auto extractCommand(const std::string& line) -> Command;

    static auto isValidCommand(const Command& command) -> CommandCheckResult;

    static auto tryParseInt(const std::string& str) -> std::optional<int>;

private:
    static bool hasArgs(const std::map<std::string, std::string>& args,
                        std::initializer_list<std::string_view> required);

    static void skipWhitespace(const std::string& str, size_t& pos);

    static auto parseToMap(const std::string& line) -> std::map<std::string, std::string>;
};

#endif // COMMAND_HANDLER_HPP_