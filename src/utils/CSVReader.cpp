#include "utils/CSVReader.hpp"

#include <fstream>
#include <sstream>

using namespace std;

vector<string> CSVReader::parseCSVLine(const string& line) {
    vector<string> result;
    stringstream ss(line);
    string cell;
    while (getline(ss, cell, ',')) {
        result.push_back(cell);
    }
    return result;
}

vector<PlayerData> CSVReader::loadPlayersFromCSVFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return {};

    string line;
    getline(file, line);

    vector<PlayerData> result;
    while (getline(file, line)) {
        vector<string> cells = parseCSVLine(line);
        if (cells.size() != 4) continue;
        string username = cells[0];
        string password = cells[1];
        int xp = stoi(cells[2]);
        int rp = stoi(cells[3]);
        result.emplace_back(username, password, xp, rp);
    }

    return result;
}

vector<AdminData> CSVReader::loadAdminsFromCSVFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return {};
    }

    string line;
    getline(file, line);

    vector<AdminData> result;
    while (getline(file, line)) {
        vector<string> cells = parseCSVLine(line);
        if (cells.size() != 2) continue;
        string username = cells[0];
        string password = cells[1];
        result.emplace_back(username, password);
    }

    return result;
}