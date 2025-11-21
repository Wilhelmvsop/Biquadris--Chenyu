export module ZBlock;

import Block;
import <vector>;
import <utility>;

export class ZBlock : public Block {
     public:
     IBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

