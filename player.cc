export module Player;

import Board;
import Blocks;
import Levels;

import <string>;
import <vector>;
import <memory>;

export enum class PlayStatus { Endturn, Continue, Lost };

export struct PlayResult {
    PlayStatus status;
    Debuff debuff;

    bool operator==(const PlayResult& other) const {
        return status == other.status && debuff == other.debuff;
    }
};

export class Player {
    std::shared_ptr<Level> level;
    std::shared_ptr<Board> board;
    Debuff debuffs;
    int initLevel;  // initial level when player is constructed
    int numBlocksPlaced;
    int score;
    int highscore;
    std::shared_ptr<std::istream>
        input;  // source when prompt something back to real player

    int calculateRowScore(int numRowsCleared) const;
    // calculate scores from blocksCleared and delete them
    int calculateBlockScore(
        const std::vector<std::shared_ptr<Block>>& blocksCleared) const;

   public:
    // take the target level and set up a new board (player owns the board)
    Player(std::shared_ptr<Level> level,
           std::shared_ptr<std::istream> input = nullptr);
    ~Player();
    bool updateDebuff(const Debuff& d);
    // play the target command, debuff and extra argument (for any command that
    // needs extra argument) then return the play result
    // throw a string "invalid command" if command invalid.
    PlayResult play(const std::string& command, const std::string& extra = "",
                    bool lastRep = true);
    // get current debuff in the turn and returns
    // const 18x11 vector represent pixels in the board
    std::vector<std::vector<char>> getPixels() const;
    std::shared_ptr<Block> getCurrentBlock() const;
    std::shared_ptr<Block> getNextBlock() const;
    int getScore() const;
    int getHighscore() const;
    int getLevelNum() const;

    // set new source of input and delete the old one
    void setInput(std::shared_ptr<std::istream> newInput);
};
