export module Player;

import <string>;

import Levels;
import Board;

export enum class PlayStatus { Endturn, Continue, Lost };

export struct PlayResult {
    PlayStatus status;
    Debuff debuff;
};

export class Player {
    Level* level;
    Board* board;
    Debuff debuffs;
    int initLevel;  // initial level when player is constructed
    int numBlocksPlaced;
    int score;
    int highscore;
    std::istream* input;  // source when prompt something back to real player

    int calculateRowScore(int numRowsCleared) const;
    // calculate scores from blocksCleared and delete them
    int calculateBlockScore(const std::vector<Block*>& blocksCleared) const;

   public:
    // take the target level and set up a new board (player owns the board)
    Player(Level* level, std::istream* input = nullptr);
    ~Player();
    // play the target command, debuff and extra argument (for any command that
    // needs extra argument) then return the play result
    // throw a string "invalid command" if command invalid.
    PlayResult play(const std::string& command, const Debuff& debuff,
                    const std::string& extra = "");
    // get current debuff in the turn and returns
    // const 18x11 array represent pixels in the board
    // require: caller must delete[] after use
    const char (*getPixels(const Debuff& debuff) const)[11];
    Block* getNextBlock() const;
    int getScore() const;
    int getHighscore() const;

    // set new source of input and delete the old one
    void setInput(*std::istream newInput);
};
