module;

#include <X11/Xlib.h>

module Renderers;

import Blocks;
import <string>;
import <vector>;
import <iostream>;
import <climits>;
import <algorithm>;

const int GUI_WINDOW_X = 100;
const int GUI_WINDOW_Y = 100;
const unsigned int GUI_WINDOW_WIDTH = 700;
const unsigned int GUI_WINDOW_BORDER_WIDTH = 10;
// split line width = 2% of window width
const unsigned int GUI_SPLIT_LINE_PERCENT = 1;
// margin-X width = 1% of window width
const unsigned int GUI_MARGIN_X_PERCENT = 1;
// margin-Y width = 1% of window height
const unsigned int GUI_MARGIN_Y_PERCENT = 1;

// width that we can use to draw for one player
const unsigned int GUI_PLAYER_WINDOW_WIDTH =
    (GUI_WINDOW_WIDTH *
     (100 - GUI_SPLIT_LINE_PERCENT - (4 * GUI_MARGIN_X_PERCENT))) /
    200;

// width for each cell (1 coord in a block)
const unsigned int GUI_CELL_WIDTH = GUI_PLAYER_WINDOW_WIDTH / 11;

// height that we can use to draw for one player
// top = 3 cells, board = 18 cells, bottom = 5 cells
const unsigned int GUI_PLAYER_WINDOW_HEIGHT = 26 * GUI_CELL_WIDTH;
const unsigned int GUI_WINDOW_HEIGHT =
    (GUI_PLAYER_WINDOW_HEIGHT * 100) / (100 - (2 * GUI_MARGIN_Y_PERCENT));

const std::string GUI_WINDOW_NAME = "Biquardris++";

GuiRenderer::GuiRenderer() {
    display = XOpenDisplay(NULL);
    if (!display) throw "Couldn't open X display";

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(
        display, RootWindow(display, screen), GUI_WINDOW_Y, GUI_WINDOW_X,
        GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, GUI_WINDOW_BORDER_WIDTH,
        BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, GUI_WINDOW_NAME.c_str());
    gc = XCreateGC(display, window, 0, NULL);

    XMapWindow(display, window);

    XFlush(display);
}

GuiRenderer::~GuiRenderer() {
    XFreeGC(display, gc);
    XCloseDisplay(display);
}

void GuiRenderer::clearWindow() {
    XSetForeground(display, gc, WhitePixel(display, screen));
    XFillRectangle(display, window, gc, 0, 0, GUI_WINDOW_WIDTH,
                   GUI_WINDOW_HEIGHT);

    XFlush(display);
}

void GuiRenderer::renderSplitLine() {
    int x = GUI_PLAYER_WINDOW_WIDTH +
            ((GUI_WINDOW_WIDTH * GUI_MARGIN_X_PERCENT) / 100);
    int y = 0;
    unsigned int width = ((GUI_WINDOW_WIDTH * GUI_SPLIT_LINE_PERCENT) / 100);
    unsigned int height = GUI_WINDOW_HEIGHT;

    XSetForeground(display, gc, BlackPixel(display, screen));
    XFillRectangle(display, window, gc, x, y, width, height);

    XFlush(display);
}

void GuiRenderer::renderHalf(const RenderPackage& pkg, bool left) {
    const std::string levelMsg = "Level: " + std::to_string(pkg.level);
    const std::string scoreMsg = "Score: " + std::to_string(pkg.score);
    const std::string highscoreMsg =
        "High score: " + std::to_string(pkg.highscore);
    const std::string nextMsg = "Next:";

    // cursor for traversing
    int x = (GUI_WINDOW_WIDTH * GUI_MARGIN_X_PERCENT) / 100;
    int y = (GUI_WINDOW_HEIGHT * GUI_MARGIN_Y_PERCENT) / 100;
    if (!left) {
        x += GUI_PLAYER_WINDOW_WIDTH +
             ((GUI_WINDOW_WIDTH *
               (GUI_SPLIT_LINE_PERCENT + (2 * GUI_MARGIN_X_PERCENT))) /
              100);
    }

    // render top part;
    XSetForeground(display, gc, BlackPixel(display, screen));
    y += GUI_CELL_WIDTH;
    XDrawString(display, window, gc, x, y, levelMsg.c_str(), levelMsg.length());
    y += GUI_CELL_WIDTH;
    XDrawString(display, window, gc, x, y, scoreMsg.c_str(), scoreMsg.length());
    y += GUI_CELL_WIDTH;
    XDrawString(display, window, gc, x, y, highscoreMsg.c_str(),
                highscoreMsg.length());

    // draw top part's end line:
    XDrawLine(display, window, gc, x, y + 10, x + GUI_PLAYER_WINDOW_WIDTH,
              y + 10);
    XFlush(display);

    // render board:
    for (int row = 0; row < 18; ++row, y += GUI_CELL_WIDTH) {
        int xAlias = x;
        for (int col = 0; col < 11; ++col, xAlias += GUI_CELL_WIDTH) {
            char c = pkg.pixels[row][col];
            if (c == ' ') continue;
            unsigned long color = colorMap.at(c);
            XSetForeground(display, gc, color);
            XFillRectangle(display, window, gc, xAlias, y, GUI_CELL_WIDTH,
                           GUI_CELL_WIDTH);
        }
    }
    XFlush(display);

    // render bottom part:
    XSetForeground(display, gc, BlackPixel(display, screen));
    y += GUI_CELL_WIDTH;
    XDrawString(display, window, gc, x, y, nextMsg.c_str(), nextMsg.length());

    if (pkg.nextBlock) {
        const std::vector<std::pair<int, int>> coords =
            pkg.nextBlock->getCoords();
        const unsigned long color = colorMap.at(pkg.nextBlock->getChar());
        XSetForeground(display, gc, color);

        // offset 5 cells: (xCoord,yCoord) -> (xCoord + 5, yCoord + 3)
        // (so that (0, 0) coord can be at the middle bottom)
        const int xOffset = 5;
        const int yOffset = 3;

        // block use (row, col) coord, so it's (y, x)
        for (auto& [yCoord, xCoord] : coords) {
            int xAlias = x + ((xCoord + xOffset) * GUI_CELL_WIDTH);
            int yAlias = y + ((yCoord + yOffset) * GUI_CELL_WIDTH);
            XFillRectangle(display, window, gc, xAlias, yAlias, GUI_CELL_WIDTH,
                           GUI_CELL_WIDTH);
        }
    }

    XFlush(display);
}

