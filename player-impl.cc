module Player;

import Levels;
import Blocks;
import Board;

import <iostream>;
import <string>;
import <tuple>;
import <utility>;

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
    board->setNextBlock(nextBlock);
}

Player::~Player() {
    delete level;
    delete board;
}

Block* Player::getCurrentBlock() const { return board->getCurrentBlock(); }
Block* Player::getNextBlock() const { return board->getNextBlock(); }
int Player::getScore() const { return score; }
int Player::getHighscore() const { return highscore; }
int Player::getLevelNum() const { return level->getLevelNum(); }

const char (*Player::getPixels(const Debuff& debuff) const)[11] {
    // add up the current debuff with permanent debuff
    Debuff curDebuff = debuff + debuffs;
    Board::Canvas& canvas = board->getCanvas();
    char (*res)[11] = new char[18][11];
    for (int row = 0; row < 18; ++row) {
        for (int col = 0; col < 11; ++col) {
            if (curDebuff.blind && row >= 2 && row <= 11 && col >= 2 &&
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
PlayResult Player::play(const std::string& command, const Debuff& playDebuff,
                        const std::string& extra) {
    // add up current turn debuff with our permanent debuff
    Debuff debuff = playDebuff + debuffs;
    if (debuff.force) {
        board->setCurrentBlock(debuff.force);
    }

    PlayResult res{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    if (command == "left") {
        board->left();
        for (int i = 0; i < debuff.heaviness; ++i) {
            board->down();
        }
    } else if (command == "right") {
        board->right();
        for (int i = 0; i < debuff.heaviness; ++i) {
            board->down();
        }

    } else if (command == "down") {
        board->down();
    } else if (command == "clockwise") {
        board->rotate(true);
    } else if (command == "counterclockwise") {
        board->rotate(false);
    } else if (command == "drop") {
        res.status = PlayStatus::Endturn;
        auto dropResult = board->drop();
        int numRowsCleared = std::get<1>(dropResult);

        // deal with scores
        score += calculateRowScore(numRowsCleared);
        score += calculateBlockScore(std::get<2>(dropResult));
        if (score > highscore) highscore = score;

        // check if lost
        bool curBlockFit = std::get<0>(dropResult);
        if (!curBlockFit) {
            res.status = PlayStatus::Lost;
            return res;
        }

        // check special action
        if (numRowsCleared >= 2) {
            std::string specialAction;
#ifndef TESTING
            std::cout << "Choose your special action (blind, heavy, force "
                         "[block]): ";
#endif
            while ((*input) >> specialAction) {
                if ("blind" == specialAction) {
                    res.debuff.blind = true;
                    break;
                } else if ("heavy" == specialAction) {
                    res.debuff.heaviness = 1;
                    break;
                } else if ("force" == specialAction) {
                    char forcedBlock;
                    BlockFactory bf;
                    (*input) >> forcedBlock;
                    Block* forcedBlockPtr =
                        bf.createBlock(forcedBlock, level->getLevelNum());
                    res.debuff.force = forcedBlockPtr;
                    break;
                } else {
#ifndef TESTING
                    std::cout << std::endl << "Invalid effect. Try again: ";
#endif
                }
            }
        }

        // deal with debuff.insert
        ++numBlocksPlaced;
        Block* insertedBlock = debuff.insert.first;
        int whenToInsert = debuff.insert.second;
        if (insertedBlock && (numBlocksPlaced % whenToInsert == 0)) {
            Block* currentBlock = board->getCurrentBlock();
            Block* clonedCurrentBlock = currentBlock->clone();
            // this will delete currentBlock, so we need to clone it before hand
            board->setCurrentBlock(insertedBlock->clone());
            board->setNextBlock(clonedCurrentBlock);
            auto bombDropResult = board->drop();
            score += calculateRowScore(std::get<1>(bombDropResult));
            score += calculateBlockScore(std::get<2>(bombDropResult));
            bool curBlockFit = std::get<0>(bombDropResult);
            if (!curBlockFit) {
                res.status = PlayStatus::Lost;
                return res;
            }
        }

        board->setNextBlock(level->getNextBlock());
    } else if (command == "levelup") {
        LevelFactory lf;
        level = lf.levelup(level);
    } else if (command == "leveldown") {
        LevelFactory lf;
        level = lf.leveldown(level);
    } else if (command == "norandom") {
        // turn off random mode for level 3-4
        // then set the srcfile to the target srcfile
        // (if level 0-2, do nothing)
        if (level->getLevelNum() >= 3) {
            std::string srcfile = extra;
            level->setRandom(false);
            level->setSrcfile(srcfile);
        }

    } else if (command == "random") {
        // turn on random mode for level 3-4 (if level 0-2, do nothing)

        int levelNum = level->getLevelNum();
        if (levelNum >= 3) {
            level->setRandom(true);
        }
        // for command I, J, ...
    } else if (command.length() == 1 &&
               BlockFactory().isValidChar(command[0])) {
        board->setCurrentBlock(BlockFactory().createBlock(command[0]));
    } else if (command == "restart") {
        score = 0;
        numBlocksPlaced = 0;

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
        board->setNextBlock(nextBlock);

        // reset debuff
        debuffs = level->getDebuff();
    } else {
        throw "invalid command";
    }

    return res;
}
