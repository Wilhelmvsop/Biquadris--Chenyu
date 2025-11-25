import Player;
import Levels;
import Blocks;
import Board;

import <fstream>;
import <filesystem>;
import <iostream>;
import <utility>;

#include "test_runner.h"

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
    Player Zibo{lf.createLevel(0, 67, testFilePath), &std::cin};
    Debuff d{};
    auto res = Zibo.play("right", d);
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
    Player Zibo{lf.createLevel(0, 67, testFilePath), &std::cin};
    Debuff d{};
    Zibo.play("right", d);
    auto res = Zibo.play("left", d);
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
    Player Zibo{lf.createLevel(0, 67, testFilePath), &std::cin};
    Debuff d{};
    auto res = Zibo.play("down", d);
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
    Player Zibo{lf.createLevel(0, 67, testFilePath), &std::cin};
    Debuff d{};

    // test clockwise
    auto res = Zibo.play("clockwise", d);
    PlayResult expect{PlayStatus::Continue, {0, false, nullptr, {nullptr, 1}}};
    REQUIRE(expect == res);

    auto pixels = Zibo.getPixels();
    std::vector<std::pair<int, int>> expectedCoords = {
        {3, 0}, {3, 1}, {2, 0}, {1, 0}};
    for (auto [row, col] : expectedCoords) {
        REQUIRE('L' == pixels[row][col]);
    }

    // test counterclockwise
    res = Zibo.play("counterclockwise", d);
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
    Player Zibo{lf.createLevel(0, 67, testFilePath), &std::cin};
    Debuff d{};

    // drop first I
    auto res = Zibo.play("drop", d);
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
        Zibo.play("right", d);
    }
    res = Zibo.play("drop", d);
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
        Zibo.play("right", d);
    }
    res = Zibo.play("drop", d);

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

// TESTCASE(Player_PlayDrop_SpecialAction) {}
// TESTCASE(Player_PlayDrop_Lost) {}
// TESTCASE(Player_PlayDrop_Debuffed) {}
