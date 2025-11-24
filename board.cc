export module Board;

import Blocks;
import <vector>;
import <tuple>;

export class Board {
    Block* canvas[18][11] = {};
    Block* currentBlock = nullptr;
    Block* nextBlock = nullptr;
    std::vector<Block*> blocks = {};

    bool updateCurrentBlock();  // true if currentBlock fits on canvas and false
                                // otherwise
    int
    clear();  // clears any rows thats filled and return number of rows cleared
    // Updates blocks vector to remove any pointer that points to blocks whose
    // coordinates are all erased returns vector of heap allocated Block
    // objects, user needs to delete pointers after use
    std::vector<Block*> refreshBlocks();

   public:
    Board();
    ~Board();

    // Current block movements, potentially modifies coords for current block
    // and canvas
    void left();
    void right();
    void rotate(bool clockwise);
    void down();
    // drops the current block and updates it with the next block, sets next
    // block to nullptr first element: whether the next block fits or not (ie
    // check if player loses) second element: number of rows cleared third
    // element: vector of blocks cleared
    std::tuple<bool, int, std::vector<Block*>> drop();

    // Setters and getters
    void setCurrentBlock(
        Block* newBlock);  // only called ONCE in the beginning, modifies canvas
    void setNextBlock(Block* newBlock);
    using Canvas = Block* [18][11];
    Canvas& getCanvas();
    Block* getNextBlock() const;  // getter for nextBlock
};
