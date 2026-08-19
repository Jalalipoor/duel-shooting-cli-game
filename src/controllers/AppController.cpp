#include "controllers/AppController.hpp"

#include "Constants.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"
#include "controllers/CommandHandler.hpp"
#include "utils/CSVReader.hpp"
#include "views/ConsoleView.hpp"

using namespace std;

bool AppController::isValidArgumentsCount(int argc) {
    return argc == constants::VALID_RUN_ARGUMENTS_COUNT;
}

int AppController::run(int argc, const char* argv[]) {
    if (!isValidArgumentsCount(argc)) { return -1; }

    string playersCSVFilename = argv[1];
    string adminsCSVFilename = argv[2];

    const auto players = CSVReader::loadPlayersFromCSVFile(playersCSVFilename);
    const auto admins = CSVReader::loadAdminsFromCSVFile(adminsCSVFilename);

    game_system_.initializePlayers(players);
    game_system_.initializeAdmins(admins);

    while (true) {
        auto lineOpt = ConsoleView::readLine();

        if (!lineOpt.has_value()) { break; }

        if (!lineOpt->empty()) {
            try {
                Command command = CommandHandler::extractCommand(*lineOpt);
                manageCommand(command);
            } catch (const BadRequestException&) {
                ConsoleView::printBadRequest();
            } catch (const NotFoundException&) { ConsoleView::printNotFound(); }
        }
    }

    return 0;
}

void AppController::manageCommand(const Command& command) {
    string method = command.method;
    string cmd = command.cmd;
    map<string, string> args = command.args;

    if (method == constants::methods::METHOD_POST) { processPostCommand(cmd, args); }
    else if (method == constants::methods::METHOD_GET) { processGetCommand(cmd, args); }
}

void AppController::processPostCommand(const string& cmd, const map<string, string>& args) {
    try {
        if (cmd == constants::cmds::post::REGISTER) {
            game_system_.registerPlayer(args.at(string(constants::args::USERNAME)),
                                        args.at(string(constants::args::PASSWORD)));
        }
        else if (cmd == constants::cmds::post::LOGIN) {
            game_system_.login(args.at(string(constants::args::USERNAME)),
                               args.at(string(constants::args::PASSWORD)));
        }
        else if (cmd == constants::cmds::post::LOGOUT) { game_system_.logout(); }
        else if (cmd == constants::cmds::post::CASUAL_MATCH_READY) {
            game_system_.setCasualMatchReady((args.at(string(constants::args::READY_STATUS)) ==
                                                 constants::args::READY_STATUS_TRUE));
        }
        else if (cmd == constants::cmds::post::SEND_INVITATION) {
            MatchType targeted_match_type =
                (args.at(string(constants::args::MATCH_TYPE)) == constants::args::MATCH_TYPE_CASUAL)
                    ? MatchType::Casual
                    : MatchType::Ranked;
            game_system_.sendInvitation(args.at(string(constants::args::USERNAME)),
                                        targeted_match_type);
        }
        else if (cmd == constants::cmds::post::START_MATCH) {
            game_system_.startMatch(stoi(args.at(string(constants::args::INVITATION_ID))));
        }
        else if (cmd == constants::cmds::post::REJECT_INVITATION) {
            game_system_.rejectInvitation(stoi(args.at(string(constants::args::INVITATION_ID))));
        }
        else if (cmd == constants::cmds::post::GAME_ACTION) {
            game_system_.submitAction(args.at(string(constants::args::ACTION_TYPE)));
        }
        else if (cmd == constants::cmds::post::REPORT_USER) {
            game_system_.submitReport(args.at(string(constants::args::USERNAME)),
                                      args.at(string(constants::args::REPORT_REASON)));
        }
        else if (cmd == constants::cmds::post::BLOCK_USER) {
            BlockStatus status = (args.at(string(constants::args::BLOCK_STATUS)) ==
                                  constants::args::BLOCK_STATUS_BLOCKED)
                                     ? BlockStatus::Blocked
                                     : BlockStatus::Unblocked;
            game_system_.changeBlockStatus(args.at(string(constants::args::USERNAME)), status);
        }
        else if (cmd == constants::cmds::post::APPLY_PENALTY) {
            int report_id =
                CommandHandler::tryParseInt(args.at(string(constants::args::REPORT_ID))).value();
            PenaltyType type = (args.at(string(constants::args::PENALTY_TYPE)) ==
                                constants::args::PENALTY_TYPE_HEALTH)
                                   ? PenaltyType::Health
                                   : PenaltyType::Bullet;
            int amount =
                CommandHandler::tryParseInt(args.at(string(constants::args::PENALTY_AMOUNT)))
                    .value();
            int number_of_matches = CommandHandler::tryParseInt(
                                        args.at(string(constants::args::PENALTY_NUMBER_OF_MATCHES)))
                                        .value();
            game_system_.applyPenalty(report_id, type, amount, number_of_matches);
        }
        else if (cmd == constants::cmds::post::DISMISS_REPORT) {
            int report_id =
                CommandHandler::tryParseInt(args.at(string(constants::args::REPORT_ID))).value();
            game_system_.dismissReport(report_id);
        }
        ConsoleView::printOK();
    } catch (const BadRequestException&) {
        ConsoleView::printBadRequest();
    } catch (const PermissionDeniedException&) {
        ConsoleView::printPermissionDenied();
    } catch (const NotFoundException&) {
        ConsoleView::printNotFound();
    } catch (const LevelMismatchException&) { ConsoleView::printLevelMismatch(); }
}

