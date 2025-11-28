import Levels;
import Blocks;
import TestRunner;

import <iostream>;
import <fstream>;
import <filesystem>;
import <map>;
import <string>;
import <memory>;

void setupSeqFile(const std::string& testFilePath,
                  const std::string& testContent);
void removeSeqFile(const std::string& testFilePath);

void LevelFactoryLevelCreation() {
    LevelFactory factory;
    const std::string seqFilePath = "factory_test.txt";
    setupSeqFile(seqFilePath, "I O");

    for (unsigned int i = 0; i < 5; ++i) {
        std::shared_ptr<Level> lvi =
            factory.createLevel(i, i, "factory_test.txt");

        Tester::assert_true(lvi->getLevelNum() == i);
        Tester::assert_true(lvi->getSeed() == i);
        Tester::assert_true(lvi->getSrcfile() == "factory_test.txt");
        Tester::assert_true(lvi->getRandom() == i > 0);
    }
    removeSeqFile(seqFilePath);
}

void LevelFactoryLevelup() {
    LevelFactory factory;
    const unsigned int expectedSeed = 1;
    const std::string expectedSrcfile = "factory_test2.txt";
    setupSeqFile(expectedSrcfile, "I O Z");

    std::shared_ptr<Level> lv = nullptr;
    int i = 0;
    for (; i < 5; ++i) {
        if (!lv) {
            lv = factory.createLevel(0, expectedSeed, expectedSrcfile);
        } else {
            lv = factory.levelup(lv);
        }

        Tester::assert_true(lv->getLevelNum() == i);
        Tester::assert_true(lv->getSeed() == expectedSeed);
        Tester::assert_true(lv->getSrcfile() == expectedSrcfile);
    }

    lv = factory.levelup(lv);
    Tester::assert_true(lv->getLevelNum() ==
                        4);  // level up shouldn't affect level 4
    Tester::assert_true(lv->getSeed() == expectedSeed);
    Tester::assert_true(lv->getSrcfile() == expectedSrcfile);
    removeSeqFile(expectedSrcfile);
}

void LevelFactoryLeveldown() {
    LevelFactory factory;
    const unsigned int expectedSeed = 2;
    const std::string expectedSrcfile = "factory_test3.txt";
    setupSeqFile(expectedSrcfile, "I O");

    std::shared_ptr<Level> lv = nullptr;
    int i = 4;
    for (; i >= 0; --i) {
        if (!lv) {
            lv = factory.createLevel(4, expectedSeed, expectedSrcfile);
        } else {
            lv = factory.leveldown(lv);
        }

        Tester::assert_true(lv->getLevelNum() == i);
        Tester::assert_true(lv->getSeed() == expectedSeed);
        Tester::assert_true(lv->getSrcfile() == expectedSrcfile);
    }

    lv = factory.leveldown(lv);
    Tester::assert_true(lv->getLevelNum() ==
                        0);  // level down shouldn't affect level 0
    Tester::assert_true(lv->getSeed() == expectedSeed);
    Tester::assert_true(lv->getSrcfile() == expectedSrcfile);
    removeSeqFile(expectedSrcfile);
}

void Level0BlockGeneration() {
    const std::string testFilePath = "level0_test_1.txt";
    std::ofstream testFile{testFilePath};

    const std::string testContent = "T I J O L S Z S I J J L Z Z";
    testFile << testContent;
    testFile.close();

    LevelFactory factory;
    std::shared_ptr<Level> lv0 = factory.createLevel(0, 0, testFilePath);

    for (auto c : testContent) {
        if (c == ' ') continue;
        char expected = c;
        std::shared_ptr<Block> block = lv0->getNextBlock();
        char actual = block->getChar();
        Tester::assert_true(expected == actual);
    }

    std::filesystem::remove(testFilePath);
}

void Level0BlockGenerationSrcCirculation() {
    const std::string testFilePath = "level0_test_2.txt";
    std::ofstream testFile{testFilePath};

    const std::string testContent = "I J L";
    testFile << testContent;
    testFile.close();

    LevelFactory factory;
    std::shared_ptr<Level> lv0 = factory.createLevel(0, 0, testFilePath);

    for (char c : testContent) {
        if (c == ' ') continue;
        char expected = c;
        std::shared_ptr<Block> block = lv0->getNextBlock();
        char actual = block->getChar();
        Tester::assert_true(expected == actual);
    }

    // check if it goes back at the beginning of file
    for (char c : testContent) {
        if (c == ' ') continue;
        char expected = c;
        std::shared_ptr<Block> block = lv0->getNextBlock();
        char actual = block->getChar();
        Tester::assert_true(expected == actual);
    }

    std::filesystem::remove(testFilePath);
}

void test_distribution(std::shared_ptr<Level> level, int ratio_S, int ratio_Z,
                       int ratio_I, int ratio_J, int ratio_L, int ratio_O,
                       int ratio_T, int num_samples) {
    std::map<char, int> counts;

    for (int i = 0; i < num_samples; i++) {
        std::shared_ptr<Block> block = level->getNextBlock();
        counts[block->getChar()]++;
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

void Level1distribution() {
    std::shared_ptr<Level1> lv1 = std::make_shared<Level1>(42);
    // Level1: S=1/12, Z=1/12, rest=2/12 each
    test_distribution(lv1, 1, 1, 2, 2, 2, 2, 2, 15000);
}

void Level2distribution() {
    std::shared_ptr<Level2> lv2 = std::make_shared<Level2>(42);
    // Level2: all equal
    test_distribution(lv2, 1, 1, 1, 1, 1, 1, 1, 15000);
}

void Level3distribution() {
    std::shared_ptr<Level3> lv3 = std::make_shared<Level3>(42);
    // Level3: S=2/9, Z=2/9, rest=1/9 each
    test_distribution(lv3, 2, 2, 1, 1, 1, 1, 1, 15000);
}

void Level4distribution() {
    std::shared_ptr<Level4> lv4 = std::make_shared<Level4>(42);
    // Level4: same as Level3
    test_distribution(lv4, 2, 2, 1, 1, 1, 1, 1, 15000);
}

namespace {
Tester::TestRegistrar r9("Level creation", LevelFactoryLevelCreation);
Tester::TestRegistrar r10("Level up", LevelFactoryLevelup);
Tester::TestRegistrar r11("Level down", LevelFactoryLeveldown);
Tester::TestRegistrar r12("Level0 block gen", Level0BlockGeneration);
Tester::TestRegistrar r13("Level0 block gen loop",
                          Level0BlockGenerationSrcCirculation);
Tester::TestRegistrar r14("Level0 block gen loop",
                          Level0BlockGenerationSrcCirculation);
Tester::TestRegistrar r15("Level1 dist", Level1distribution);
Tester::TestRegistrar r16("Level2 dist", Level2distribution);
Tester::TestRegistrar r17("Level3 dist", Level3distribution);
Tester::TestRegistrar r18("Level4 dist", Level4distribution);
}  // namespace
