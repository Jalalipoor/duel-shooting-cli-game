#include "views/ConsoleView.hpp"

#include <iostream>

#include "Types.hpp"
#include "views/ResponseFormatter.hpp"

using namespace std;

auto ConsoleView::readLine() -> optional<string> {
    string line;
    if (getline(cin, line)) { return line; }
    if (cin.eof()) { return nullopt; }
    return "";
}

void ConsoleView::printOK() { cout << "OK" << endl; }

void ConsoleView::printCasualMatchOpponents(const vector<CasualPlayerInfo>& opponents) {
    string output_str = ResponseFormatter::formatCasualOpponents(opponents);
    cout << output_str << endl;
}

void ConsoleView::printCasualMatchStatus(const MatchStatus& status) {
    string output_str = ResponseFormatter::formatMatchStatus(status);
    cout << output_str << endl;
}

void ConsoleView ::printPlayerProfile(const PlayerProfile& profile) {
    string output_str = ResponseFormatter::formatPlayerProfile(profile);
    cout << output_str << endl;
}

void ConsoleView::printReceivedInvitations(const vector<InvitationInfo>& invitations) {
    string output_str = ResponseFormatter::formatReceivedInvitations(invitations);
    cout << output_str << endl;
}

void ConsoleView::printRankedMatchOpponents(const std::vector<RankedPlayerInfo> opponents) {
    string output_str = ResponseFormatter::formatRankedOpponents(opponents);
    cout << output_str << endl;
}

void ConsoleView::printReports(const vector<ReportInfo>& reports) {
    string output_str = ResponseFormatter::formatReports(reports);
    cout << output_str << endl;
}

void ConsoleView::printNotFound() { cout << "Not Found" << endl; }

void ConsoleView::printBadRequest() { cout << "Bad Request" << endl; }

void ConsoleView::printPermissionDenied() { cout << "Permission Denied" << endl; }

void ConsoleView::printLevelMismatch() { cout << "Level Mismatch" << endl; }