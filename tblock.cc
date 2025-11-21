export module TBlock;

import Block;
import <vector>;
import <utility>;

export class TBlock : public Block {
     public:
     IBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

