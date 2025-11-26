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

TEST_CASE(Blocks_Rotation) {
    auto expectedi1 =
        std::vector<std::pair<int, int>>{{0, 0}, {1, 0}, {2, 0}, {3, 0}};
    auto expectedi2 =
        std::vector<std::pair<int, int>>{{3, 0}, {2, 0}, {1, 0}, {0, 0}};
    REQUIRE(expectedi1 == iblock.getRotatedCoords(true));
    REQUIRE(expectedi2 == iblock.getRotatedCoords(false));

    auto expectedj1 =
        std::vector<std::pair<int, int>>{{1, 0}, {2, 0}, {3, 0}, {1, 1}};
    auto expectedj2 =
        std::vector<std::pair<int, int>>{{3, 1}, {2, 1}, {1, 1}, {3, 0}};
    REQUIRE(expectedj1 == jblock.getRotatedCoords(true));
    REQUIRE(expectedj2 == jblock.getRotatedCoords(false));

    auto expectedl1 =
        std::vector<std::pair<int, int>>{{1, 0}, {2, 0}, {3, 0}, {3, 1}};
    auto expectedl2 =
        std::vector<std::pair<int, int>>{{3, 1}, {2, 1}, {1, 1}, {1, 0}};
    REQUIRE(expectedl1 == lblock.getRotatedCoords(true));
    REQUIRE(expectedl2 == lblock.getRotatedCoords(false));

    auto expectedo1 =
        std::vector<std::pair<int, int>>{{2, 0}, {3, 0}, {2, 1}, {3, 1}};
    auto expectedo2 =
        std::vector<std::pair<int, int>>{{3, 1}, {2, 1}, {3, 0}, {2, 0}};
    REQUIRE(expectedo1 == oblock.getRotatedCoords(true));
    REQUIRE(expectedo2 == oblock.getRotatedCoords(false));

    auto expecteds1 =
        std::vector<std::pair<int, int>>{{1, 0}, {2, 0}, {2, 1}, {3, 1}};
    auto expecteds2 =
        std::vector<std::pair<int, int>>{{3, 1}, {2, 1}, {2, 0}, {1, 0}};
    REQUIRE(expecteds1 == sblock.getRotatedCoords(true));
    REQUIRE(expecteds2 == sblock.getRotatedCoords(false));

    auto expectedz1 =
        std::vector<std::pair<int, int>>{{2, 0}, {3, 0}, {1, 1}, {2, 1}};
    auto expectedz2 =
        std::vector<std::pair<int, int>>{{2, 1}, {1, 1}, {3, 0}, {2, 0}};
    REQUIRE(expectedz1 == zblock.getRotatedCoords(true));
    REQUIRE(expectedz2 == zblock.getRotatedCoords(false));

    auto expectedt1 =
        std::vector<std::pair<int, int>>{{2, 0}, {1, 1}, {2, 1}, {3, 1}};
    auto expectedt2 =
        std::vector<std::pair<int, int>>{{2, 1}, {3, 0}, {2, 0}, {1, 0}};
    REQUIRE(expectedt1 == tblock.getRotatedCoords(true));
    REQUIRE(expectedt2 == tblock.getRotatedCoords(false));

    auto expectedbomb1 = std::vector<std::pair<int, int>>{{3, 5}};
    auto expectedbomb2 = std::vector<std::pair<int, int>>{{3, 5}};
    REQUIRE(expectedbomb1 == bomb.getRotatedCoords(true));
    REQUIRE(expectedbomb2 == bomb.getRotatedCoords(false));
}

TEST_CASE(Blocks_Getters_and_Setters) {
    auto expected_i =
        std::vector<std::pair<int, int>>{{3, 0}, {3, 1}, {3, 2}, {3, 3}};
    auto expected_j =
        std::vector<std::pair<int, int>>{{3, 0}, {3, 1}, {3, 2}, {2, 0}};
    auto expected_l =
        std::vector<std::pair<int, int>>{{3, 0}, {3, 1}, {3, 2}, {2, 2}};
    auto expected_o =
        std::vector<std::pair<int, int>>{{3, 0}, {3, 1}, {2, 0}, {2, 1}};
    auto expected_s =
        std::vector<std::pair<int, int>>{{3, 0}, {3, 1}, {2, 1}, {2, 2}};
    auto expected_z =
        std::vector<std::pair<int, int>>{{3, 1}, {3, 2}, {2, 0}, {2, 1}};
    auto expected_t =
        std::vector<std::pair<int, int>>{{3, 1}, {2, 0}, {2, 1}, {2, 2}};
    auto expected_bomb = std::vector<std::pair<int, int>>{{3, 5}};

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
    REQUIRE('*' == bomb.getChar());
    REQUIRE(0 == iblock.getMotherLevel());
}

TEST_CASE(Blocks_Others) {
    IBlock tempBlock{0};
    REQUIRE(false == tempBlock.isCleared());
    tempBlock.deleteCoords({3, 0});

    auto expected = std::vector<std::pair<int, int>>{{3, 1}, {3, 2}, {3, 3}};
    REQUIRE(expected == tempBlock.getCoords());

    tempBlock.deleteCoords({3, 1});
    tempBlock.deleteCoords({3, 2});
    tempBlock.deleteCoords({3, 3});
    REQUIRE(true == tempBlock.isCleared());
}
