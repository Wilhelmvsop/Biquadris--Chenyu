import Game;
import Player;
import Levels;
import Blocks;
import Renderers;
import InputHandler;

import <fstream>;
import <filesystem>;
import <sstream>;
import <iostream>;
import <vector>;

#include "test_runner.h"

// Helpers
static void setupFile(const std::string& path, const std::string& content) {
    std::ofstream f{path};
    f << content;
    f.close();
}

static void removeFile(const std::string& path) {
    std::filesystem::remove(path);
}

static void registerDefaultAliases(InputHandler& ih) {
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
        {"I", "I"},
        {"J", "J"},
        {"L", "L"},
        {"O", "O"},
        {"S", "S"},
        {"Z", "Z"},
        {"T", "T"}};
    for (auto& p : aliases) ih.registerAlias(p.first, p.second);
}

// Dummy renderer to count invocations
class DummyRenderer : public Renderer {
   public:
    int calls = 0;
    void render(const RenderPackage& p1, const RenderPackage& p2) override {
        calls++;
    }
};

/*
export struct GameSettings {
    // renderers
    bool tuiOnly;

    // level
    unsigned int seed;
    std::string script1;
    std::string script2;
    int startLevel;

    // input handler
    std::shared_ptr<istream> input;
    std::shared_ptr<InputHandler> ih;
};
 * */

// TODO: GOON
// Sequence command switches input to file and back to stream
TEST_CASE(Game_Sequence_Command_SwitchInput) {
    const std::string bs1path = "sequence1.txt";
    const std::string bs2path = "sequence2.txt";
    const std::string csPath = "command_seq.txt";
    setupFile(bs1path, "I J L O S Z T\n");
    setupFile(bs2path, "I J L O S Z T\n");
    setupFile(seqPath, "levelup\nleveldown\n");

    std::stringstream ss;
    ss << "sequence " << seqPath << "\n";
    InputHandler ih;
    registerDefaultAliases(ih);
    Game g{{
        true,
        67,
        bs1path,
        bs2path,
    }};
    g.play();
    // After levelup then leveldown, level returns to 0
    REQUIRE(g.getPlayer(1)->getLevelNum() == 0);
    removeFile(bs1path);
    removeFile(bs2path);
    removeFile(csPath);
}

// Restart resets score but keeps highscore
TEST_CASE(Game_Restart_Resets_State) {
    setupFile("sequence1.txt", "I J L O S Z T\n");
    std::string seqPath = "test_game_restart.txt";
    setupFile(seqPath, "I I J\n");
    std::stringstream ss;
    ss << "sequence " << seqPath << "\n";
    InputHandler ih;
    registerDefaultAliases(ih);
    DummyRenderer dr;
    const std::vector<Renderer*> renders{&dr};
    Game g{&ss, &ih, renders};
    g.play();
    // After playing sequence to clear a row, trigger restart
    std::stringstream cmd2;
    cmd2 << "restart\n";
    Game g2{&cmd2, &ih, renders};
    g2.play();
    // Rendering should have happened at least once
    REQUIRE(dr.calls > 0);
    removeFile("sequence1.txt");
}

// Lost terminates the loop
TEST_CASE(Game_Lost_Terminates) {
    setupFile("sequence1.txt", "I J L O S Z T\n");
    std::string seqPath = "test_game_lost.txt";
    // Enough drops to lose with a single I sequence
    setupFile(seqPath, "I\n");
    std::stringstream ss;
    ss << "sequence " << seqPath << "\n";
    InputHandler ih;
    registerDefaultAliases(ih);
    const std::vector<Renderer*> renders{};
    Game g{&ss, &ih, renders};
    g.play();
    // If the game ended, players should be valid and level unchanged
    REQUIRE(g.getPlayer(1)->getLevelNum() == 0);
    removeFile(seqPath);
    removeFile("sequence1.txt");
}
// Not working
