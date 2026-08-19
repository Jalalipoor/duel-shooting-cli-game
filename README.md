# Duel Shooting CLI Game

A turn-based, multiplayer dueling game engine controlled via a REST-like text protocol over the command line. Originally built as a course project in a university Advanced Programming course, later extended with a ranked-match system, rank levels, and penalty tracking.

## Requirements
- C++20 compiler with `<format>` support: **GCC 13+** or **Clang 17+**
  (older compilers such as GCC 11 will fail to build)

## Build & Run
```bash
make
./UTGame data/players.csv data/admins.csv
```

## Input Files
- `players.csv`: `username,password,xp,rp` (one header row + data rows)
- `admins.csv`: `username,password` (one header row + data rows)

## Features
- Player registration/login and separately-loaded admin accounts
- Casual matches (1-bullet shoot/defend/reload duels)
- Ranked matches (health-based, RP-driven matchmaking with rank levels: Bronze/Silver/Golden/Platinum)
- Invitation system (send/accept/reject), with auto-rejection of other pending invitations once a match starts
- Turn-based actions with simultaneous-turn resolution rules
- XP/RP calculated per match: `Δ = max(5, 50 - 0.1 × (winnerScore - loserScore))`
- Infraction reporting system with admin review and penalty enforcement (health or bullet penalties applied over a configurable number of matches)
- Leaderboard/scoreboard and player profile views
- Block/unblock other players

## Command Protocol
Every command is a REST-like text line sent over stdin:
\`\`\`
<VERB> <command> ? key "value" key2 "value2"
\`\`\`
Verbs: `GET`, `POST`, `PUT`, `DELETE`. Responses are one of:
`OK`, `Empty`, `Not Found`, `Bad Request`, `Permission Denied`, or a
command-specific formatted payload.

### Core commands
| Verb | Command | Description |
|------|---------|-------------|
| POST | `register` | Create a new player account and log in |
| POST | `login` / `logout` | Session management |
| POST | `casual_match_ready` | Toggle readiness for casual matchmaking |
| GET  | `casual_match_opponents` | List ready opponents, sorted by XP |
| POST | `invitation` | Send a match invitation |
| POST | `start_match` / `reject_invitation` | Accept/decline an invitation |
| POST | `action` | Submit a turn action (`shoot`/`defend`/`reload`) |
| GET  | `match_status` | View live match state and turn history |
| POST | `report` | Report another player to admins |
| GET  | `reports` (admin) | List pending infraction reports |
| POST | `review_report` (admin) | Review and resolve a report |
| GET  | `scoreboard` | View the player leaderboard |

## Architecture
- `models/` — domain logic: `Player`, `Admin`, abstract `Match` base class with `CasualMatch`/`RankedMatch` implementations, `Invitation`, `Report`, and the central `GameSystem` orchestrator (ownership via `unique_ptr`)
- `controllers/` — `AppController` (main loop) and `CommandHandler` (parses/validates the command protocol)
- `views/` — `ConsoleView` / `ResponseFormatter` for output rendering
- `utils/` — `CSVReader` for loading initial player/admin data
- Custom exception hierarchy (`GameException` and subclasses) mapped to the required error-priority order: verb validation → route existence → signature validation → authorization → business logic
