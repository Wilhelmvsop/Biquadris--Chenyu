import Player;
import Levels;
import Blocks;
import Board;
import TestRunner;

import <fstream>;
import <filesystem>;
import <iostream>;
import <utility>;
import <memory>;

void setupSeqFile(const std::string& testFilePath,
                  const std::string& testContent) {
    std::ofstream testFile{testFilePath};

    testFile << testContent;
    testFile.close();
}

void removeSeqFile(const std::string& testFilePath) {
    std::filesystem::remove(testFilePath);
}

TEST_CASE(Player_PlayMove) {
    std::string testFilePath = "test_playright.txt";
    std::string testContent = "L L L L L L";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    auto res = Zibo.play("right");
    PlayResult expect{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    REQUIRE(expect == res);

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {3, 1}, {3, 2}, {3, 3}, {2, 3}};
    for (auto [row, col] : expectedCoords) {
        REQUIRE('L' == pixels[row][col]);
    }

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayLeft) {
    std::string testFilePath = "test_playleft.txt";
    std::string testContent = "L L L L L L";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    Zibo.play("right");
    auto res = Zibo.play("left");
    PlayResult expect{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    REQUIRE(expect == res);

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {3, 0}, {3, 1}, {3, 2}, {2, 2}};
    for (auto [row, col] : expectedCoords) {
        REQUIRE('L' == pixels[row][col]);
    }

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayDown) {
    std::string testFilePath = "test_playdown.txt";
    std::string testContent = "L L L L L L";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    auto res = Zibo.play("down");
    PlayResult expect{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    REQUIRE(expect == res);

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {4, 0}, {4, 1}, {4, 2}, {3, 2}};
    for (auto [row, col] : expectedCoords) {
        REQUIRE('L' == pixels[row][col]);
    }

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayRotation) {
    std::string testFilePath = "test_playrotate.txt";
    std::string testContent = "L L L L L L";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    // test clockwise
    auto res = Zibo.play("clockwise");
    PlayResult expect{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    REQUIRE(expect == res);

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {3, 0}, {3, 1}, {2, 0}, {1, 0}};
    for (auto [row, col] : expectedCoords) {
        REQUIRE('L' == pixels[row][col]);
    }

    // test counterclockwise
    res = Zibo.play("counterclockwise");
    REQUIRE(expect == res);

    pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords2 = {
        {3, 0}, {3, 1}, {3, 2}, {2, 2}};
    for (auto [row, col] : expectedCoords2) {
        REQUIRE('L' == pixels[row][col]);
    }

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayDrop) {
    std::string testFilePath = "test_playdrop.txt";
    std::string testContent = "I I J";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    // drop first I
    auto res = Zibo.play("drop");
    PlayResult expect{PlayStatus::Endturn, {0, false, nullptr, {nullptr, 1}}};
    REQUIRE(expect == res);
    REQUIRE('I' == Zibo.getCurrentBlock()->getChar());
    REQUIRE('J' == Zibo.getNextBlock()->getChar());

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {17, 0}, {17, 1}, {17, 2}, {17, 3}};
    for (auto [row, col] : expectedCoords) {
        REQUIRE('I' == pixels[row][col]);
    }

    // drop second I to the right of the first
    for (int i = 0; i < 4; ++i) {
        Zibo.play("right");
    }
    res = Zibo.play("drop");
    REQUIRE(expect == res);
    REQUIRE('J' == Zibo.getCurrentBlock()->getChar());
    REQUIRE('I' == Zibo.getNextBlock()->getChar());

    pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords2 = {
        {17, 4}, {17, 5}, {17, 6}, {17, 7}};
    for (auto [row, col] : expectedCoords2) {
        REQUIRE('I' == pixels[row][col]);
    }

    // drop a J to the right of the second I to complete a row:
    // [          J  ]
    // [IIII IIII JJJ]
    for (int i = 0; i < 8; ++i) {
        Zibo.play("right");
    }
    res = Zibo.play("drop");

    REQUIRE(expect == res);
    REQUIRE('I' == Zibo.getCurrentBlock()->getChar());
    REQUIRE('I' == Zibo.getNextBlock()->getChar());
    // clear 1 row and 2 blocks
    // (0 + 1)^2 + (0 + 1)^2 + (0 + 1)^2 = 3
    REQUIRE(3 == Zibo.getScore());
    REQUIRE(3 == Zibo.getHighscore());

    pixels = Zibo.getPixels();
    for (int col = 0; col < 11; ++col) {
        if (col == 8) {
            REQUIRE('J' == pixels[17][col]);
        } else {
            REQUIRE(' ' == pixels[17][col]);
        }
    }

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayDrop_SpecialAction_Blind) {
    std::string testFilePath = "test_playdrop_special_action.txt";
    std::string testContent = "O O O O O I";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istringstream>("blind heavy force J")};

    // should end up like this before clear:
    // [               I]
    // [               I]
    // [OO OO OO OO OO I]
    // [OO OO OO OO OO I]

    Zibo.play("drop");
    for (int i = 0; i < 2; ++i) Zibo.play("right");
    Zibo.play("drop");
    for (int i = 0; i < 4; ++i) Zibo.play("right");
    Zibo.play("drop");
    for (int i = 0; i < 6; ++i) Zibo.play("right");
    Zibo.play("drop");
    for (int i = 0; i < 8; ++i) Zibo.play("right");
    Zibo.play("drop");
    Zibo.play("clockwise");
    for (int i = 0; i < 10; ++i) Zibo.play("right");
    PlayResult res = Zibo.play("drop");
    PlayResult expectBlind{PlayStatus::Endturn,
                           {0, true, nullptr, {nullptr, 1}}};
    REQUIRE(expectBlind == res);
}

