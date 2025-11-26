module Board;

import Blocks;
import <iostream>;
import <vector>;
import <utility>;
import <tuple>;
import <memory>;

Board::Board() noexcept {}
Board::~Board() noexcept {}

bool Board::updateCurrentBlock() noexcept {
     currentBlock = std::move(nextBlock);

     bool fits = true;
     std::vector<std::pair<int, int>> coords = currentBlock->getCoords();
     for (auto coord : coords) {
          if (canvas[coord.first][coord.second] != nullptr) {
               fits = false;
          }
          canvas[coord.first][coord.second] = currentBlock;
     }
     return fits;
}

int Board::clear() noexcept {
     int clearedRows = 0;
     std::vector<int> rowsToClear;

    // get the row numbers thats filled
    for (int row = 0; row < 18; row++) {
        bool rowFilled = true;
        for (int col = 0; col < 11; col++) {
            if (canvas[row][col] == nullptr) {
                rowFilled = false;
                break;
            }
        }

        if (rowFilled) {
            rowsToClear.emplace_back(row);
            clearedRows += 1;
        }
    }

    // delete the rows by deleting the coords from the blocks and setting its
    // pos to nullptr
    for (auto row : rowsToClear) {
        for (int col = 0; col < 11; col++) {
            canvas[row][col]->deleteCoords({row, col});
            canvas[row][col] = nullptr;
        }

        for (int i = row - 1; i >= 0; i--) {
            for (int j = 0; j < 11; j++) {
                if (canvas[i][j] != nullptr) {
                    std::vector<std::pair<int, int>> coords =
                        canvas[i][j]->getCoords();
                    for (auto& coord : coords) {
                        if (coord == std::make_pair(i, j)) {
                            coord = std::make_pair(i + 1, j);
                            break;
                        }
                    }
                    canvas[i][j]->setCoords(coords);
                    canvas[i + 1][j] = canvas[i][j];
                    canvas[i][j] = nullptr;
                }
            }
        }
    }

    return clearedRows;
}

std::vector<std::shared_ptr<Block>> Board::refreshBlocks() noexcept {
     std::vector<std::shared_ptr<Block>> clearedBlocks;

    auto it = blocks.begin();
    while (it != blocks.end()) {
        if ((*it)->isCleared()) {
            clearedBlocks.emplace_back(*it);
            it = blocks.erase(it);
        } else {
            ++it;
        }
    }

    return clearedBlocks;
}

void Board::left() noexcept {
     std::vector<std::pair<int, int>> coords = currentBlock->getCoords();

    for (auto coord : coords) {
        canvas[coord.first][coord.second] = nullptr;
    }

    bool canMove = true;
    for (auto coord : coords) {
        int newCol = coord.second - 1;
        if (newCol < 0 || canvas[coord.first][newCol] != nullptr) {
            canMove = false;
            break;
        }
    }

    if (!canMove) {
        for (auto coord : coords) {
            canvas[coord.first][coord.second] = currentBlock;
        }
        // std::cout << "Unable to move current block to the left by 1" <<
        // std::endl;
    } else {
        std::vector<std::pair<int, int>> newCoords;
        for (auto coord : coords) {
            newCoords.emplace_back(
                std::make_pair(coord.first, coord.second - 1));
            canvas[coord.first][coord.second - 1] = currentBlock;
        }
        currentBlock->setCoords(newCoords);
        // std::cout << "Moved current block to the left by 1" << std::endl;
    }
}

