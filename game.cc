export module Game;

import Player;
import InputHandler;
import Levels;
import Renderers;
import <iostream>;
import <vector>;
import <fstream>;

export class Game {
        Player* p1;
        Player* p2;
        int turn;  // current turn in game (1 or 2)
        std::istream* input; 
        InputHandler* inputHandler; // command resolver; Game owns this if nullptr passed
        bool ownerOfInputHandler;
        std::vector<Renderer*> renderers;
        Debuff DebuffP1;
        Debuff DebuffP2;
        int turnCount; // number of turns
    
        // helper methods
        void cleanupDebuff(Debuff &d);
        void applyDebuffToOpponent(const Debuff &d, Player* opponent);
        void parseFirstWord(const std::string &line, std::string &first, std::string &extra);
        void processCmd(const std::string &rawFirst, const std::string &extra, int multiplier, Player* cur, Player* other, Debuff &pendingCur, Debuff &pendingOther, std::istream*& curInput, std::vector<std::ifstream*> &fileStack, bool &shouldQuit);

    public:
        Game(std::istream* in = &std::cin,
                 InputHandler* ih = nullptr,
                 const std::vector<Renderer*>& renders = {});
        ~Game();
        void play();
};

