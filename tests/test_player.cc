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
import <vector>;

void setupSeqFile(const std::string& testFilePath,
                  const std::string& testContent) {
    std::ofstream testFile{testFilePath};

    testFile << testContent;
    testFile.close();
}

void removeSeqFile(const std::string& testFilePath) {
    std::filesystem::remove(testFilePath);
}

void Player_PlayMove() {
    std::string testFilePath = "test_playright.txt";
    std::string testContent = "L L L L L L";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    auto res = Zibo.play("right");
    PlayResult expect{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    Tester::assert_true(expect == res);

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {3, 1}, {3, 2}, {3, 3}, {2, 3}};
    for (auto [row, col] : expectedCoords) {
        Tester::assert_true('L' == pixels[row][col]);
    }

    removeSeqFile(testFilePath);
}

void Player_PlayLeft() {
    std::string testFilePath = "test_playleft.txt";
    std::string testContent = "L L L L L L";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    Zibo.play("right");
    auto res = Zibo.play("left");
    PlayResult expect{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    Tester::assert_true(expect == res);

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {3, 0}, {3, 1}, {3, 2}, {2, 2}};
    for (auto [row, col] : expectedCoords) {
        Tester::assert_true('L' == pixels[row][col]);
    }

    removeSeqFile(testFilePath);
}

void Player_PlayDown() {
    std::string testFilePath = "test_playdown.txt";
    std::string testContent = "L L L L L L";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    auto res = Zibo.play("down");
    PlayResult expect{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    Tester::assert_true(expect == res);

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {4, 0}, {4, 1}, {4, 2}, {3, 2}};
    for (auto [row, col] : expectedCoords) {
        Tester::assert_true('L' == pixels[row][col]);
    }

    removeSeqFile(testFilePath);
}

void Player_PlayRotation() {
    std::string testFilePath = "test_playrotate.txt";
    std::string testContent = "L L L L L L";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    // test clockwise
    auto res = Zibo.play("clockwise");
    PlayResult expect{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    Tester::assert_true(expect == res);

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {3, 0}, {3, 1}, {2, 0}, {1, 0}};
    for (auto [row, col] : expectedCoords) {
        Tester::assert_true('L' == pixels[row][col]);
    }

    // test counterclockwise
    res = Zibo.play("counterclockwise");
    Tester::assert_true(expect == res);

    pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords2 = {
        {3, 0}, {3, 1}, {3, 2}, {2, 2}};
    for (auto [row, col] : expectedCoords2) {
        Tester::assert_true('L' == pixels[row][col]);
    }

    removeSeqFile(testFilePath);
}

void Player_PlayDrop() {
    std::string testFilePath = "test_playdrop.txt";
    std::string testContent = "I I J";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    // drop first I
    auto res = Zibo.play("drop");
    PlayResult expect{PlayStatus::Endturn, {0, false, nullptr, {nullptr, 1}}};
    Tester::assert_true(expect == res);
    Tester::assert_true('I' == Zibo.getCurrentBlock()->getChar());
    Tester::assert_true('J' == Zibo.getNextBlock()->getChar());

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {17, 0}, {17, 1}, {17, 2}, {17, 3}};
    for (auto [row, col] : expectedCoords) {
        Tester::assert_true('I' == pixels[row][col]);
    }

    // drop second I to the right of the first
    for (int i = 0; i < 4; ++i) {
        Zibo.play("right");
    }
    res = Zibo.play("drop");
    Tester::assert_true(expect == res);
    Tester::assert_true('J' == Zibo.getCurrentBlock()->getChar());
    Tester::assert_true('I' == Zibo.getNextBlock()->getChar());

    pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords2 = {
        {17, 4}, {17, 5}, {17, 6}, {17, 7}};
    for (auto [row, col] : expectedCoords2) {
        Tester::assert_true('I' == pixels[row][col]);
    }

    // drop a J to the right of the second I to complete a row:
    // [          J  ]
    // [IIII IIII JJJ]
    for (int i = 0; i < 8; ++i) {
        Zibo.play("right");
    }
    res = Zibo.play("drop");

    Tester::assert_true(expect == res);
    Tester::assert_true('I' == Zibo.getCurrentBlock()->getChar());
    Tester::assert_true('I' == Zibo.getNextBlock()->getChar());
    // clear 1 row and 2 blocks
    // (0 + 1)^2 + (0 + 1)^2 + (0 + 1)^2 = 3
    Tester::assert_true(3 == Zibo.getScore());
    Tester::assert_true(3 == Zibo.getHighscore());

    pixels = Zibo.getPixels();
    for (int col = 0; col < 11; ++col) {
        if (col == 8) {
            Tester::assert_true('J' == pixels[17][col]);
        } else {
            Tester::assert_true(' ' == pixels[17][col]);
        }
    }

    removeSeqFile(testFilePath);
}

void Player_PlayDrop_SpecialAction_Blind() {
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
    Tester::assert_true(expectBlind == res);
}

void Player_PlayDrop_SpecialAction_Heavy() {
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
    Tester::assert_true(expectHeavy == res);

    removeSeqFile(testFilePath);
}

void Player_PlayDrop_SpecialAction_Force() {
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
    Tester::assert_true(expectForce == res);

    removeSeqFile(testFilePath);
}

void Player_PlayDrop_Lost() {
    std::string testFilePath = "test_playdrop.txt";
    std::string testContent = "I";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    for (int i = 0; i < 14; i++) Zibo.play("drop");
    auto res = Zibo.play("drop");
    Tester::assert_true(res.status == PlayStatus::Lost);

    removeSeqFile(testFilePath);
}