void AppController::processGetCommand(const string& cmd, const map<string, string>& args) {
    try {
        if (cmd == constants::cmds::get::CASUAL_MATCH_OPPONENTS) {
            SortOrder order{SortOrder::Descending};
            if (args.contains(string(constants::args::SORT_ORDER))) {
                order =
                    (args.at(string(constants::args::SORT_ORDER)) == constants::args::DESCENDING)
                        ? SortOrder::Descending
                        : SortOrder::Ascending;
            }
            const auto opponents = game_system_.getCasualMatchOpponents(order);
            ConsoleView::printCasualMatchOpponents(opponents);
        }
        else if (cmd == constants::cmds::get::RANKED_MATCH_OPPONENTS) {
            SortOrder order{SortOrder::Descending};
            if (args.contains(string(constants::args::SORT_ORDER))) {
                order =
                    (args.at(string(constants::args::SORT_ORDER)) == constants::args::DESCENDING)
                        ? SortOrder::Descending
                        : SortOrder::Ascending;
            }
            const auto opponents = game_system_.getRankedMatchOpponents(order);
            ConsoleView::printRankedMatchOpponents(opponents);
        }
        else if (cmd == constants::cmds::get::MATCH_STATUS) {
            const auto status = game_system_.getMatchStatus();
            ConsoleView::printCasualMatchStatus(status);
        }
        else if (cmd == constants::cmds::get::PROFILE) {
            string username{""};
            if (args.contains(string(constants::args::USERNAME))) {
                username = args.at(string(constants::args::USERNAME));
            }
            const auto profile = game_system_.getProfile(username);
            ConsoleView::printPlayerProfile(profile);
        }
        else if (cmd == constants::cmds::get::RECEIVED_INVITATIONS) {
            const auto received_invitations = game_system_.getReceivedInvitations();
            ConsoleView::printReceivedInvitations(received_invitations);
        }
        else if (cmd == constants::cmds::get::REPORTS) {
            const auto reports = game_system_.getReports();
            ConsoleView::printReports(reports);
        }
    } catch (const BadRequestException&) {
        ConsoleView::printBadRequest();
    } catch (const PermissionDeniedException&) {
        ConsoleView::printPermissionDenied();
    } catch (const NotFoundException&) { ConsoleView::printNotFound(); }
}