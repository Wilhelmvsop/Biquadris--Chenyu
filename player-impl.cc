module Player;

Player::Player(Level* level)
    : level{level},
      board{new Board()},
      debuffs{level->getDebuff()},
      numBlocksPlaced{0},
      score{0},
      highscore{0} {}

Player::~Player() {
    delete level;
    delete board;
}

Block* Player::getNextBlock() { return board->getNextBlock(); }
int Player::getScore() { return score; }
int Player::getHighscore() { return highscore; }

const char (*Player::getPixels())[11] {
    Canvas& canvas = board->getCanvas();
    char (*res)[11] = new char[18][11];
    for (int row = 0; row < 18; ++row) {
        for (int col = 0; col < 11; ++col) {
            if (debuffs.blind && row >= 2 && row <= 11 && col >= 2 &&
                col <= 8) {
                res[row][col] = '?';
            } else if (canvas[row][col]) {
                res[row][col] = canvas[row][col]->getChar();
            } else {
                res[row][col] = ' ';
            }
        }
    }
    return res;
}

PlayResult Player::play(std::string command, Debuff debuff) {
    return PlayResult{};  // TODO
}
