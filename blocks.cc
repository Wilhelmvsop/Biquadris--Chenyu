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
     bool isCleared();
     void deleteCoords(std::pair<int, int>);
     
     // getters and setters
     std::vector<std::pair<int, int>> getCoords();
     char getChar();
     int getMotherLevel();
     void setCoords(std::vector<std::pair<int, int>> coords);

     virtual std::vector<std::pair<int, int>> getRotatedCoords() = 0;
     virtual ~Block() = default;
};

export class IBlock : public Block {
     public:
     IBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

export class JBlock : public Block {
     public:
     JBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

export class LBlock : public Block {
     public:
     LBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

export class OBlock : public Block {
     public:
     OBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

export class SBlock : public Block {
     public:
     SBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

export class TBlock : public Block {
     public:
     TBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

export class ZBlock : public Block {
     public:
     ZBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

export class BombBlockCat : public Block {
     public:
     BombBlockCat(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