void Board::right() noexcept {
     std::vector<std::pair<int, int>> coords = currentBlock->getCoords();

    for (auto coord : coords) {
        canvas[coord.first][coord.second] = nullptr;
    }

    bool canMove = true;
    for (auto coord : coords) {
        int newCol = coord.second + 1;
        if (newCol > 10 || canvas[coord.first][newCol] != nullptr) {
            canMove = false;
            break;
        }
    }

    if (!canMove) {
        for (auto coord : coords) {
            canvas[coord.first][coord.second] = currentBlock;
        }
        // std::cout << "Unable to move current block to the right by 1" <<
        // std::endl;
    } else {
        std::vector<std::pair<int, int>> newCoords;
        for (auto coord : coords) {
            newCoords.emplace_back(
                std::make_pair(coord.first, coord.second + 1));
            canvas[coord.first][coord.second + 1] = currentBlock;
        }
        currentBlock->setCoords(newCoords);
        // std::cout << "Moved current block to the right by 1" << std::endl;
    }
}

void Board::rotate(bool clockwise) noexcept {
     std::vector<std::pair<int, int>> coords = currentBlock->getCoords();

    for (auto coord : coords) {
        canvas[coord.first][coord.second] = nullptr;
    }

    bool canMove = true;
    std::vector<std::pair<int, int>> rotatedCoords =
        currentBlock->getRotatedCoords(clockwise);
    for (auto coord : rotatedCoords) {
        if (coord.second > 10 || canvas[coord.first][coord.second] != nullptr) {
            canMove = false;
            break;
        }
    }

    if (!canMove) {
        for (auto coord : coords) {
            canvas[coord.first][coord.second] = currentBlock;
        }
        // std::cout << "Unable to rotate current block" << std::endl;
    } else {
        for (auto coord : rotatedCoords) {
            canvas[coord.first][coord.second] = currentBlock;
        }
        currentBlock->setCoords(rotatedCoords);
        // std::cout << "Rotated current block" << std::endl;
    }
}

void Board::down() noexcept {
     std::vector<std::pair<int, int>> coords = currentBlock->getCoords();

    for (auto coord : coords) {
        canvas[coord.first][coord.second] = nullptr;
    }

    bool canMove = true;
    for (auto coord : coords) {
        int newRow = coord.first + 1;
        if (newRow > 17 || canvas[newRow][coord.second] != nullptr) {
            canMove = false;
            break;
        }
    }

    if (!canMove) {
        for (auto coord : coords) {
            canvas[coord.first][coord.second] = currentBlock;
        }
        // std::cout << "Unable to move current block down by 1" << std::endl;
    } else {
        std::vector<std::pair<int, int>> newCoords;
        for (auto coord : coords) {
            newCoords.emplace_back(
                std::make_pair(coord.first + 1, coord.second));
            canvas[coord.first + 1][coord.second] = currentBlock;
        }
        currentBlock->setCoords(newCoords);
        // std::cout << "Moved current block down by 1" << std::endl;
    }
}

std::tuple<bool, int, std::vector<std::shared_ptr<Block>>> Board::drop() noexcept {
     while (true) {
          std::vector<std::pair<int, int>> currentCoords = currentBlock->getCoords();
          down();

        if (currentBlock->getCoords() == currentCoords) {
            break;
        }
    }

     blocks.emplace_back(std::move(currentBlock));

     int rowsCleared = clear();
     std::vector<std::shared_ptr<Block>> clearedBlocks = refreshBlocks();
     bool continueGame = updateCurrentBlock();
     return std::make_tuple(continueGame, rowsCleared, clearedBlocks);
}

void Board::setCurrentBlock(std::shared_ptr<Block> newBlock) noexcept {
     currentBlock = newBlock;
     
     std::vector<std::pair<int, int>> coords = currentBlock->getCoords();
     for (auto coord : coords) {
          canvas[coord.first][coord.second] = currentBlock;
     }
}
void Board::setNextBlock(std::shared_ptr<Block> newBlock) noexcept {
     nextBlock = newBlock;
}

using Canvas = std::shared_ptr<Block>[18][11];
Canvas& Board::getCanvas() noexcept {
     return canvas;
}

std::shared_ptr<Block> Board::getNextBlock() const { return nextBlock; }
std::shared_ptr<Block> Board::getCurrentBlock() const { return currentBlock; }
