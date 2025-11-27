import <iostream>;
import <string>;
import InputHandler;
#include "test_runner.h"
using namespace std;

InputHandler ih;

std::vector<std::pair<std::string, std::string>> aliases = {
    {"left", "left"},
    {"right", "right"},
    {"down", "down"},
    {"drop", "drop"},
    {"clockwise", "clockwise"},
    {"cw", "clockwise"},
    {"clock", "clockwise"},
    {"counterclockwise", "counterclockwise"},
    {"ccw", "counterclockwise"},
    {"levelup", "levelup"},
    {"leveldown", "leveldown"},
    {"norandom", "norandom"},
    {"random", "random"},
    {"sequence", "sequence"},
    {"restart", "restart"},
    // Block-replacement single-letter commands
    {"I", "I"},
    {"J", "J"},
    {"L", "L"},
    {"O", "O"},
    {"S", "S"},
    {"Z", "Z"},
    {"T", "T"}};

TEST_CASE(InputHandler_register) {
    // Register canonical commands and common aliases from the spec

    // Register aliases
    // NOTE: already change to automatically register default aliases
    // but this still works because we allow rewrite command for same alias
    for (auto& p : aliases) {
        REQUIRE(ih.registerAlias(p.first, p.second));
    }
}

TEST_CASE(InputHandler_resolve) {  // resolve checks: exact matches
    for (auto& p : aliases) {
        REQUIRE_EQUAL(p.second, ih.resolve(p.first));
        // Unique-prefix resolution checks
        // "levelup" vs "leveldown" -> "lev" ambiguous, specific prefixes should
        // resolve
        REQUIRE_EQUAL(std::string("INVALID_CMD"), ih.resolve("lev"));
        REQUIRE_EQUAL(std::string("levelup"), ih.resolve("levelu"));
        REQUIRE_EQUAL(std::string("leveldown"), ih.resolve("leveld"));

        // Single-letter prefix 'r' is ambiguous because of "right" and "random"
        REQUIRE_EQUAL(std::string("INVALID_CMD"), ih.resolve("r"));
        REQUIRE_EQUAL(std::string("norandom"), ih.resolve("n"));

        // Rename an alias: move "restart" to "reboot" and verify
        REQUIRE(ih.registerAlias("temp_restart", "restart"));
        REQUIRE_EQUAL(std::string("restart"), ih.resolve("temp_restart"));

        // multiplier: prefix digits should be accepted for commands that allow
        // multipliers
        REQUIRE_EQUAL(std::string("left"), ih.resolve("3lef"));
        REQUIRE_EQUAL(std::string("right"), ih.resolve("3ri"));
        // multiplier disallowed for restart -> invalid
        REQUIRE_EQUAL(std::string("INVALID_CMD"), ih.resolve("3restart"));
    }
}

TEST_CASE(InputHandler_rename) {
    REQUIRE(ih.rename("temp_restart", "reboot"));
    REQUIRE_EQUAL(std::string("restart"), ih.resolve("reboot"));
    REQUIRE_EQUAL(std::string("INVALID_CMD"), ih.resolve("temp_restart"));
}