void GuiRenderer::render(const RenderPackage& p1, const RenderPackage& p2) {
    clearWindow();
    renderHalf(p1, true);
    renderSplitLine();
    renderHalf(p2, false);
}

void TuiRenderer::render(const RenderPackage &p1, const RenderPackage &p2) {
    // Header lines: level / score / highscore
    auto printHeader = [](const RenderPackage &p) {
        auto pad = [](int value, int width) {
            std::string s = std::to_string(value);
            if ((int)s.size() < width) s = std::string(width - s.size(), ' ') + s;
            return s;
        };
        std::cout << "Level:" << pad(p.level, 3) << " ";
        std::cout << "Score:" << pad(p.score, 6) << " ";
        std::cout << "High:" << pad(p.highscore, 6);
    };

    // print Level line and Score line separately for each half
    auto padToWidth = [](const std::string &s, int w) {
        if ((int)s.size() >= w) return s.substr(0, w);
        return s + std::string(w - s.size(), ' ');
    };
    auto printLevelLine = [&](const RenderPackage &a, const RenderPackage &b) {
        std::string L = "Level: " + std::to_string(a.level);
        std::string R = "Level: " + std::to_string(b.level);
        std::cout << padToWidth(L, 11) << "  " << padToWidth(R, 11) << '\n';
    };
    auto printScoreLine = [&](const RenderPackage &a, const RenderPackage &b) {
        std::string L = "Score: " + std::to_string(a.score);
        std::string R = "Score: " + std::to_string(b.score);
        std::cout << padToWidth(L, 11) << "  " << padToWidth(R, 11) << '\n';
    };

    printLevelLine(p1, p2);
    printScoreLine(p1, p2);

    // print top dashed separator (centered across the 11 printed columns, with one-space padding each side -> total 13)
    auto printTopBorder = [](void) {
        std::cout << std::string(11, '-');
    };
    printTopBorder();
    std::cout << "  ";
    printTopBorder();
    std::cout << '\n';

    // print rows (18 rows)
    for (int r = 0; r < 18; ++r) {
        // left board row (11 chars, left-aligned)
        for (int c = 0; c < 11; ++c) {
            char ch = p1.pixels[r][c];
            std::cout << (ch == ' ' ? ' ' : ch);
        }

        std::cout << "  ";

        // right board row (11 chars, left-aligned)
        for (int c = 0; c < 11; ++c) {
            char ch = p2.pixels[r][c];
            std::cout << (ch == ' ' ? ' ' : ch);
        }
        std::cout << '\n';
    }

    // bottom dashed separator aligned with board
    printTopBorder();
    std::cout << "  ";
    printTopBorder();
    std::cout << '\n';

    // build Next: preview for each half and print side-by-side aligned with board (13 chars per half)
    auto buildNextVec = [](const RenderPackage &p) {
        const int halfDisplayWidth = 11; // match board width
        std::vector<std::string> out;
        // First line: Next:
        std::string line = "Next:";
        if ((int)line.size() < halfDisplayWidth) line += std::string(halfDisplayWidth - line.size(), ' ');
        out.push_back(line);

        if (!p.nextBlock) {
            // no block: add a few empty lines
            for (int i = 0; i < 3; ++i) out.push_back(std::string(halfDisplayWidth, ' '));
            return out;
        }

        // compute bounding box of the block's coords
        auto coords = p.nextBlock->getCoords();
        char fill = p.nextBlock->getChar();
        int minR = INT_MAX, maxR = INT_MIN, minC = INT_MAX, maxC = INT_MIN;
        for (auto &pr : coords) {
            int r = pr.first;
            int c = pr.second;
            minR = std::min(minR, r);
            maxR = std::max(maxR, r);
            minC = std::min(minC, c);
            maxC = std::max(maxC, c);
        }
        int H = maxR - minR + 1;
        int W = maxC - minC + 1;
        // build minimal shape lines (row major from minR..maxR)
        for (int r = minR; r <= maxR; ++r) {
            std::string s;
            for (int c = minC; c <= maxC; ++c) {
                bool occupied = false;
                for (auto &pr : coords) if (pr.first == r && pr.second == c) { occupied = true; break; }
                s.push_back(occupied ? fill : ' ');
            }
            if ((int)s.size() < halfDisplayWidth) s += std::string(halfDisplayWidth - s.size(), ' ');
            out.push_back(s);
        }
        return out;
    };

    auto leftNext = buildNextVec(p1);
    auto rightNext = buildNextVec(p2);
    for (size_t i = 0; i < leftNext.size() || i < rightNext.size(); ++i) {
        std::string L = i < leftNext.size() ? leftNext[i] : std::string(13, ' ');
        std::string R = i < rightNext.size() ? rightNext[i] : std::string(13, ' ');
        std::cout << L << "  " << R << '\n';
    }
    std::cout << '\n';
}
