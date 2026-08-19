#include "controllers/CommandHandler.hpp"

#include <format>
#include <initializer_list>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>

#include "Constants.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"

using namespace std;

auto CommandHandler::extractCommand(const string& line) -> Command {
    stringstream ss(line);
    string method, cmd, questionMark;
    if (!(ss >> method >> cmd >> questionMark) || (questionMark != "?")) {
        throw BadRequestException("Miss pattern command.");
    }

    string arguments_str;
    getline(ss, arguments_str);

    map<string, string> args = parseToMap(arguments_str);

    Command command = {method, cmd, args};
    switch (isValidCommand(command)) {
        case CommandCheckResult::BadRequest:
            throw BadRequestException();
        case CommandCheckResult::NotFound:
            throw NotFoundException();
        case CommandCheckResult::OK:
            return command;
    }

    return {};
}

auto CommandHandler::isValidCommand(const Command& command) -> CommandCheckResult {
    const string& method = command.method;
    const string& cmd = command.cmd;
    const map<string, string>& args = command.args;

    if (method == constants::methods::METHOD_POST) {
        if (cmd == constants::cmds::post::REGISTER || cmd == constants::cmds::post::LOGIN) {
            return (hasArgs(args, {constants::args::USERNAME, constants::args::PASSWORD}))
                       ? CommandCheckResult::OK
                       : CommandCheckResult::BadRequest;
        }
        else if (cmd == constants::cmds::post::LOGOUT) { return CommandCheckResult::OK; }
        else if (cmd == constants::cmds::post::CASUAL_MATCH_READY) {
            if (!hasArgs(args, {constants::args::READY_STATUS})) {
                return CommandCheckResult::BadRequest;
            }

            if (!(args.at(string(constants::args::READY_STATUS)) ==
                      constants::args::READY_STATUS_TRUE ||
                  args.at(string(constants::args::READY_STATUS)) ==
                      constants::args::READY_STATUS_FALSE)) {
                return CommandCheckResult::BadRequest;
            }

            return CommandCheckResult::OK;
        }
        else if (cmd == constants::cmds::post::SEND_INVITATION) {
            if (!hasArgs(args, {constants::args::USERNAME, constants::args::MATCH_TYPE})) {
                return CommandCheckResult::BadRequest;
            }

            if (!(args.at(string(constants::args::MATCH_TYPE)) ==
                      constants::args::MATCH_TYPE_CASUAL ||
                  args.at(string(constants::args::MATCH_TYPE)) ==
                      constants::args::MATCH_TYPE_RANKED)) {
                return CommandCheckResult::BadRequest;
            }

            return CommandCheckResult::OK;
        }
        else if (cmd == constants::cmds::post::START_MATCH ||
                 cmd == constants::cmds::post::REJECT_INVITATION) {
            if (!hasArgs(args, {constants::args::INVITATION_ID}))
                return CommandCheckResult::BadRequest;
            if (!tryParseInt(args.at(string(constants::args::INVITATION_ID))).has_value()) {
                return CommandCheckResult::BadRequest;
            }
            return CommandCheckResult::OK;
        }
        else if (cmd == constants::cmds::post::GAME_ACTION) {
            return hasArgs(args, {constants::args::ACTION_TYPE}) ? CommandCheckResult::OK
                                                                 : CommandCheckResult::BadRequest;
        }
        else if (cmd == constants::cmds::post::REPORT_USER) {
            if (!hasArgs(args, {constants::args::USERNAME, constants::args::REPORT_REASON})) {
                return CommandCheckResult::BadRequest;
            }

            if (args.at(string(constants::args::REPORT_REASON)).empty()) {
                return CommandCheckResult::BadRequest;
            }

            return CommandCheckResult::OK;
        }
        else if (cmd == constants::cmds::post::BLOCK_USER) {
            return hasArgs(args, {constants::args::USERNAME, constants::args::BLOCK_STATUS}) &&
                           (args.at(string(constants::args::BLOCK_STATUS)) ==
                                constants::args::BLOCK_STATUS_BLOCKED ||
                            args.at(string(constants::args::BLOCK_STATUS)) ==
                                constants::args::BLOCK_STATUS_UNBLOCKED)
                       ? CommandCheckResult::OK
                       : CommandCheckResult::BadRequest;
        }
        else if (cmd == constants::cmds::post::APPLY_PENALTY) {

            if (!hasArgs(args, {constants::args::REPORT_ID, constants::args::PENALTY_TYPE,
                                constants::args::PENALTY_AMOUNT,
                                constants::args::PENALTY_NUMBER_OF_MATCHES})) {
                return CommandCheckResult::BadRequest;
            }

            string type = args.at(string(constants::args::PENALTY_TYPE));
            if (!(type == constants::args::PENALTY_TYPE_HEALTH ||
                  type == constants::args::PENALTY_TYPE_BULLET)) {
                return CommandCheckResult::BadRequest;
            }

            if (!tryParseInt(args.at(string(constants::args::REPORT_ID))).has_value() ||
                !tryParseInt(args.at(string(constants::args::PENALTY_AMOUNT))).has_value() ||
                !tryParseInt(args.at(string(constants::args::PENALTY_NUMBER_OF_MATCHES)))
                     .has_value()) {
                return CommandCheckResult::BadRequest;
            }

            return CommandCheckResult::OK;
        }
        else if (cmd == constants::cmds::post::DISMISS_REPORT) {
            return hasArgs(args, {constants::args::REPORT_ID}) &&
                           tryParseInt(args.at(string(constants::args::REPORT_ID))).has_value()
                       ? CommandCheckResult::OK
                       : CommandCheckResult::BadRequest;
        }
        return CommandCheckResult::NotFound;
    }
    else if (method == constants::methods::METHOD_GET) {
        if (cmd == constants::cmds::get::CASUAL_MATCH_OPPONENTS) {
            if (hasArgs(args, {constants::args::SORT_ORDER}) &&
                (args.at(string(constants::args::SORT_ORDER)) != constants::args::ASCENDING &&
                 args.at(string(constants::args::SORT_ORDER)) != constants::args::DESCENDING)) {
                return CommandCheckResult::BadRequest;
            }
            return CommandCheckResult::OK;
        }
        else if (cmd == constants::cmds::get::RANKED_MATCH_OPPONENTS) {
            if (hasArgs(args, {constants::args::SORT_ORDER}) &&
                (args.at(string(constants::args::SORT_ORDER)) != constants::args::ASCENDING &&
                 args.at(string(constants::args::SORT_ORDER)) != constants::args::DESCENDING)) {
                return CommandCheckResult::BadRequest;
            }
            return CommandCheckResult::OK;
        }
        else if (cmd == constants::cmds::get::MATCH_STATUS) { return CommandCheckResult::OK; }
        else if (cmd == constants::cmds::get::PROFILE) { return CommandCheckResult::OK; }
        else if (cmd == constants::cmds::get::RECEIVED_INVITATIONS) {
            return CommandCheckResult::OK;
        }
        else if (cmd == constants::cmds::get::REPORTS) { return CommandCheckResult::OK; }
    }

    return CommandCheckResult::BadRequest;
}

