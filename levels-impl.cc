module Levels;

import Blocks;

import <cstdlib>;
import <fstream>;
import <iostream>;
import <string>;
import <utility>;
import <memory>;

const std::string DEFAULT_SOURCE_FILE = "sequence1.txt";

/////////////////////////// Debuff ///////////////////////////

bool Debuff::operator==(const Debuff& other) const {
    bool insertEq = false;
    if (insert.first && other.insert.first) {
        insertEq = insert.first->getChar() == other.insert.first->getChar();
    } else if (!insert.first && !other.insert.first) {
        insertEq = true;
    } else {
        insertEq = false;
    }

    bool forceEq = false;
    if (force && other.force) {
        forceEq = force->getChar() == other.force->getChar();
    } else if (!force && !other.force) {
        forceEq = true;
    } else {
        forceEq = false;
    }

    return (insertEq && insert.second == other.insert.second &&
            heaviness == other.heaviness && blind == other.blind && forceEq);
}

// add up the debuff
Debuff Debuff::operator+(const Debuff& other) const {
    Debuff res{};
    res.heaviness = heaviness + other.heaviness;
    res.blind = blind || other.blind;
    res.force = force ? force : other.force;
    res.insert = insert.first ? insert : other.insert;
    return res;
}

void Debuff::operator+=(const Debuff& other) { *this = (*this) + other; }

/////////////////////////// Level ///////////////////////////

Level::Level(int levelNum, bool random, std::string srcfile, unsigned int seed,
             Debuff debuff)
    : levelNum{levelNum},
      srcfile{srcfile},
      src{nullptr},
      random{random},
      seed{seed},
      effect{debuff} {
    if (!random) {
        src = std::make_shared<std::fstream>(srcfile);
        // check if file exists when build for prod
#ifndef TESTING
        if (!(*src)) {
            throw "srcfile " + srcfile + " not exists.";
        }
#endif
    } else {
        std::srand(seed);
    }
}

int Level::getLevelNum() const { return levelNum; }
Debuff Level::getDebuff() const { return effect; }
bool Level::getRandom() const { return random; }
unsigned int Level::getSeed() const { return seed; }
std::string Level::getSrcfile() const { return srcfile; }

void Level::setRandom(bool random) { this->random = random; }
void Level::setSrcfile(std::string srcfile) {
    if (this->srcfile == srcfile) return;
    this->srcfile = srcfile;
    if (!random) {
        src = std::make_shared<std::fstream>(srcfile);
        // check if file exists when build for prod
#ifndef TESTING
        if (!(*src)) {
            throw "srcfile " + srcfile + " not exists.";
        }
#endif
    }
}
void Level::setSeed(unsigned int seed) { this->seed = seed; }

std::shared_ptr<Block> Level::charToBlock(char c, int levelNum) {
    switch (c) {
        case 'I':
            return std::make_shared<IBlock>(levelNum);
        case 'J':
            return std::make_shared<JBlock>(levelNum);
        case 'L':
            return std::make_shared<LBlock>(levelNum);
        case 'O':
            return std::make_shared<OBlock>(levelNum);
        case 'S':
            return std::make_shared<SBlock>(levelNum);
        case 'Z':
            return std::make_shared<ZBlock>(levelNum);
        case 'T':
            return std::make_shared<TBlock>(levelNum);
        default:
            return nullptr;
    }
}

Level::~Level() {}

/////////////////////////// L0 ///////////////////////////

Level0::Level0(std::string srcfile) : Level{0, false, srcfile, 1} {}

std::shared_ptr<Block> Level0::getNextBlock() {
    char nextBlockChar;
    (*src) >> nextBlockChar;

    // if end of a file, going back to the beginning
    if (src->eof()) {
        src->clear();
        src->seekg(0, std::ios::beg);
        (*src) >> nextBlockChar;
    }
    return charToBlock(nextBlockChar);
}

/////////////////////////// L1 ///////////////////////////

Level1::Level1(unsigned int seed) : Level{1, true, DEFAULT_SOURCE_FILE, seed} {}

std::shared_ptr<Block> Level1::getNextBlock() {
    int rand = std::rand() % 12;
    if (rand == 0) {
        return std::make_shared<SBlock>(getLevelNum());
    } else if (rand <= 1) {
        return std::make_shared<ZBlock>(getLevelNum());
    } else if (rand <= 3) {
        return std::make_shared<LBlock>(getLevelNum());
    } else if (rand <= 5) {
        return std::make_shared<OBlock>(getLevelNum());
    } else if (rand <= 7) {
        return std::make_shared<TBlock>(getLevelNum());
    } else if (rand <= 9) {
        return std::make_shared<JBlock>(getLevelNum());
    } else {
        return std::make_shared<IBlock>(getLevelNum());
    }
}

/////////////////////////// L2 ///////////////////////////

Level2::Level2(unsigned int seed) : Level{2, true, DEFAULT_SOURCE_FILE, seed} {}

