export module Blocks;
import <vector>;
import <utility>;
import <memory>;

export class Block;

export class BlockFactory {
   public:
    // check if char c is valid input for producing a block
    bool isValidChar(char c) const;
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
    Block(std::vector<std::pair<int, int>> coords, char ch, int level) noexcept;
    bool isCleared() const noexcept;
    void deleteCoords(std::pair<int, int> target);
    // clone returns new heap allocated pointer to exact same block of it
    std::shared_ptr<Block> clone() const;
    std::vector<std::pair<int, int>> getRotatedCoords(bool clockwise) const noexcept;

    // getters and setters
    std::vector<std::pair<int, int>> getCoords() const noexcept;
    char getChar() const noexcept;
    int getMotherLevel() const noexcept;
    void setCoords(std::vector<std::pair<int, int>> coords) noexcept;

    virtual ~Block() noexcept = 0;
};

export class IBlock : public Block {
   public:
    IBlock(int level) noexcept;
};

export class JBlock : public Block {
   public:
    JBlock(int level) noexcept;
};

export class LBlock : public Block {
   public:
    LBlock(int level) noexcept;
};

export class OBlock : public Block {
   public:
    OBlock(int level) noexcept;
};

export class SBlock : public Block {
   public:
    SBlock(int level) noexcept;
};

export class TBlock : public Block {
   public:
    TBlock(int level) noexcept;
};

export class ZBlock : public Block {
   public:
    ZBlock(int level) noexcept;
};

export class BombBlockCat : public Block {
   public:
    BombBlockCat(int level) noexcept;
};