void CommandHandler::skipWhitespace(const string& str, size_t& pos) {
    while (pos < str.length() && isspace(str[pos])) { pos++; }
}

auto CommandHandler::parseToMap(const string& line) -> map<string, string> {
    map<string, string> arguments;
    size_t currentPos = 0;
    const size_t lineLength = line.length();

    while (currentPos < lineLength) {
        skipWhitespace(line, currentPos);

        if (currentPos == lineLength) { break; }

        if (line[currentPos] == '\"') {
            throw BadRequestException("Keys must not start with double quotes.");
        }

        string key;
        while (currentPos < lineLength && !isspace(line[currentPos]) && line[currentPos] != '\"') {
            key += line[currentPos];
            currentPos++;
        }

        if (key.empty()) { throw BadRequestException("Extracted key cannot be empty."); }

        skipWhitespace(line, currentPos);

        if (currentPos >= lineLength || line[currentPos] != '\"') {
            throw BadRequestException(
                format("Missing opening quote for the value of key '{}'.", key));
        }

        currentPos++;

        string value;
        bool foundClosingQuotes = false;

        while (currentPos < lineLength) {
            if (line[currentPos] == '\"') {
                foundClosingQuotes = true;
                currentPos++;
                break;
            }
            value += line[currentPos];
            currentPos++;
        }

        if (!foundClosingQuotes) {
            throw BadRequestException(
                format("Mismatched quotes. Missing closing quote for key '{}'.", key));
        }

        if (arguments.contains(key)) {
            throw BadRequestException(format("Duplicate key '{}' detected in command.", key));
        }

        arguments[key] = value;
    }

    return arguments;
}

auto CommandHandler::tryParseInt(const string& str) -> optional<int> {
    int value = 0;

    auto [ptr, ec] = from_chars(str.data(), str.data() + str.size(), value);

    if (ec == errc{} && ptr == str.data() + str.size()) { return value; }

    return nullopt;
}

bool CommandHandler::hasArgs(const map<string, string>& args,
                             initializer_list<string_view> required) {
    for (auto arg : required) {
        if (!args.contains(string(arg))) return false;
    }
    return true;
}