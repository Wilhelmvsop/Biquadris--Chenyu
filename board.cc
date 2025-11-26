export module Board;

import Blocks;
import <vector>;
import <tuple>;

export class Board {
    Block* canvas[18][11] = {};
    Block* currentBlock = nullptr;
    Block* nextBlock = nullptr;
    std::vector<Block*> blocks = {};

    // true if currentBlock fits on canvas and false otherwise
    bool updateCurrentBlock();
    // clears any rows thats filled and return number of rows cleared
    int clear();
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
    // Drops the current block and updates it with the next block, sets next
    // block to nullptr
    // First element: whether the next block fits or not (ie check if player
    // loses)
    // Second element: number of rows cleared
    // Third element: vector of
    // blocks cleared (user MUST delete all pointers in this vector after
    // function call)
    std::tuple<bool, int, std::vector<Block*>> drop();

    // Setters and getters:

    // setCurrentBlock deletes current block (if exists)
    // and set newBlock as new current block
    void setCurrentBlock(Block* newBlock);
    void setNextBlock(Block* newBlock);
    using Canvas = Block* [18][11];
    Canvas& getCanvas();
    Block* getNextBlock() const;
    Block* getCurrentBlock() const;
};
