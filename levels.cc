export module Levels;

import Blocks;

import <utility>;
import <iostream>;
import <string>;

export class Level;

export struct Debuff {
    int heaviness;
    bool blind;
    Block* force;
    std::pair<Block*, int> insert;

    bool operator==(const Debuff& other) const;
    // add up the debuff
    Debuff operator+(const Debuff& other) const;
    void operator+=(const Debuff& other);
};

export class LevelFactory {
   public:
    // createLevel return a new level according to levelNum
    // and set the seed and srcfile to a level. returns nullptr if level invalid
    Level* createLevel(int levelNum = 0, unsigned int seed = 1,
                       std::string srcfile = "");

    // Levelup returns the one level harder of `level` and delete the old one.
    // maintains the seed and srcfile, BUT, the randomness
    // behavior is set to default of the level
    Level* levelup(Level* level);

    // Levelup returns the one level easier of `level` and delete the old one.
    // maintains the seed and srcfile, BUT, the randomness
    // behavior is set to default of the level
    Level* leveldown(Level* level);
};

class Level {
   protected:
    static Block* charToBlock(char c, int levelNum = 0);

    int levelNum;
    Debuff effect;

    bool random;
    unsigned int seed;

    std::istream* src;
    std::string srcfile;

   public:
    Level(int levelNum, bool random, std::string srcfile, unsigned int seed,
          Debuff debuff = Debuff{});
    virtual Block* getNextBlock() = 0;

    // getters and setters:
    int getLevelNum() const;
    Debuff getDebuff() const;
    bool getRandom() const;
    unsigned int getSeed() const;
    std::string getSrcfile() const;

    void setRandom(bool random);
    // set source file name and open it if need be (level 0 or norandom)
    // in prod build, throw error message if file not exists
    void setSrcfile(std::string srcfile);
    void setSeed(unsigned int seed);

    virtual ~Level();

    friend class LevelFactory;
};

export class Level0 : public Level {
   public:
    Level0(std::string srcfile);
    Block* getNextBlock() override;
};

export class Level1 : public Level {
   public:
    Level1(unsigned int seed = 1);
    Block* getNextBlock() override;
};

export class Level2 : public Level {
    unsigned int seed;

   public:
    Level2(unsigned int seed = 1);
    Block* getNextBlock() override;
};

export class Level3 : public Level {
    Block* randomNextBlock();
    Block* getNextBlockFromSrc();

   public:
    Level3(unsigned int seed = 1);
    Block* getNextBlock() override;
};

export class Level4 : public Level {
    Block* randomNextBlock();
    Block* getNextBlockFromSrc();

   public:
    Level4(unsigned int seed = 1);
    Block* getNextBlock() override;
};
