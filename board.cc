export module Board;

import Blocks;
import <vector>;

export class Board {
     Block* canvas[18][11] = {};
     Block* currentBlock = nullptr;
     Block* nextBlock = nullptr;
     std::vector<Block*> blocks = {};

     bool updateCurrentBlock(Block* newBlock); // true if currentBlock fits on canvas and false otherwise
     int clear(); // clears any rows thats filled and return number of rows cleared

     public:
     Board();
     ~Board();

     // Current block movements, modifies coords for current block
     void left();
     void right();
     void rotate(bool clockwise);
     void down();
     void drop();
     
     // Updates blocks vector to remove any pointer that points to blocks whose coordinates are all erased
     // returns vector of heap allocated Block objects, user needs to delete pointers after use
     vector<Block*> refreshBlocks();
};
