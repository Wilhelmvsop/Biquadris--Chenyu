export module Board;

import Blocks;
import <vector>;
import <tuple>;
import <memory>;

export class Board {
     std::shared_ptr<Block> canvas[18][11];
     std::shared_ptr<Block> currentBlock;
     std::shared_ptr<Block> nextBlock;
     std::vector<std::shared_ptr<Block>> blocks;

     bool updateCurrentBlock() noexcept; // true if currentBlock fits on canvas and false otherwise
     int clear() noexcept; // clears any rows thats filled and return number of rows cleared
     // Updates blocks vector to remove any pointer that points to blocks whose coordinates are all erased
     // returns vector of heap allocated Block objects, user needs to delete pointers after use
     std::vector<std::shared_ptr<Block>> refreshBlocks() noexcept;

     public:
     Board() noexcept;
     ~Board() noexcept;

     // Current block movements, potentially modifies coords for current block and canvas
     void left() noexcept;
     void right() noexcept;
     void rotate(bool clockwise) noexcept;
     void down() noexcept;
     // drops the current block and updates it with the next block, sets next block to nullptr
     // first element: whether the next block fits or not (ie check if player loses)
     // second element: number of rows cleared
     // third element: vector of blocks cleared (user don't need to delete because its unique_ptr)
     std::tuple<bool, int, std::vector<std::shared_ptr<Block>>> drop() noexcept;

     // Setters and getters
     void setCurrentBlock(std::shared_ptr<Block> newBlock) noexcept; // only called ONCE in the beginning, modifies canvas
     void setNextBlock(std::shared_ptr<Block> newBlock) noexcept;
     using Canvas = std::shared_ptr<Block>[18][11];
     Canvas& getCanvas() noexcept;
};
