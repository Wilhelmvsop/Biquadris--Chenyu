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
    int numBlocksPlaced;
    int score;
    int highscore;

   public:
    Player(Level* level);
    ~Player();
    PlayResult play(std::string command, Debuff debuff);
    // return const 18x11 array represent pixels in the board
    // require: caller must delete[] after use
    const char (*getPixels() const)[11];
    Block* getNextBlock() const;
    int getScore() const;
    int getHighscore() const;
};
