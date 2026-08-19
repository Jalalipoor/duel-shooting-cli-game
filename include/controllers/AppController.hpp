#ifndef APP_CONTROLLER_HPP_
#define APP_CONTROLLER_HPP_

#include <string>

#include "Types.hpp"
#include "models/GameSystem.hpp"

class AppController {
public:
    int run(int argc, const char* argv[]);

private:
    GameSystem game_system_;

    void manageCommand(const Command& command);

    void processPostCommand(const std::string& cmd, const std::map<std::string, std::string>& args);

    void processGetCommand(const std::string& cmd, const std::map<std::string, std::string>& args);

    static bool isValidArgumentsCount(int argc);
};

#endif // APP_CONTROLLER_HPP_