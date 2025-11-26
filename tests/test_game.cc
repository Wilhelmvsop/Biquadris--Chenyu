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

void setupFile(const std::string& path, const std::string& content) {
    std::ofstream f{path};
    f << content;
}

void removeFile(const std::string& path) {
    std::filesystem::remove(path);
}

// Register the default aliases used by the game
void registerDefaultAliases(InputHandler& ih) {
    std::vector<std::pair<std::string, std::string>> aliases = {
        {"left", "left"}, {"right", "right"}, {"down", "down"}, {"drop", "drop"},
        {"clockwise", "clockwise"}, {"cw", "clockwise"}, {"clock", "clockwise"},
        {"counterclockwise", "counterclockwise"}, {"ccw", "counterclockwise"},
        {"levelup", "levelup"}, {"leveldown", "leveldown"}, {"norandom", "norandom"},
        {"random", "random"}, {"sequence", "sequence"}, {"restart", "restart"},
        {"I", "I"}, {"J", "J"}, {"L", "L"}, {"O", "O"}, {"S", "S"}, {"Z", "Z"}, {"T", "T"}
    };
    for (auto &p : aliases) ih.registerAlias(p.first, p.second);
}

// Capturing renderer that stores snapshots of render packages
struct CapturedPkg {
    int score1;
    int high1;
    int level1;
    std::vector<std::string> pixels1;

    int score2;
    int high2;
    int level2;
    std::vector<std::string> pixels2;
};

class CapturingRenderer : public Renderer {
  public:
    std::vector<CapturedPkg> caps;
    void render(const RenderPackage &p1, const RenderPackage &p2) override {
        CapturedPkg c;
        c.score1 = p1.score; c.high1 = p1.highscore; c.level1 = p1.level;
        for (int r = 0; r < 18; ++r) c.pixels1.emplace_back(std::string(p1.pixels[r], 11));
        c.score2 = p2.score; c.high2 = p2.highscore; c.level2 = p2.level;
        for (int r = 0; r < 18; ++r) c.pixels2.emplace_back(std::string(p2.pixels[r], 11));
        caps.emplace_back(std::move(c));
    }
};

TEST_CASE(Game_PlayRestart) {
    std::string seqPath = "game_test_seq.txt";
    // create a nr file for deterministic sequence used by both players
    std::string nrPath = "game_test_nr.txt";
    std::string nrContent = "I J Z L O I J Z L O I J Z L O";
    setupFile(nrPath, nrContent);

    // Build a command stream: both players switch to non-random, then each drop once to gain score
    std::stringstream ss;
    ss << "norandom " << nrPath << "\n";  // player 1
    ss << "norandom " << nrPath << "\n";  // player 2
    // both players drop to place blocks and possibly score
    ss << "drop\n"; // p1 drop
    ss << "drop\n"; // p2 drop
    // now p1 drop again
    ss << "drop\n"; // p1 drop
    // force restart
    ss << "restart\n";
    // then drop once more
    ss << "drop\n"; // p1 drop after restart

    InputHandler ih;
    registerDefaultAliases(ih);

    CapturingRenderer r;
    std::vector<Renderer*> renders{&r};

    Game g{&ss, &ih, renders};
    g.play();

    // There must be at least a couple of render frames
    REQUIRE(!r.caps.empty());

    // Find a captured frame before and after the restart by scanning captures for a reset in score
    int before_idx = -1, after_idx = -1;
    for (size_t i = 0; i < r.caps.size(); ++i) {
        if (r.caps[i].score1 > 0) { before_idx = i; break; }
    }
    for (size_t i = 0; i < r.caps.size(); ++i) {
        if (r.caps[i].score1 == 0 && r.caps[i].high1 > 0) { after_idx = i; break; }
    }
    REQUIRE(before_idx >= 0);
    REQUIRE(after_idx >= 0);
    // Ensure that a restart actually cleared the score but preserved highscore
    REQUIRE(r.caps[after_idx].score1 == 0);
    REQUIRE(r.caps[after_idx].high1 >= r.caps[before_idx].high1);

    removeFile(nrPath);
}

TEST_CASE(Game_SequenceFile) {
    // main input triggers sequence which contains a couple of drop commands
    std::string seqPath = "game_commands.txt";
    std::string seqContent;
    seqContent += "norandom game_nr.txt\n";
    seqContent += "drop\n";
    seqContent += "drop\n";
    setupFile(seqPath, seqContent);

    // Create a non-random file referenced by the sequence file
    std::string nrPath = "game_nr.txt";
    setupFile(nrPath, "I J Z L O I J Z L O");

    std::stringstream ss;
    ss << "sequence " << seqPath << "\n"; // instruct game to switch to the sequence file

    InputHandler ih;
    registerDefaultAliases(ih);
    CapturingRenderer cr;
    std::vector<Renderer*> renders{&cr};

    Game g{&ss, &ih, renders};
    g.play();

    // Validate that the sequence file affected gameplay: at least one frame should show a non-zero score
    bool sawScore = false;
    for (const auto &c : cr.caps) {
        if (c.score1 > 0 || c.score2 > 0) sawScore = true;
    }
    REQUIRE(sawScore == true);

    removeFile(seqPath);
    removeFile(nrPath);
}