void Player_PlayDrop_Debuffed_Heavy() {
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
        Tester::assert_true(pixels[row][col] == 'Z');
    }

    // another down by one after left
    Zibo.play("left");
    std::vector<std::pair<int, int>> expectCoords2 = {
        {4, 0}, {4, 1}, {5, 1}, {5, 2}};
    pixels = Zibo.getPixels();
    for (const auto& [row, col] : expectCoords2) {
        Tester::assert_true(pixels[row][col] == 'Z');
    }

    removeSeqFile(testFilePath);
}

void Player_PlayDrop_Debuffed_Force() {
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
    Tester::assert_true(pixels[17][0] == 'L');

    removeSeqFile(testFilePath);
}

void Player_PlayLevelChange() {
    std::string testFilePath = "test_playlevelchange.txt";
    std::string testContent = "Z";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    int initLevel = 0;
    Player Zibo{lf.createLevel(initLevel, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};
    Debuff d{0, false, nullptr, {nullptr, 1}};

    Zibo.play("levelup");
    Tester::assert_true(Zibo.getLevelNum() == initLevel + 1);
    Zibo.play("levelup");
    Tester::assert_true(Zibo.getLevelNum() == initLevel + 2);
    Zibo.play("levelup");
    Tester::assert_true(Zibo.getLevelNum() == initLevel + 3);
    Zibo.play("levelup");
    Tester::assert_true(Zibo.getLevelNum() == initLevel + 4);

    Zibo.play("leveldown");
    Tester::assert_true(Zibo.getLevelNum() == initLevel + 3);
    Zibo.play("leveldown");
    Tester::assert_true(Zibo.getLevelNum() == initLevel + 2);
    Zibo.play("leveldown");
    Tester::assert_true(Zibo.getLevelNum() == initLevel + 1);
    Zibo.play("leveldown");
    Tester::assert_true(Zibo.getLevelNum() == initLevel);

    removeSeqFile(testFilePath);
}

void Player_PlayNoRandom() {
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
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'I');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'J');
    Zibo.play("drop");
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'J');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'Z');
    Zibo.play("drop");
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'Z');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'L');
    Zibo.play("drop");
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'L');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'O');
    Zibo.play("drop");
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'O');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'I');

    removeSeqFile(testFilePath);
}

void Player_PlayRandom() {
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
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'I');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'J');
    Zibo.play("drop");
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'J');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'Z');

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
    Tester::assert_true(!exactSeq);

    removeSeqFile(testFilePath);
}

void Player_PlayChar() {
    std::string testFilePath = "test_playchar.txt";
    std::string testContent = "I J";
    setupSeqFile(testFilePath, testContent);

    LevelFactory lf;
    Player Zibo{lf.createLevel(0, 67, testFilePath),
                std::make_shared<std::istream>(std::cin.rdbuf())};

    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'I');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'J');

    Zibo.play("L");
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'L');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'J');

    removeSeqFile(testFilePath);
}

void Player_PlayRestart() {
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
    Tester::assert_true(3 == Zibo.getScore());
    Tester::assert_true(3 == Zibo.getHighscore());

    auto pixels = Zibo.getPixels();
    Tester::assert_true('J' == pixels[17][8]);

    Zibo.play("restart");
    Tester::assert_true(0 == Zibo.getScore());
    Tester::assert_true(3 == Zibo.getHighscore());

    pixels = Zibo.getPixels();
    Tester::assert_true(' ' == pixels[17][8]);

    removeSeqFile(testFilePath);
}

void Player_Level4() {
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
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'I');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'J');

    // drop I J L at right side
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 10; ++j) Zibo.play("right");
        auto res = Zibo.play("drop");
        if (res.status == PlayStatus::Lost) {
            break;
        }
    }

    auto pixels = Zibo.getPixels();
    Tester::assert_true(pixels[17][5] == '*');
    Tester::assert_true(Zibo.getCurrentBlock()->getChar() == 'I');
    Tester::assert_true(Zibo.getNextBlock()->getChar() == 'J');

    removeSeqFile(testFilePath);
}

namespace {
Tester::TestRegistrar r19("Play move", Player_PlayMove);
Tester::TestRegistrar r20("Play left", Player_PlayLeft);
Tester::TestRegistrar r21("Play down", Player_PlayDown);
Tester::TestRegistrar r22("Play rotation", Player_PlayRotation);
Tester::TestRegistrar r23("Play drop", Player_PlayDrop);
Tester::TestRegistrar r24("Play drop: blind",
                          Player_PlayDrop_SpecialAction_Blind);
Tester::TestRegistrar r25("Play drop: force",
                          Player_PlayDrop_SpecialAction_Force);
Tester::TestRegistrar r26("Play drop: heavy",
                          Player_PlayDrop_SpecialAction_Heavy);
Tester::TestRegistrar r27("Play drop: lost", Player_PlayDrop_Lost);
Tester::TestRegistrar r28("Play drop & got debuffed heavy",
                          Player_PlayDrop_Debuffed_Heavy);
Tester::TestRegistrar r29("Play drop & got forced",
                          Player_PlayDrop_Debuffed_Force);
Tester::TestRegistrar r30("Play levelchange", Player_PlayLevelChange);
Tester::TestRegistrar r31("Play random", Player_PlayRandom);
Tester::TestRegistrar r32("Play norandom", Player_PlayNoRandom);
Tester::TestRegistrar r33("Play char", Player_PlayChar);
Tester::TestRegistrar r34("Play restart", Player_PlayRestart);
Tester::TestRegistrar r35("Play & getBombed", Player_Level4);
}  // namespace
