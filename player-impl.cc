module Player;

import Levels;
import Blocks;
import Board;

import <iostream>;
import <string>;
import <tuple>;

Player::Player(Level* level, std::istream* input)
    : level{level},
      board{new Board()},
      debuffs{level->getDebuff()},
      initLevel{level->getLevelNum()},
      numBlocksPlaced{0},
      score{0},
      highscore{0},
      input{input} {
    Block* firstBlock = level->getNextBlock();
    board->setCurrentBlock(firstBlock);
    Block* nextBlock = level->getNextBlock();
    board->setNextBlock(firstBlock);
}

Player::~Player() {
    delete level;
    delete board;
}

Block* Player::getNextBlock() const { return board->getNextBlock(); }
int Player::getScore() const { return score; }
int Player::getHighscore() const { return highscore; }

const char (*Player::getPixels(const Debuff& debuff) const)[11] {
    debuff += debuffs;  // add up the current debuff with permanent debuff
    Canvas& canvas = board->getCanvas();
    char (*res)[11] = new char[18][11];
    for (int row = 0; row < 18; ++row) {
        for (int col = 0; col < 11; ++col) {
            if (debuff.blind && row >= 2 && row <= 11 && col >= 2 && col <= 8) {
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

void Player::setInput(std::istream* newInput) {
    if (input != newInput) delete input;
    input = newInput;
}

int Player::calculateRowScore(int numRowsCleared) const {
    int base = level->getLevelNum() + numRowsCleared;
    return base * base;
}

int Player::calculateBlockScore(
    const std::vector<Block*>& blocksCleared) const {
    int res = 0;
    for (auto blockp : blocksCleared) {
        int base = (blockp->getMotherLevel() + 1);
        res += (base * base);
        delete blockp;
    }
    return res;
}

// TODO: centralize command, not in here
PlayResult Player::play(const std::string& command, const Debuff& debuff,
                        const std::string& extra) {
    debuff += debuffs;  // add up current turn debuff with our permanent debuff

    if (debuff.force) {
        board->setCurrentBlock(debuff.force);
    }

    PlayResult res{PlayStatus::Continue, {0, false, nullptr, {nullptr, 5}}};
    switch (command) {
        case "left":
            board->left();
            for (int i = 0; i < debuff.heaviness; ++i) {
                board->down();
            }
            break;
        case "right":
            board->right();
            for (int i = 0; i < debuff.heaviness; ++i) {
                board->down();
            }
            break;
        case "down":
            board->down();
            break;
        case "clockwise":
            board->rotate(true);
            break;
        case "counterclockwise":
            board->rotate(false);
            break;
        case "drop":
            auto dropResult = board->drop();
            int numRowsCleared = std::get<1>(dropResult);

            // deal with scores
            score += calculateRowScore(numRowsCleared);
            score += calculateBlockScore(std::get<2>(dropResult));
            if (score > highscore) highscore = score;

            // check if lost
            if (!std::get<0>(dropResult)) {
                res.status = PlayStatus::Lost;
                return res;
            }

            // check special action
            if (numRowsCleared >= 2) {
                std::string specialAction;
                // TODO: don't want to contaminate my test. but also want user
                // to see the prompt
                std::cout << "Choose your special action (blind, heavy, force "
                             "[block]): ";
                while (input >> specialAction) {
                    if ("blind" == specialAction) {
                        res.debuff.blind = true;
                        break;
                    } else if ("heavy" == specialAction) {
                        res.debuff.heavy = 1;
                        break;
                    } else if ("force" == specialAction) {
                        char forcedBlock;
                        BlockFactory bf;
                        input >> forcedBlock;
                        Block* forcedBlockPtr =
                            bf.createBlock(forcedBlock, level->getLevelNum());
                        res.debuff.force = forcedBlockPtr;
                        break;
                    } else {
                        std::cout << std::endl << "Invalid effect. Try again: ";
                    }
                }
            }

            ++numBlockPlaced;
            if (debuff.insert.first &&
                (numBlocksPlaced % debuff.insert.second) == 0) {
            }

            break;
        case "levelup":
            LevelFactory lf;
            level = lf.levelup(level);
            break;
        case "leveldown":
            LevelFactory lf;
            level = lf.leveldown(level);
            break;
        // turn off random mode for level 3-4
        // then set the srcfile to the target srcfile
        // (if level 0-2, do nothing)
        case "norandom":
            if (level->getLevelNum() >= 3) {
                std::string srcfile = extra;
                level->setRandom(false);
                level->setSrcfile(srcfile);
            }
            break;

        // turn on random mode for level 3-4 (if level 0-2, do nothing)
        case "random":
            int levelNum = level->getLevelNum();
            if (levelNum >= 3) {
                level->setRandom(true);
            }
            break;
        case "restart":
            score = 0;
            numBlocksPlace = 0;

            // reset level
            LevelFactory lf{};
            unsigned int seed = level->getSeed();
            std::string srcfile = level->getSrcfile();
            delete level;
            level = lf.createLevel(initLevel, seed, srcfile);

            // reset board
            delete board;
            board = new Board{};
            Block* firstBlock = level->getNextBlock();
            board->setCurrentBlock(firstBlock);
            Block* nextBlock = level->getNextBlock();
            board->setNextBlock(firstBlock);

            // reset debuff
            debuffs = level->getDebuff();

            break;
        default:
            throw "invalid command"
    }
    return res;
}
