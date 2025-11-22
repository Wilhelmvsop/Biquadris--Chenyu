module Blocks;

import <vector>;
import <utility>;
import <algorithm>;
import <stdexcept>;
import <string>;

// BLOCK IMPLEMENTATIONS
Block::Block(std::vector<std::pair<int, int>> coords, char ch, int level) : 
     coords{coords}, ch{ch}, motherLevel{level} {}
Block::~Block() {}

bool Block::isCleared() const {
     return coords.empty();
}

// Assume target coords are in the vector, throws exception if it is not
void Block::deleteCoords(std::pair<int, int> target) {
     auto it = coords.begin();
     for (; it != coords.end(); ++it) {
          if (*it == target) {
               coords.erase(it);
               return;
          }
     }
     if (it == coords.end()) {
          throw std::runtime_error(
               "Target coordinate not found: (" 
               + std::to_string(target.first) 
               + ", " + std::to_string(target.second) + ")."
          );
     }
}
     
const std::vector<std::pair<int, int>>& Block::getCoords() const {
     return coords;
}

char Block::getChar() const {
     return ch;
}

int Block::getMotherLevel() const {
     return motherLevel;
}

void Block::setCoords(std::vector<std::pair<int, int>> coords) {
     this->coords = coords;
}

const std::vector<std::pair<int, int>>& Block::getRotatedCoords(int dir) const {
     int min_y = coords[0].first;
     int max_y = coords[0].first;
     int min_x = coords[0].second;
     int max_x = coords[0].second;

     for (auto coord : coords) {
          min_y = std::min(min_y, coord.first);
          max_y = std::max(max_y, coord.first);
          min_x = std::min(min_x, coord.second);
          max_x = std::max(max_x, coord.second);
     }

     int width = max_x - min_x + 1;
     int height = max_y - min_y + 1;
     std::vector<std::pair<int, int>> rotatedCoords;
     std::pair<int, int> originalLL = {max_y, min_x};

     for (const auto& coord : coords) {
          int local_y = coord.first - min_y;
          int local_x = coord.second - min_x;
          int new_x, new_y;

          if (dir == 0) { // Clockwise rotation
               new_x = width - 1 - local_y;
               new_y = local_x;
          }
          else { // Counter-clockwise rotation
               new_x = local_y;
               new_y = height - 1 - local_x;
          }

          rotatedCoords.emplace_back(min_y + new_y, min_x + new_x);
     }

     std::pair<int, int> newLL = rotatedCoords[0];
     for (auto coord : rotatedCoords) {
          newLL.first = std::max(newLL.first, coord.first);
          newLL.second = std::min(newLL.second, coord.second);
     }

     int yDiff = newLL.first - originalLL.first;
     int xDiff = newLL.second - originalLL.second;

     for (auto& coord : rotatedCoords) {
          coord.first -= yDiff;
          coord.second -= xDiff;
     }

     return rotatedCoords;
}

// SUBCLASS CONSTRUCTORS
IBlock::IBlock(int level) : Block{{{0, 0}, {0, 1}, {0, 2}, {0, 3}}, 'I', level} {}
JBlock::JBlock(int level) : Block{{{0, 0}, {0, 1}, {0, 2}, {-1, 0}}, 'J', level} {}
LBlock::LBlock(int level) : Block{{{0, 0}, {0, 1}, {0, 2}, {-1, 2}}, 'L', level} {}
OBlock::OBlock(int level) : Block{{{0, 0}, {0, 1}, {-1, 0}, {-1, 1}}, 'O', level} {}
SBlock::SBlock(int level) : Block{{{0, 0}, {0, 1}, {-1, 1}, {-1, 2}}, 'S', level} {}
ZBlock::ZBlock(int level) : Block{{{0, 1}, {0, 2}, {-1, 0}, {-1, 1}}, 'Z', level} {}
TBlock::TBlock(int level) : Block{{{0, 1}, {-1, 0}, {-1, 1}, {-1, 2}}, 'T', level} {}
BombBlockCat::BombBlockCat(int level) : Block{{{0, 0}}, '#', level} {}

