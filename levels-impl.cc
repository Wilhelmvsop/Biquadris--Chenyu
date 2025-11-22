module Levels;

import Blocks;

import <cstdlib>;
import <fstream>;
import <iostream>;
import <string>;
import <utility>;

const std::string DEFAULT_SOURCE_FILE = "sequence1.txt";

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
        src = new std::ifstream{srcfile};
    } else {
        std::srand(seed);
    }
}

int Level::getLevelNum() { return levelNum; }
Debuff Level::getDebuff() { return effect; }
bool Level::getRandom() { return random; }
unsigned int Level::getSeed() { return seed; }
std::string Level::getSrcfile() { return srcfile; }

void Level::setRandom(bool random) { this->random = random; }
void Level::setSrcfile(std::string srcfile) {
    this->srcfile = srcfile;
    if (!random) {
        if (src) delete src;
        src = new std::ifstream{srcfile};
    }
}
void Level::setSeed(unsigned int seed) { this->seed = seed; }

Block* Level::charToBlock(char c, int levelNum) {
    switch (c) {
        case 'I':
            return new IBlock(levelNum);
        case 'J':
            return new JBlock(levelNum);
        case 'L':
            return new LBlock(levelNum);
        case 'O':
            return new OBlock(levelNum);
        case 'S':
            return new SBlock(levelNum);
        case 'Z':
            return new ZBlock(levelNum);
        case 'T':
            return new TBlock(levelNum);
        default:
            return nullptr;
    }
}

Level::~Level() {
    if (src) delete src;
    // TODO: might have to delete the bomb pointer here
}

/////////////////////////// L0 ///////////////////////////

Level0::Level0(std::string srcfile) : Level{0, false, srcfile, 1} {}

Block* Level0::getNextBlock() {
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

Block* Level1::getNextBlock() {
    int rand = std::rand() % 12;
    if (rand == 0) {
        return new SBlock(getLevelNum());
    } else if (rand <= 1) {
        return new ZBlock(getLevelNum());
    } else if (rand <= 3) {
        return new LBlock(getLevelNum());
    } else if (rand <= 5) {
        return new OBlock(getLevelNum());
    } else if (rand <= 7) {
        return new TBlock(getLevelNum());
    } else if (rand <= 9) {
        return new JBlock(getLevelNum());
    } else {
        return new IBlock(getLevelNum());
    }
}

/////////////////////////// L2 ///////////////////////////

Level2::Level2(unsigned int seed) : Level{2, true, DEFAULT_SOURCE_FILE, seed} {}

Block* Level2::getNextBlock() {
    int rand = std::rand() % 7;
    if (rand == 0) {
        return new SBlock(getLevelNum());
    } else if (rand == 1) {
        return new ZBlock(getLevelNum());
    } else if (rand == 2) {
        return new LBlock(getLevelNum());
    } else if (rand == 3) {
        return new OBlock(getLevelNum());
    } else if (rand == 4) {
        return new TBlock(getLevelNum());
    } else if (rand == 5) {
        return new JBlock(getLevelNum());
    } else {
        return new IBlock(getLevelNum());
    }
}

/////////////////////////// L3 ///////////////////////////

Level3::Level3(unsigned int seed)
    : Level{3, true, DEFAULT_SOURCE_FILE, seed, Debuff{1, false, nullptr, {}}} {
}

Block* Level3::randomNextBlock() {
    int rand = std::rand() % 9;
    if (rand <= 1) {
        return new SBlock(getLevelNum());
    } else if (rand <= 3) {
        return new ZBlock(getLevelNum());
    } else if (rand == 4) {
        return new LBlock(getLevelNum());
    } else if (rand == 5) {
        return new OBlock(getLevelNum());
    } else if (rand == 6) {
        return new TBlock(getLevelNum());
    } else if (rand == 7) {
        return new JBlock(getLevelNum());
    } else {
        return new IBlock(getLevelNum());
    }
}

// requires: src must be valid and open
Block* Level3::getNextBlockFromSrc() {
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

Block* Level3::getNextBlock() {
    if (random) {
        return randomNextBlock();
    } else {
        return getNextBlockFromSrc();
    }
}

/////////////////////////// L4 ///////////////////////////

Level4::Level4(unsigned int seed)
    : Level{4, true, DEFAULT_SOURCE_FILE, seed,
            Debuff{1, false, nullptr, {new BombBlockCat{getLevelNum()}, 5}}} {}

Block* Level4::randomNextBlock() {
    int rand = std::rand() % 9;
    if (rand <= 1) {
        return new SBlock(getLevelNum());
    } else if (rand <= 3) {
        return new ZBlock(getLevelNum());
    } else if (rand == 4) {
        return new LBlock(getLevelNum());
    } else if (rand == 5) {
        return new OBlock(getLevelNum());
    } else if (rand == 6) {
        return new TBlock(getLevelNum());
    } else if (rand == 7) {
        return new JBlock(getLevelNum());
    } else {
        return new IBlock(getLevelNum());
    }
}

// requires: src must be valid and open
Block* Level4::getNextBlockFromSrc() {
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

Block* Level4::getNextBlock() {
    if (random) {
        return randomNextBlock();
    } else {
        return getNextBlockFromSrc();
    }
}

/////////////////////////// LevelFactory ///////////////////////////

// createLevel return a new level according to levelNum
// and set the seed and srcfile to a level. returns nullptr if level invalid
Level* LevelFactory::createLevel(int levelNum, unsigned int seed,
                                 std::string srcfile) {
    Level* res;
    switch (levelNum) {
        case 0:
            res = new Level0(srcfile);
            res->setSeed(seed);
            return res;
        case 1:
            res = new Level1(seed);
            res->setSrcfile(srcfile);
            return res;
        case 2:
            res = new Level2(seed);
            res->setSrcfile(srcfile);
            return res;
        case 3:
            res = new Level3(seed);
            res->setSrcfile(srcfile);
            return res;
        case 4:
            res = new Level4(seed);
            res->setSrcfile(srcfile);
            return res;
        default:
            return nullptr;
    }
}

// Levelup returns the one level harder of `level` and delete it.
// maintains the seed and srcfile, BUT, the randomness
// behavior is set to default of the level
Level* LevelFactory::levelup(Level* level) {
    int levelNum = level->getLevelNum();
    if (levelNum == 4) return level;

    unsigned int seed = level->getSeed();
    std::string srcfile = level->getSrcfile();
    bool random = level->getRandom();
    delete level;

    Level* newLevel = createLevel(levelNum + 1, seed, srcfile);
    return newLevel;
}

// Levelup returns the one level easier of `level` and delete it.
// maintains the seed and srcfile, BUT, the randomness
// behavior is set to default of the level
Level* LevelFactory::leveldown(Level* level) {
    int levelNum = level->getLevelNum();
    if (levelNum == 0) return level;

    unsigned int seed = level->getSeed();
    std::string srcfile = level->getSrcfile();
    bool random = level->getRandom();
    delete level;

    Level* newLevel = createLevel(levelNum - 1, seed, srcfile);
    return newLevel;
}
