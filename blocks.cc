export module Blocks;
import <vector>;
import <utility>;

export class Block;

export class BlockFactory {
   public:
    // create block specify by character c
    // needs level for block creation
    // return nullptr if c invalid
    Block* createBlock(char c, int level = 0) const;
};

class Block {
   protected:
    std::vector<std::pair<int, int>> coords;
    char ch;
    int motherLevel;

   public:
    Block(std::vector<std::pair<int, int>> coords, char ch, int level);
    bool isCleared() const;
    void deleteCoords(std::pair<int, int> target);
    std::vector<std::pair<int, int>> getRotatedCoords(bool clockwise) const;
    // clone returns new heap allocated pointer to exact same block of it
    Block* clone() const;

    // getters and setters
    std::vector<std::pair<int, int>> getCoords() const;
    char getChar() const;
    int getMotherLevel() const;
    void setCoords(std::vector<std::pair<int, int>> coords);

    virtual ~Block() = 0;
};

export class IBlock : public Block {
   public:
    IBlock(int level);
};

export class JBlock : public Block {
   public:
    JBlock(int level);
};

export class LBlock : public Block {
   public:
    LBlock(int level);
};

export class OBlock : public Block {
   public:
    OBlock(int level);
};

export class SBlock : public Block {
   public:
    SBlock(int level);
};

export class TBlock : public Block {
   public:
    TBlock(int level);
};

export class ZBlock : public Block {
   public:
    ZBlock(int level);
};

export class BombBlockCat : public Block {
   public:
    BombBlockCat(int level);
};
