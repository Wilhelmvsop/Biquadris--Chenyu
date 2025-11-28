import <iostream>;
import <string>;
import <vector>;

import InputHandler;
import TestRunner;
using namespace std;

InputHandler ih;

void InputHandlerRegister() {
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

    // Register canonical commands and common aliases from the spec

    // Register aliases
    // NOTE: already change to automatically register default aliases
    // but this still works because we allow rewrite command for same alias
    for (auto& p : aliases) {
        Tester::assert_true(ih.registerAlias(p.first, p.second));
    }
}

void InputHandler_resolve() {  // resolve checks: exact matches
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

    for (auto& p : aliases) {
        Tester::assert_equal(p.second, ih.resolve(p.first));
        // Unique-prefix resolution checks
        // "levelup" vs "leveldown" -> "lev" ambiguous, specific prefixes should
        // resolve
        Tester::assert_equal(std::string("INVALID_CMD"), ih.resolve("lev"));
        Tester::assert_equal(std::string("levelup"), ih.resolve("levelu"));
        Tester::assert_equal(std::string("leveldown"), ih.resolve("leveld"));

        // Single-letter prefix 'r' is ambiguous because of "right" and "random"
        Tester::assert_equal(std::string("INVALID_CMD"), ih.resolve("r"));
        Tester::assert_equal(std::string("norandom"), ih.resolve("n"));

        // Rename an alias: move "restart" to "reboot" and verify
        Tester::assert_true(ih.registerAlias("temp_restart", "restart"));
        Tester::assert_equal(std::string("restart"),
                             ih.resolve("temp_restart"));

        // multiplier: prefix digits should be accepted for commands that allow
        // multipliers
        Tester::assert_equal(std::string("left"), ih.resolve("3lef"));
        Tester::assert_equal(std::string("right"), ih.resolve("3ri"));
        // multiplier disallowed for restart -> invalid
        Tester::assert_equal(std::string("INVALID_CMD"),
                             ih.resolve("3restart"));
    }
}

void InputHandlerRename() {
    Tester::assert_true(ih.rename("temp_restart", "reboot"));
    Tester::assert_equal(std::string("restart"), ih.resolve("reboot"));
    Tester::assert_equal(std::string("INVALID_CMD"),
                         ih.resolve("temp_restart"));
}

namespace {
Tester::TestRegistrar r6("IH register", InputHandlerRegister);
Tester::TestRegistrar r7("IH resolve", InputHandler_resolve);
Tester::TestRegistrar r8("IH rename", InputHandlerRename);
}  // namespace
