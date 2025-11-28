import TestRunner;
import Blocks;
import <iostream>;
import <vector>;

IBlock iblock{0};
JBlock jblock{0};
LBlock lblock{0};
OBlock oblock{0};
SBlock sblock{0};
ZBlock zblock{0};
TBlock tblock{0};
BombBlockCat bomb{0};

void testBlocksRotation() {
    auto expectedi1 =
        std::vector<std::pair<int, int>>{{0, 0}, {1, 0}, {2, 0}, {3, 0}};
    auto expectedi2 =
        std::vector<std::pair<int, int>>{{3, 0}, {2, 0}, {1, 0}, {0, 0}};
    Tester::assert_true(expectedi1 == iblock.getRotatedCoords(true));
    Tester::assert_true(expectedi2 == iblock.getRotatedCoords(false));

    auto expectedj1 =
        std::vector<std::pair<int, int>>{{1, 0}, {2, 0}, {3, 0}, {1, 1}};
    auto expectedj2 =
        std::vector<std::pair<int, int>>{{3, 1}, {2, 1}, {1, 1}, {3, 0}};
    Tester::assert_true(expectedj1 == jblock.getRotatedCoords(true));
    Tester::assert_true(expectedj2 == jblock.getRotatedCoords(false));

    auto expectedl1 =
        std::vector<std::pair<int, int>>{{1, 0}, {2, 0}, {3, 0}, {3, 1}};
    auto expectedl2 =
        std::vector<std::pair<int, int>>{{3, 1}, {2, 1}, {1, 1}, {1, 0}};
    Tester::assert_true(expectedl1 == lblock.getRotatedCoords(true));
    Tester::assert_true(expectedl2 == lblock.getRotatedCoords(false));

    auto expectedo1 =
        std::vector<std::pair<int, int>>{{2, 0}, {3, 0}, {2, 1}, {3, 1}};
    auto expectedo2 =
        std::vector<std::pair<int, int>>{{3, 1}, {2, 1}, {3, 0}, {2, 0}};
    Tester::assert_true(expectedo1 == oblock.getRotatedCoords(true));
    Tester::assert_true(expectedo2 == oblock.getRotatedCoords(false));

    auto expecteds1 =
        std::vector<std::pair<int, int>>{{1, 0}, {2, 0}, {2, 1}, {3, 1}};
    auto expecteds2 =
        std::vector<std::pair<int, int>>{{3, 1}, {2, 1}, {2, 0}, {1, 0}};
    Tester::assert_true(expecteds1 == sblock.getRotatedCoords(true));
    Tester::assert_true(expecteds2 == sblock.getRotatedCoords(false));

    auto expectedz1 =
        std::vector<std::pair<int, int>>{{2, 0}, {3, 0}, {1, 1}, {2, 1}};
    auto expectedz2 =
        std::vector<std::pair<int, int>>{{2, 1}, {1, 1}, {3, 0}, {2, 0}};
    Tester::assert_true(expectedz1 == zblock.getRotatedCoords(true));
    Tester::assert_true(expectedz2 == zblock.getRotatedCoords(false));

    auto expectedt1 =
        std::vector<std::pair<int, int>>{{2, 0}, {1, 1}, {2, 1}, {3, 1}};
    auto expectedt2 =
        std::vector<std::pair<int, int>>{{2, 1}, {3, 0}, {2, 0}, {1, 0}};
    Tester::assert_true(expectedt1 == tblock.getRotatedCoords(true));
    Tester::assert_true(expectedt2 == tblock.getRotatedCoords(false));

    auto expectedbomb1 = std::vector<std::pair<int, int>>{{3, 5}};
    auto expectedbomb2 = std::vector<std::pair<int, int>>{{3, 5}};
    Tester::assert_true(expectedbomb1 == bomb.getRotatedCoords(true));
    Tester::assert_true(expectedbomb2 == bomb.getRotatedCoords(false));
}

void testBlocksGettersAndSetters() {
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

    Tester::assert_true(expected_i == iblock.getCoords());
    Tester::assert_true(expected_j == jblock.getCoords());
    Tester::assert_true(expected_l == lblock.getCoords());
    Tester::assert_true(expected_o == oblock.getCoords());
    Tester::assert_true(expected_s == sblock.getCoords());
    Tester::assert_true(expected_z == zblock.getCoords());
    Tester::assert_true(expected_t == tblock.getCoords());
    Tester::assert_true(expected_bomb == bomb.getCoords());

    Tester::assert_true('I' == iblock.getChar());
    Tester::assert_true('J' == jblock.getChar());
    Tester::assert_true('L' == lblock.getChar());
    Tester::assert_true('O' == oblock.getChar());
    Tester::assert_true('S' == sblock.getChar());
    Tester::assert_true('Z' == zblock.getChar());
    Tester::assert_true('T' == tblock.getChar());
    Tester::assert_true('*' == bomb.getChar());
    Tester::assert_true(0 == iblock.getMotherLevel());
}

void testBlocksOthers() {
    IBlock tempBlock{0};
    Tester::assert_true(false == tempBlock.isCleared());
    tempBlock.deleteCoords({3, 0});

    auto expected = std::vector<std::pair<int, int>>{{3, 1}, {3, 2}, {3, 3}};
    Tester::assert_true(expected == tempBlock.getCoords());

    tempBlock.deleteCoords({3, 1});
    tempBlock.deleteCoords({3, 2});
    tempBlock.deleteCoords({3, 3});
    Tester::assert_true(true == tempBlock.isCleared());
}

namespace {
Tester::TestRegistrar r1("Blocks_Rotation", testBlocksRotation);
Tester::TestRegistrar r2("Blocks_Getters_and_Setters",
                         testBlocksGettersAndSetters);
Tester::TestRegistrar r3("Blocks_Others", testBlocksOthers);
}  // namespace
