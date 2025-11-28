module Player;

import Levels;
import Blocks;
import Board;

import <iostream>;
import <string>;
import <tuple>;
import <utility>;
import <memory>;
import <stdexcept>;

Player::Player(std::shared_ptr<Level> level,
               std::shared_ptr<std::istream> input)
    : level{level},
      board{std::make_shared<Board>()},
      debuffs{level->getDebuff()},
      initLevel{level->getLevelNum()},
      numBlocksPlaced{0},
      score{0},
      highscore{0},
      input{input} {
    std::shared_ptr<Block> firstBlock = level->getNextBlock();
    board->setCurrentBlock(firstBlock);
    std::shared_ptr<Block> nextBlock = level->getNextBlock();
    board->setNextBlock(nextBlock);
}

Player::~Player() {}

bool Player::updateDebuff(const Debuff& d) {
    debuffs = level->getDebuff() + d;
    bool fits = true;
    if (debuffs.force) {
        fits = board->setCurrentBlock(debuffs.force);
    }
    return fits;
}

std::shared_ptr<Block> Player::getCurrentBlock() const {
    return board->getCurrentBlock();
}

std::shared_ptr<Block> Player::getNextBlock() const {
    return board->getNextBlock();
}

int Player::getScore() const { return score; }
int Player::getHighscore() const { return highscore; }
int Player::getLevelNum() const { return level->getLevelNum(); }

void Player::say(const std::string& msg) const {
#ifndef TESTING
    std::cout << msg << std::endl;
#endif
}

std::vector<std::vector<char>> Player::getPixels() const {
    Board::Canvas& canvas = this->board->getCanvas();
    std::vector<std::vector<char>> res(18, std::vector<char>(11, ' '));
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

void Player::setInput(std::shared_ptr<std::istream> newInput) {
    input = newInput;
}

int Player::calculateRowScore(int numRowsCleared) const {
    if (!numRowsCleared) return 0;
    int base = level->getLevelNum() + numRowsCleared;
    return base * base;
}

int Player::calculateBlockScore(
    const std::vector<std::shared_ptr<Block>>& blocksCleared) const {
    int res = 0;
    for (auto blockp : blocksCleared) {
        int base = (blockp->getMotherLevel() + 1);
        res += (base * base);
    }
    return res;
}

// TODO: centralize command, not in here
PlayResult Player::play(const std::string& command, const std::string& extra,
                        bool lastRep) {
    // add up current turn debuff with our permanent debuff
    PlayResult res{PlayStatus::Continue, {}};
    if (command == "left") {
        board->left();
        if (lastRep) {
            for (int i = 0; i < debuffs.heaviness; ++i) {
                board->down();
            }
        }
    } else if (command == "right") {
        board->right();
        if (lastRep) {
            for (int i = 0; i < debuffs.heaviness; ++i) {
                board->down();
            }
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
        }

        // check special action
        if (numRowsCleared >= 2) {
            std::string specialAction;
            const std::string msg =
                "Choose your special action (blind, heavy, force [block]):";
            say(msg);
            while (getline(*input, specialAction)) {
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
                    std::shared_ptr<Block> forcedBlockPtr =
                        bf.createBlock(forcedBlock, level->getLevelNum());
                    res.debuff.force = forcedBlockPtr;
                    break;
                } else {
                    say("Invalid effect. Try again.");
                }
            }
        }

        // deal with debuff.insert
        ++numBlocksPlaced;
        std::shared_ptr<Block> insertedBlock = debuffs.insert.first;
        int whenToInsert = debuffs.insert.second;
        if (insertedBlock && (numBlocksPlaced % whenToInsert == 0)) {
            std::shared_ptr<Block> currentBlock = board->getCurrentBlock();
            std::shared_ptr<Block> clonedCurrentBlock = currentBlock->clone();
            // this will delete currentBlock, so we need to clone it before hand
            board->setCurrentBlock(insertedBlock->clone());
            board->setNextBlock(clonedCurrentBlock);
            auto bombDropResult = board->drop();
            score += calculateRowScore(std::get<1>(bombDropResult));
            score += calculateBlockScore(std::get<2>(bombDropResult));
            bool curBlockFit = std::get<0>(bombDropResult);
            if (!curBlockFit) {
                res.status = PlayStatus::Lost;
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
        level = lf.createLevel(initLevel, seed, srcfile);

        // reset board
        board = std::make_shared<Board>();
        std::shared_ptr<Block> firstBlock = level->getNextBlock();
        board->setCurrentBlock(firstBlock);
        std::shared_ptr<Block> nextBlock = level->getNextBlock();
        board->setNextBlock(nextBlock);

        // reset debuff
        debuffs = level->getDebuff();
    } else {
        throw std::runtime_error("invalid command");
    }

    return res;
}
