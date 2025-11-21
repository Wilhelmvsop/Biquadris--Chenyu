export module SBlock;

import Block;
import <vector>;
import <utility>;

export class SBlock : public Block {
     public:
     SBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

