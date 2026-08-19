#ifndef CONSTANTS_HPP_
#define CONSTANTS_HPP_

#include <map>
#include <string_view>

#include "Types.hpp"

namespace constants::methods {
inline constexpr std::string_view METHOD_POST = "POST";
inline constexpr std::string_view METHOD_GET = "GET";
inline constexpr std::string_view METHOD_PUT = "put";
inline constexpr std::string_view METHOD_DELETE = "delete";

} // namespace constants::methods

namespace constants::cmds::post {
inline constexpr std::string_view REGISTER = "register";
inline constexpr std::string_view LOGIN = "login";
inline constexpr std::string_view LOGOUT = "logout";

inline constexpr std::string_view CASUAL_MATCH_READY = "casual_match_ready";
inline constexpr std::string_view SEND_INVITATION = "invitation";
inline constexpr std::string_view START_MATCH = "start_match";
inline constexpr std::string_view REJECT_INVITATION = "reject_invitation";
inline constexpr std::string_view GAME_ACTION = "action";

inline constexpr std::string_view REPORT_USER = "report";
inline constexpr std::string_view BLOCK_USER = "block";
inline constexpr std::string_view APPLY_PENALTY = "penalty";
inline constexpr std::string_view DISMISS_REPORT = "dismiss_report";
} // namespace constants::cmds::post

namespace constants::cmds::get {
inline constexpr std::string_view CASUAL_MATCH_OPPONENTS = "casual_match_opponents";
inline constexpr std::string_view RANKED_MATCH_OPPONENTS = "ranked_match_opponents";
inline constexpr std::string_view MATCH_STATUS = "match_status";

inline constexpr std::string_view PROFILE = "profile";
inline constexpr std::string_view RECEIVED_INVITATIONS = "received_invitations";
inline constexpr std::string_view REPORTS = "reports";
} // namespace constants::cmds::get

namespace constants::args {
inline constexpr std::string_view USERNAME = "username";
inline constexpr std::string_view PASSWORD = "password";

inline constexpr std::string_view READY_STATUS = "status";
inline constexpr std::string_view READY_STATUS_TRUE = "true";
inline constexpr std::string_view READY_STATUS_FALSE = "false";

inline constexpr std::string_view MATCH_TYPE = "match_type";
inline constexpr std::string_view MATCH_TYPE_CASUAL = "casual";
inline constexpr std::string_view MATCH_TYPE_RANKED = "ranked";

inline constexpr std::string_view INVITATION_ID = "invitation_id";
inline constexpr std::string_view ACTION_TYPE = "action";
inline constexpr std::string_view REPORT_REASON = "reason";

inline constexpr std::string_view BLOCK_STATUS = "status";
inline constexpr std::string_view BLOCK_STATUS_BLOCKED = "blocked";
inline constexpr std::string_view BLOCK_STATUS_UNBLOCKED = "unblocked";

inline constexpr std::string_view REPORT_ID = "report_id";
inline constexpr std::string_view PENALTY_TYPE = "type";
inline constexpr std::string_view PENALTY_AMOUNT = "amount";
inline constexpr std::string_view PENALTY_NUMBER_OF_MATCHES = "number_of_matches";
inline constexpr std::string_view PENALTY_TYPE_HEALTH = "health_penalty";
inline constexpr std::string_view PENALTY_TYPE_BULLET = "bullet_penalty";

inline constexpr std::string_view SORT_ORDER = "sort_order";
inline constexpr std::string_view ASCENDING = "asc";
inline constexpr std::string_view DESCENDING = "desc";

} // namespace constants::args

namespace constants {
inline constexpr int VALID_RUN_ARGUMENTS_COUNT = 3;

inline constexpr std::string USER_NOT_LOGGED_IN = "NOT_LOGGED_IN";
inline constexpr int DEFAULT_XP_VALUE_FOR_PLAYER = 500;
inline constexpr int DEFAULT_RP_VALUE_FOR_PLAYER = 1200;

inline constexpr std::string SHOOT_ACTION = "shoot";
inline constexpr std::string DEFEND_ACTION = "defend";
inline constexpr std::string RELOAD_ACTION = "reload";

inline constexpr double BASE_XP_GAIN = 50.0;
inline constexpr double XP_DIFFERENCE_COEFFICIENT = 0.1;
inline constexpr int MINIMUM_XP_GAIN = 5;

inline constexpr int HEALTH_BONUS_COEFFICIENT = 25;
inline const std::map<RankLevel, int> RANK_TO_DELTA_RP = {{RankLevel::Bronze, 75},
                                                          {RankLevel::Silver, 100},
                                                          {RankLevel::Golden, 125},
                                                          {RankLevel::Platinum, 150}};

} // namespace constants

#endif // CONSTANTS_HPP_