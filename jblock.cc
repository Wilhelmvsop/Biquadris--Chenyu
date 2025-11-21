export module JBlock;

import Block;
import <vector>;
import <utility>;

export class JBlock : public Block {
     public:
     JBlock(std::vector<std::pair<int, int>> coords, char ch, int level) : Block{coords, ch, level} {}
     std::vector<std::pair<int, int>> getRotatedCoords() override;
};

