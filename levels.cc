export module Levels;

import Blocks;

import <utility>;
import <iostream>;
import <memory>;
import <string>;

export class Level;

export struct Debuff {
    int heaviness;
    bool blind;
    std::shared_ptr<Block> force;
    std::pair<std::shared_ptr<Block>, int> insert;

    Debuff(int heaviness = 0, bool blind = false,
           std::shared_ptr<Block> force = nullptr,
           std::pair<std::shared_ptr<Block>, int> insert = {nullptr, 1});

    bool operator==(const Debuff& other) const;
    bool operator!=(const Debuff& other) const;
    // add up the debuff
    Debuff operator+(const Debuff& other) const;
    void operator+=(const Debuff& other);
};

export class LevelFactory {
   public:
    // createLevel return a new level according to levelNum
    // and set the seed and srcfile to a level. returns nullptr if level invalid
    std::shared_ptr<Level> createLevel(int levelNum = 0, unsigned int seed = 1,
                                       std::string srcfile = "");

    // Levelup returns the one level harder of `level`.
    // maintains the seed and srcfile, BUT, the randomness
    // behavior is set to default of the level
    std::shared_ptr<Level> levelup(std::shared_ptr<Level> level);

    // Levelup returns the one level easier of `level`.
    // maintains the seed and srcfile, BUT, the randomness
    // behavior is set to default of the level
    std::shared_ptr<Level> leveldown(std::shared_ptr<Level> level);
};

class Level {
   protected:
    static std::shared_ptr<Block> charToBlock(char c, int levelNum = 0);

    int levelNum;
    Debuff effect;

    bool random;
    unsigned int seed;

    std::shared_ptr<std::istream> src;
    std::string srcfile;

   public:
    Level(int levelNum, bool random, std::string srcfile, unsigned int seed,
          Debuff debuff = Debuff{});
    virtual std::shared_ptr<Block> getNextBlock() = 0;

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
    std::shared_ptr<Block> getNextBlock() override;
};

export class Level1 : public Level {
   public:
    Level1(unsigned int seed = 1);
    std::shared_ptr<Block> getNextBlock() override;
};

export class Level2 : public Level {
    unsigned int seed;

   public:
    Level2(unsigned int seed = 1);
    std::shared_ptr<Block> getNextBlock() override;
};

export class Level3 : public Level {
    std::shared_ptr<Block> randomNextBlock();
    std::shared_ptr<Block> getNextBlockFromSrc();

   public:
    Level3(unsigned int seed = 1);
    std::shared_ptr<Block> getNextBlock() override;
};

export class Level4 : public Level {
    std::shared_ptr<Block> randomNextBlock();
    std::shared_ptr<Block> getNextBlockFromSrc();

   public:
    Level4(unsigned int seed = 1);
    std::shared_ptr<Block> getNextBlock() override;
};

export class Level5 : public Level {
   public:
    Level5(unsigned int seed = 1);
    std::shared_ptr<Block> getNextBlock() override;
};