std::shared_ptr<Block> Level2::getNextBlock() {
    int rand = std::rand() % 7;
    if (rand == 0) {
        return std::make_shared<SBlock>(getLevelNum());
    } else if (rand == 1) {
        return std::make_shared<ZBlock>(getLevelNum());
    } else if (rand == 2) {
        return std::make_shared<LBlock>(getLevelNum());
    } else if (rand == 3) {
        return std::make_shared<OBlock>(getLevelNum());
    } else if (rand == 4) {
        return std::make_shared<TBlock>(getLevelNum());
    } else if (rand == 5) {
        return std::make_shared<JBlock>(getLevelNum());
    } else {
        return std::make_shared<IBlock>(getLevelNum());
    }
}

/////////////////////////// L3 ///////////////////////////

Level3::Level3(unsigned int seed)
    : Level{3, true, DEFAULT_SOURCE_FILE, seed, Debuff{1, false, nullptr, {}}} {
}

std::shared_ptr<Block> Level3::randomNextBlock() {
    int rand = std::rand() % 9;
    if (rand <= 1) {
        return std::make_shared<SBlock>(getLevelNum());
    } else if (rand <= 3) {
        return std::make_shared<ZBlock>(getLevelNum());
    } else if (rand == 4) {
        return std::make_shared<LBlock>(getLevelNum());
    } else if (rand == 5) {
        return std::make_shared<OBlock>(getLevelNum());
    } else if (rand == 6) {
        return std::make_shared<TBlock>(getLevelNum());
    } else if (rand == 7) {
        return std::make_shared<JBlock>(getLevelNum());
    } else {
        return std::make_shared<IBlock>(getLevelNum());
    }
}

// requires: src must be valid and open
std::shared_ptr<Block> Level3::getNextBlockFromSrc() {
    char nextBlockChar;
    (*src) >> nextBlockChar;

    // if end of a file, going back to the beginning
    if (src->eof()) {
        src->clear();
        src->seekg(0, std::ios::beg);
        (*src) >> nextBlockChar;
    }
    return charToBlock(nextBlockChar, getLevelNum());
}

std::shared_ptr<Block> Level3::getNextBlock() {
    if (random) {
        return randomNextBlock();
    } else {
        return getNextBlockFromSrc();
    }
}

/////////////////////////// L4 ///////////////////////////

Level4::Level4(unsigned int seed)
    : Level{4, true, DEFAULT_SOURCE_FILE, seed,
            Debuff{1, false, nullptr, std::make_pair(std::make_shared<BombBlockCat>(getLevelNum()), 5)}} {}

std::shared_ptr<Block> Level4::randomNextBlock() {
    int rand = std::rand() % 9;
    if (rand <= 1) {
        return std::make_shared<SBlock>(getLevelNum());
    } else if (rand <= 3) {
        return std::make_shared<ZBlock>(getLevelNum());
    } else if (rand == 4) {
        return std::make_shared<LBlock>(getLevelNum());
    } else if (rand == 5) {
        return std::make_shared<OBlock>(getLevelNum());
    } else if (rand == 6) {
        return std::make_shared<TBlock>(getLevelNum());
    } else if (rand == 7) {
        return std::make_shared<JBlock>(getLevelNum());
    } else {
        return std::make_shared<IBlock>(getLevelNum());
    }
}

// requires: src must be valid and open
std::shared_ptr<Block> Level4::getNextBlockFromSrc() {
    char nextBlockChar;
    (*src) >> nextBlockChar;

    // if end of a file, going back to the beginning
    if (src->eof()) {
        src->clear();
        src->seekg(0, std::ios::beg);
        (*src) >> nextBlockChar;
    }
    return charToBlock(nextBlockChar, getLevelNum());
}

std::shared_ptr<Block> Level4::getNextBlock() {
    if (random) {
        return randomNextBlock();
    } else {
        return getNextBlockFromSrc();
    }
}

/////////////////////////// LevelFactory ///////////////////////////
std::shared_ptr<Level> LevelFactory::createLevel(int levelNum, unsigned int seed,
                                 std::string srcfile) {
    std::shared_ptr<Level> res;
    switch (levelNum) {
        case 0:
            res = std::make_shared<Level0>(srcfile);
            res->setSeed(seed);
            return res;
        case 1:
            res = std::make_shared<Level1>(seed);
            res->setSrcfile(srcfile);
            return res;
        case 2:
            res = std::make_shared<Level2>(seed);
            res->setSrcfile(srcfile);
            return res;
        case 3:
            res = std::make_shared<Level3>(seed);
            res->setSrcfile(srcfile);
            return res;
        case 4:
            res = std::make_shared<Level4>(seed);
            res->setSrcfile(srcfile);
            return res;
        default:
            return nullptr;
    }
}

std::shared_ptr<Level> LevelFactory::levelup(std::shared_ptr<Level> level) {
    int levelNum = level->getLevelNum();
    if (levelNum == 4) return level;

    unsigned int seed = level->getSeed();
    std::string srcfile = level->getSrcfile();
    bool random = level->getRandom();

    return createLevel(levelNum + 1, seed, srcfile);
}

std::shared_ptr<Level> LevelFactory::leveldown(std::shared_ptr<Level> level) {
    int levelNum = level->getLevelNum();
    if (levelNum == 0) return level;

    unsigned int seed = level->getSeed();
    std::string srcfile = level->getSrcfile();
    bool random = level->getRandom();

    return createLevel(levelNum - 1, seed, srcfile);
}
