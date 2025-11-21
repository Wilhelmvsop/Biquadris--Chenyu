export module LBlock;

import Block;
import <vector>;
import <utility>;

export class LBlock : public Block {
     public:
     LBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

