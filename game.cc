export module Game;

import InputHandler;
import Renderers;
import Levels;
import Player;

import <algorithm>;
import <fstream>;
import <iostream>;
import <memory>;
import <sstream>;
import <stdexcept>;
import <vector>;

export struct GameSettings {
    // renderers
    bool tuiOnly;

    // level
    unsigned int seed;
    std::string script1;
    std::string script2;
    int startLevel;

    // input handler
    std::shared_ptr<InputHandler> ih;
};

export class Game {
    std::shared_ptr<Player> p1;
    std::shared_ptr<Player> p2;
    std::shared_ptr<std::istream> input;
    std::shared_ptr<InputHandler> inputHandler;
    std::vector<std::shared_ptr<Renderer>> renderers;
    Debuff DebuffP1;
    Debuff DebuffP2;
    int turnCount;  // number of turns

    // helper methods
    void cleanupDebuff(Debuff& d);
    void parseFirstWord(const std::string& line, std::string& first,
                        std::string& extra);
    void processCmd(const std::string& rawFirst, const std::string& extra,
                    Player& cur, Player& other, std::istream& curInput,
                    int& whoLost);
    // for deleting input stream that is not std::cin
    void istreamDeleter(std::istream* is) {
        if (is != &std::cin) delete is;
    }

   public:
    // Game(std::istream* in = &std::cin, InputHandler* ih = nullptr);
    Game(const GameSettings& settings);
    // const std::vector<Renderer*>& renders = {});
    ~Game();
    // NOTE: for "sequence file" command, each command must end with \n
    void play();
    // helper accessor for test
    std::shared_ptr<Player> getPlayer(int idx);
};
