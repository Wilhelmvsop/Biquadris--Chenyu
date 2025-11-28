module Blocks;

import <vector>;
import <utility>;
import <algorithm>;
import <stdexcept>;
import <string>;
import <stdexcept>;

// BLOCK IMPLEMENTATIONS
Block::Block(std::vector<std::pair<int, int>> coords, char ch, int level, int duration) noexcept
    : coords{coords}, ch{ch}, motherLevel{level}, duration{duration} {}
Block::~Block() noexcept {}

bool Block::isCleared() const noexcept { return coords.empty(); }

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
        throw std::runtime_error("Target coordinate not found: (" +
                                 std::to_string(target.first) + ", " +
                                 std::to_string(target.second) + ").");
    }
}

std::vector<std::pair<int, int>> Block::getCoords() const noexcept {
    return coords;
}

char Block::getChar() const noexcept { return ch; }

int Block::getMotherLevel() const noexcept { return motherLevel; }

int Block::getDuration() const noexcept { return duration; }

void Block::decreaseDuration() {
     if (duration > 0) {
          duration -= 1;
     }
     else {
          throw std::runtime_error("Cannot decrease duration by 1 as duration is <= 0.");
     }
}

void Block::setCoords(std::vector<std::pair<int, int>> coords) noexcept {
    this->coords = coords;
}

std::vector<std::pair<int, int>> Block::getRotatedCoords(bool clockwise) const noexcept {
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

        if (clockwise) {  // Clockwise rotation
            new_x = width - 1 - local_y;
            new_y = local_x;
        } else {  // Counter-clockwise rotation
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

std::shared_ptr<Block> Block::clone() const {
    BlockFactory bf{};
    return bf.createBlock(ch, motherLevel, duration);
}

// SUBCLASS CONSTRUCTORS
IBlock::IBlock(int level, int duration) noexcept
    : Block{{{3, 0}, {3, 1}, {3, 2}, {3, 3}}, 'I', level, duration} {}
JBlock::JBlock(int level, int duration) noexcept
    : Block{{{3, 0}, {3, 1}, {3, 2}, {2, 0}}, 'J', level, duration} {}
LBlock::LBlock(int level, int duration) noexcept
    : Block{{{3, 0}, {3, 1}, {3, 2}, {2, 2}}, 'L', level, duration} {}
OBlock::OBlock(int level, int duration) noexcept
    : Block{{{3, 0}, {3, 1}, {2, 0}, {2, 1}}, 'O', level, duration} {}
SBlock::SBlock(int level, int duration) noexcept
    : Block{{{3, 0}, {3, 1}, {2, 1}, {2, 2}}, 'S', level, duration} {}
ZBlock::ZBlock(int level, int duration) noexcept
    : Block{{{3, 1}, {3, 2}, {2, 0}, {2, 1}}, 'Z', level, duration} {}
TBlock::TBlock(int level, int duration) noexcept
    : Block{{{3, 1}, {2, 0}, {2, 1}, {2, 2}}, 'T', level, duration} {}
BombBlockCat::BombBlockCat(int level, int duration) noexcept
    : Block{{{3, 5}}, '*', level, duration} {}

std::shared_ptr<Block> BlockFactory::createBlock(char c, int level, int duration) const {
    switch (c) {
        case 'I':
            return std::make_shared<IBlock>(level, duration);
        case 'J':
            return std::make_shared<JBlock>(level, duration);
        case 'L':
            return std::make_shared<LBlock>(level, duration);
        case 'O':
            return std::make_shared<OBlock>(level, duration);
        case 'S':
            return std::make_shared<SBlock>(level, duration);
        case 'Z':
            return std::make_shared<ZBlock>(level, duration);
        case 'T':
            return std::make_shared<TBlock>(level, duration);
        case '*':
            return std::make_shared<BombBlockCat>(level, duration);
        default:
            return nullptr;
    }
}

bool BlockFactory::isValidChar(char c) const {
    const std::string validChars = "IJLOSZT*";
    return validChars.find(c) != std::string::npos;
}

