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
    std::shared_ptr<Block> createBlock(char c, int level = 0, int turnPlaced = -1) const;
};

class Block {
   protected:
    std::vector<std::pair<int, int>> coords;
    char ch;
    int motherLevel;
    int duration;

   public:
    Block(std::vector<std::pair<int, int>> coords, char ch, int level, int duration = -1) noexcept;
    // checks if a block is cleared (ie: the coordinates is empty)
    bool isCleared() const noexcept;
    // deletes a coordinate from the block, throws exception if the coordinate is not valid (ie: not in block)
    void deleteCoords(std::pair<int, int> target);
    // clone returns new heap allocated pointer to exact same block of it
    std::shared_ptr<Block> clone() const;
    // returns the rotated coordinates of the block
    std::vector<std::pair<int, int>> getRotatedCoords(bool clockwise) const noexcept;

    // getters and setters
    std::vector<std::pair<int, int>> getCoords() const noexcept;
    char getChar() const noexcept;
    int getMotherLevel() const noexcept;
    void setCoords(std::vector<std::pair<int, int>> coords) noexcept;
    int getDuration() const noexcept;
    void decreaseDuration();

    virtual ~Block() noexcept = 0;
};

export class IBlock : public Block {
   public:
    IBlock(int level, int duration = -1) noexcept;
};

export class JBlock : public Block {
   public:
    JBlock(int level, int duration = -1) noexcept;
};

export class LBlock : public Block {
   public:
    LBlock(int level, int duration = -1) noexcept;
};

export class OBlock : public Block {
   public:
    OBlock(int level, int duration = -1) noexcept;
};

export class SBlock : public Block {
   public:
    SBlock(int level, int duration = -1) noexcept;
};

export class TBlock : public Block {
   public:
    TBlock(int level, int duration = -1) noexcept;
};

export class ZBlock : public Block {
   public:
    ZBlock(int level, int duration = -1) noexcept;
};

export class BombBlockCat : public Block {
   public:
    BombBlockCat(int level, int duration = -1) noexcept;
};