TEST_CASE(Player_PlayDrop_SpecialAction_Heavy) {
    std::string testFilePath = "test_playdrop_special_action.txt";
    std::string testContent = "O O O O O I";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istringstream>("heavy force J")};

    // should end up like this before clear:
    // [               I]
    // [               I]
    // [OO OO OO OO OO I]
    // [OO OO OO OO OO I]
    Zibo.play("drop");
    for (int i = 0; i < 2; ++i) Zibo.play("right");
    Zibo.play("drop");
    for (int i = 0; i < 4; ++i) Zibo.play("right");
    Zibo.play("drop");
    for (int i = 0; i < 6; ++i) Zibo.play("right");
    Zibo.play("drop");
    for (int i = 0; i < 8; ++i) Zibo.play("right");
    Zibo.play("drop");
    Zibo.play("clockwise");
    for (int i = 0; i < 10; ++i) Zibo.play("right");
    PlayResult res = Zibo.play("drop");
    PlayResult expectHeavy{PlayStatus::Endturn,
                           {1, false, nullptr, {nullptr, 1}}};
    REQUIRE(expectHeavy == res);

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayDrop_SpecialAction_Force) {
    std::string testFilePath = "test_playdrop_special_action.txt";
    std::string testContent = "O O O O O I";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istringstream>("force J blind heavy")};

    // should end up like this before clear:
    // [               I]
    // [               I]
    // [OO OO OO OO OO I]
    // [OO OO OO OO OO I]
    Zibo.play("drop");
    for (int i = 0; i < 2; ++i) Zibo.play("right");
    Zibo.play("drop");
    for (int i = 0; i < 4; ++i) Zibo.play("right");
    Zibo.play("drop");
    for (int i = 0; i < 6; ++i) Zibo.play("right");
    Zibo.play("drop");
    for (int i = 0; i < 8; ++i) Zibo.play("right");
    Zibo.play("drop");
    Zibo.play("clockwise");
    for (int i = 0; i < 10; ++i) Zibo.play("right");
    PlayResult res = Zibo.play("drop");
    BlockFactory bf{};
    std::shared_ptr<Block> expectedForcedBlock = bf.createBlock('J', 0);
    PlayResult expectForce{PlayStatus::Endturn,
                           {0, false, bf.createBlock('J', 0), {nullptr, 1}}};
    REQUIRE(expectForce == res);

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayDrop_Lost) {
    std::string testFilePath = "test_playdrop.txt";
    std::string testContent = "I";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    for (int i = 0; i < 14; i++) Zibo.play("drop");
    auto res = Zibo.play("drop");
    REQUIRE(res.status == PlayStatus::Lost);

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayDrop_Debuffed_Heavy) {
    std::string testFilePath = "test_playdrop.txt";
    std::string testContent = "Z";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    Debuff heavy{1, false, nullptr, {nullptr, 1}};

    // should be down by one after right
    Zibo.updateDebuff(heavy);
    Zibo.play("right");
    std::vector<std::pair<int, int>> expectCoords = {
        {3, 1}, {3, 2}, {4, 2}, {4, 3}};
    auto pixels = Zibo.getPixels();
    for (const auto& [row, col] : expectCoords) {
        REQUIRE(pixels[row][col] == 'Z');
    }

    // another down by one after left
    Zibo.play("left");
    std::vector<std::pair<int, int>> expectCoords2 = {
        {4, 0}, {4, 1}, {5, 1}, {5, 2}};
    pixels = Zibo.getPixels();
    for (const auto& [row, col] : expectCoords2) {
        REQUIRE(pixels[row][col] == 'Z');
    }

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayDrop_Debuffed_Force) {
    std::string testFilePath = "test_playdrop.txt";
    std::string testContent = "Z";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    BlockFactory bf;
    Debuff forceL{0, false, bf.createBlock('L', 0), {nullptr, 1}};

    Zibo.updateDebuff(forceL);
    Zibo.play("drop");
    auto pixels = Zibo.getPixels();
    REQUIRE(pixels[17][0] == 'L');

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayLevelChange) {
    std::string testFilePath = "test_playlevelchange.txt";
    std::string testContent = "Z";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    int initLevel = 0;
    Player Zibo{lf.createLevel(initLevel, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    Debuff d{0, false, nullptr, {nullptr, 1}};

    Zibo.play("levelup");
    REQUIRE(Zibo.getLevelNum() == initLevel + 1);
    Zibo.play("levelup");
    REQUIRE(Zibo.getLevelNum() == initLevel + 2);
    Zibo.play("levelup");
    REQUIRE(Zibo.getLevelNum() == initLevel + 3);
    Zibo.play("levelup");
    REQUIRE(Zibo.getLevelNum() == initLevel + 4);

    Zibo.play("leveldown");
    REQUIRE(Zibo.getLevelNum() == initLevel + 3);
    Zibo.play("leveldown");
    REQUIRE(Zibo.getLevelNum() == initLevel + 2);
    Zibo.play("leveldown");
    REQUIRE(Zibo.getLevelNum() == initLevel + 1);
    Zibo.play("leveldown");
    REQUIRE(Zibo.getLevelNum() == initLevel);

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayNoRandom) {
    std::string testFilePath = "test_play_norandom.txt";
    std::string testContent = "I J Z L O";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    int initLevel = 3;  // should work on level 3 or 4
    Player Zibo{lf.createLevel(initLevel, 67, "fakeFilepath.txt"),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    Zibo.play("norandom", testFilePath);

    // clear the first 2 random blocks
    Zibo.play("drop");
    Zibo.play("drop");

    // this should be non-random sequence
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'I');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'J');
    Zibo.play("drop");
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'J');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'Z');
    Zibo.play("drop");
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'Z');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'L');
    Zibo.play("drop");
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'L');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'O');
    Zibo.play("drop");
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'O');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'I');

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayRandom) {
    std::string testFilePath = "test_play_random.txt";
    std::string testContent = "I J Z L O";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    int initLevel = 3;  // should work on level 3 or 4
    Player Zibo{lf.createLevel(initLevel, 67, "fakeFilepath.txt"),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    Zibo.play("norandom", testFilePath);

    // clear the first 2 random blocks
    Zibo.play("drop");
    Zibo.play("drop");

    // this should be non-random sequence
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'I');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'J');
    Zibo.play("drop");
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'J');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'Z');

    Zibo.play("random");
    // clear 2 current non-random blocks
    Zibo.play("drop");
    Zibo.play("drop");

    // shouldn't generate exact sequence we used to set
    bool exactSeq = true;
    for (auto ch : std::string("L O I J Z")) {
        if (ch == ' ') continue;
        char currBlock = Zibo.getCurrentBlock()->getChar();
        if (ch != currBlock) {
            exactSeq = false;
        }
        auto res = Zibo.play("drop");
        if (res.status == PlayStatus::Lost) {
            break;
        }
    }
    REQUIRE(!exactSeq);

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayChar) {
    std::string testFilePath = "test_playchar.txt";
    std::string testContent = "I J";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'I');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'J');

    Zibo.play("L");
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'L');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'J');

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_PlayRestart) {
    std::string testFilePath = "test_playrestart.txt";
    std::string testContent = "I I J";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    // [          J  ]
    // [IIII IIII JJJ]
    auto res = Zibo.play("drop");
    for (int i = 0; i < 4; ++i) Zibo.play("right");
    res = Zibo.play("drop");
    for (int i = 0; i < 8; ++i) Zibo.play("right");
    res = Zibo.play("drop");

    // clear 1 row and 2 blocks
    // (0 + 1)^2 + (0 + 1)^2 + (0 + 1)^2 = 3
    REQUIRE(3 == Zibo.getScore());
    REQUIRE(3 == Zibo.getHighscore());

    auto pixels = Zibo.getPixels();
    REQUIRE('J' == pixels[17][8]);

    Zibo.play("restart");
    REQUIRE(0 == Zibo.getScore());
    REQUIRE(3 == Zibo.getHighscore());

    pixels = Zibo.getPixels();
    REQUIRE(' ' == pixels[17][8]);

    removeSeqFile(testFilePath);
}

TEST_CASE(Player_Level4) {
    std::string testFilePath = "test_playerL4.txt";
    std::string testContent = "I J L";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(4, 67, "fakeFilePath.txt"),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    BlockFactory bf;

    Zibo.play("norandom", testFilePath);
    // clear first 2 random blocks at somewhere leftside
    Zibo.play("drop");
    Zibo.play("drop");
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'I');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'J');

    // drop I J L at right side
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 10; ++j) Zibo.play("right");
        auto res = Zibo.play("drop");
        if (res.status == PlayStatus::Lost) {
            break;
        }
    }

    auto pixels = Zibo.getPixels();
    REQUIRE(pixels[17][5] == '*');
    REQUIRE(Zibo.getCurrentBlock()->getChar() == 'I');
    REQUIRE(Zibo.getNextBlock()->getChar() == 'J');

    removeSeqFile(testFilePath);
}
