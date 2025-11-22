import Blocks;
import <iostream>;
#include "test_runner.h"

IBlock iblock{0};
JBlock jblock{0};
LBlock lblock{0};
OBlock oblock{0};
SBlock sblock{0};
ZBlock zblock{0};
TBlock tblock{0};
BombBlockCat bomb{0};

TEST_CASE(Rotation) {
     auto expected1 = std::vector<std::pair<int, int>>{{-1, 0}, {0, 0}, {-2, 1}, {-1, 1}};
     auto expected2 = std::vector<std::pair<int, int>>{{-1, 1}, {-2, 1}, {0, 0}, {-1, 0}};
     REQUIRE(expected1 == zblock.getRotatedCoords(true));
     REQUIRE(expected2 == zblock.getRotatedCoords(false));
}

TEST_CASE(Getters_and_Setters) {
     auto expected_i = std::vector<std::pair<int, int>>{{0, 0}, {0, 1}, {0, 2}, {0, 3}};
     auto expected_j = std::vector<std::pair<int, int>>{{0, 0}, {0, 1}, {0, 2}, {-1, 0}};
     auto expected_l = std::vector<std::pair<int, int>>{{0, 0}, {0, 1}, {0, 2}, {-1, 2}};
     auto expected_o = std::vector<std::pair<int, int>>{{0, 0}, {0, 1}, {-1, 0}, {-1, 1}};
     auto expected_s = std::vector<std::pair<int, int>>{{0, 0}, {0, 1}, {-1, 1}, {-1, 2}};
     auto expected_z = std::vector<std::pair<int, int>>{{0, 1}, {0, 2}, {-1, 0}, {-1, 1}};
     auto expected_t = std::vector<std::pair<int, int>>{{0, 1}, {-1, 0}, {-1, 1}, {-1, 2}};
     auto expected_bomb = std::vector<std::pair<int, int>>{{0, 0}};
     
     REQUIRE(expected_i == iblock.getCoords());
     REQUIRE(expected_j == jblock.getCoords());
     REQUIRE(expected_l == lblock.getCoords());
     REQUIRE(expected_o == oblock.getCoords());
     REQUIRE(expected_s == sblock.getCoords());
     REQUIRE(expected_z == zblock.getCoords());
     REQUIRE(expected_t == tblock.getCoords());
     REQUIRE(expected_bomb == bomb.getCoords());
     
     REQUIRE('I' == iblock.getChar());
     REQUIRE('J' == jblock.getChar());
     REQUIRE('L' == lblock.getChar());
     REQUIRE('O' == oblock.getChar());
     REQUIRE('S' == sblock.getChar());
     REQUIRE('Z' == zblock.getChar());
     REQUIRE('T' == tblock.getChar());
     REQUIRE('#' == bomb.getChar());
     REQUIRE(0 == iblock.getMotherLevel());
}

TEST_CASE(Others) {
     IBlock tempBlock{0};
     REQUIRE(false == tempBlock.isCleared());
     tempBlock.deleteCoords({0, 0});
     
     auto expected = std::vector<std::pair<int, int>>{{0, 1}, {0, 2}, {0, 3}};
     REQUIRE(expected == tempBlock.getCoords());
     
     tempBlock.deleteCoords({0, 1});
     tempBlock.deleteCoords({0, 2});
     tempBlock.deleteCoords({0, 3});
     REQUIRE(true == tempBlock.isCleared());
}
