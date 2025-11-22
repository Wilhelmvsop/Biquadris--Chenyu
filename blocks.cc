export module Blocks;
import <vector>;
import <utility>;

export class Block {
     protected:
     std::vector<std::pair<int, int>> coords;
     char ch;
     int motherLevel;

     public:
     Block(std::vector<std::pair<int, int>> coords, char ch, int level);
     bool isCleared() const;
     void deleteCoords(std::pair<int, int> target);
     std::vector<std::pair<int, int>> getRotatedCoords(bool clockwise) const;

     // getters and setters
     const std::vector<std::pair<int, int>>& getCoords() const;
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

