export module level;

import block;

import <utility>;
import <iostream>;
import <string>;

export class Level;

export struct Debuff {
    int heaviness;
    bool blind;
    Block* force;
    std::pair<Block*, int> insert;
};

export class LevelFactory {
   public:
    Level* createLevel(int levelNum = 0, unsigned int seed = 1,
                       std::string srcfile = "");
    Level* levelup(Level* level);
    Level* leveldown(Level* level);
};

class Level {
   protected:
    static Block* charToBlock(char c);

    int levelNum;
    Debuff effect;

    bool random;
    unsigned int seed;

    std::istream* src;
    std::string srcfile;

    void setRandom(bool random);
    void setSrcfile(std::string srcfile);
    void setSeed(unsigned int seed);

   public:
    Level(int levelNum, bool random, std::string srcfile, unsigned int seed,
          Debuff debuff = Debuff{});
    virtual Block* getNextBlock() = 0;

    int getLevelNum();
    Debuff getDebuff();
    bool getRandom();
    unsigned int getSeed();
    std::string getSrcfile();

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
