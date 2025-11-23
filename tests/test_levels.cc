#include <fstream>

import Levels;
import Blocks;

import <iostream>;
import <fstream>;
import <filesystem>;
import <map>;
import <string>;

#include "test_runner.h"

TEST_CASE(LevelFactory_LevelCreation) {
    LevelFactory factory;

    for (unsigned int i = 0; i < 5; ++i) {
        Level* lvi = factory.createLevel(i, i, "factory_test.txt");

        REQUIRE(lvi->getLevelNum() == i);
        REQUIRE(lvi->getSeed() == i);
        REQUIRE(lvi->getSrcfile() == "factory_test.txt");
        REQUIRE(lvi->getRandom() == i > 0);

        delete lvi;
    }
}

TEST_CASE(LevelFactory_Levelup) {
    LevelFactory factory;
    const unsigned int expectedSeed = 1;
    const std::string expectedSrcfile = "factory_test2.txt";

    Level* lv = nullptr;
    int i = 0;
    for (; i < 5; ++i) {
        if (!lv) {
            lv = factory.createLevel(0, expectedSeed, expectedSrcfile);
        } else {
            lv = factory.levelup(lv);
        }

        REQUIRE(lv->getLevelNum() == i);
        REQUIRE(lv->getSeed() == expectedSeed);
        REQUIRE(lv->getSrcfile() == expectedSrcfile);
    }

    lv = factory.levelup(lv);
    REQUIRE(lv->getLevelNum() == 4);  // level up shouldn't affect level 4
    REQUIRE(lv->getSeed() == expectedSeed);
    REQUIRE(lv->getSrcfile() == expectedSrcfile);
}

TEST_CASE(LevelFactory_Leveldown) {
    LevelFactory factory;
    const unsigned int expectedSeed = 2;
    const std::string expectedSrcfile = "factory_test3.txt";

    Level* lv = nullptr;
    int i = 4;
    for (; i >= 0; --i) {
        if (!lv) {
            lv = factory.createLevel(4, expectedSeed, expectedSrcfile);
        } else {
            lv = factory.leveldown(lv);
        }

        REQUIRE(lv->getLevelNum() == i);
        REQUIRE(lv->getSeed() == expectedSeed);
        REQUIRE(lv->getSrcfile() == expectedSrcfile);
    }

    lv = factory.leveldown(lv);
    REQUIRE(lv->getLevelNum() == 0);  // level down shouldn't affect level 0
    REQUIRE(lv->getSeed() == expectedSeed);
    REQUIRE(lv->getSrcfile() == expectedSrcfile);
}

TEST_CASE(Level0_BlockGeneration) {
    const std::string testFilePath = "level0_test_1.txt";
    std::ofstream testFile{testFilePath};

    const std::string testContent = "T I J O L S Z S I J J L Z Z";
    testFile << testContent;
    testFile.close();

    LevelFactory factory;
    Level* lv0 = factory.createLevel(0, 0, testFilePath);

    for (auto c : testContent) {
        if (c == ' ') continue;
        char expected = c;
        Block* block = lv0->getNextBlock();
        char actual = block->getChar();
        REQUIRE(expected == actual);

        delete block;
    }

    delete lv0;
    std::filesystem::remove(testFilePath);
}

TEST_CASE(Level0_BlockGeneration_SrcCirculation) {
    const std::string testFilePath = "level0_test_2.txt";
    std::ofstream testFile{testFilePath};

    const std::string testContent = "I J L";
    testFile << testContent;
    testFile.close();

    LevelFactory factory;
    Level* lv0 = factory.createLevel(0, 0, testFilePath);

    for (char c : testContent) {
        if (c == ' ') continue;
        char expected = c;
        Block* block = lv0->getNextBlock();
        char actual = block->getChar();
        REQUIRE(expected == actual);

        delete block;
    }

    // check if it goes back at the beginning of file
    for (char c : testContent) {
        if (c == ' ') continue;
        char expected = c;
        Block* block = lv0->getNextBlock();
        char actual = block->getChar();
        REQUIRE(expected == actual);

        delete block;
    }

    delete lv0;
    std::filesystem::remove(testFilePath);
}

void test_distribution(Level* level, int ratio_S, int ratio_Z, int ratio_I,
                       int ratio_J, int ratio_L, int ratio_O, int ratio_T,
                       int num_samples) {
    std::map<char, int> counts;

    for (int i = 0; i < num_samples; i++) {
        Block* block = level->getNextBlock();
        counts[block->getChar()]++;
        delete block;
    }

    // Calculate total ratio and expected counts
    int total_ratio =
        ratio_S + ratio_Z + ratio_I + ratio_J + ratio_L + ratio_O + ratio_T;

    // tolerance 20%
    auto check_range = [&](char type, int ratio) {
        int expected = (num_samples * ratio) / total_ratio;
        int lower = expected * 0.8;
        int upper = expected * 1.2;
        int actual = counts[type];
        std::string message = "Block " + std::string(1, type) +
                              ": count=" + std::to_string(actual) +
                              ", expected=[" + std::to_string(lower) + ", " +
                              std::to_string(upper) + "]";
        Tester::assert_true(actual > lower && actual < upper, message);
    };

    check_range('S', ratio_S);
    check_range('Z', ratio_Z);
    check_range('I', ratio_I);
    check_range('J', ratio_J);
    check_range('L', ratio_L);
    check_range('O', ratio_O);
    check_range('T', ratio_T);
}

TEST_CASE(Level1_distribution) {
    Level1* lv1 = new Level1(42);
    // Level1: S=1/12, Z=1/12, rest=2/12 each
    test_distribution(lv1, 1, 1, 2, 2, 2, 2, 2, 15000);
    delete lv1;
}

TEST_CASE(Level2_distribution) {
    Level2* lv2 = new Level2(42);
    // Level2: all equal
    test_distribution(lv2, 1, 1, 1, 1, 1, 1, 1, 15000);
    delete lv2;
}

TEST_CASE(Level3_distribution) {
    Level3* lv3 = new Level3(42);
    // Level3: S=2/9, Z=2/9, rest=1/9 each
    test_distribution(lv3, 2, 2, 1, 1, 1, 1, 1, 15000);
    delete lv3;
}

TEST_CASE(Level4_distribution) {
    Level4* lv4 = new Level4(42);
    // Level4: same as Level3
    test_distribution(lv4, 2, 2, 1, 1, 1, 1, 1, 15000);
    delete lv4;
}
