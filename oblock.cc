export module OBlock;

import Block;
import <vector>;
import <utility>;

export class OBlock : public Block {
     public:
     OBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

