module;

#include <X11/Xlib.h>
#include <X11/Xutil.h>

module Renderers;

import Blocks;
import <string>;
import <vector>;
import <iostream>;
import <climits>;
import <algorithm>;
import <stdexcept>;

bool RenderPackage::operator==(const RenderPackage& other) const {
    return score == other.score && highscore == other.highscore &&
           pixels == other.pixels && level == other.level &&
           nextBlock->getChar() == other.nextBlock->getChar() &&
           lost == other.lost;
}

bool RenderPackage::operator!=(const RenderPackage& other) const {
    return !(*this == other);
}

const int GUI_WINDOW_X = 100;
const int GUI_WINDOW_Y = 100;
const unsigned int GUI_WINDOW_WIDTH = 800;
const unsigned int GUI_WINDOW_BORDER_WIDTH = 10;
// split line width = 1% of window width
const unsigned int GUI_SPLIT_LINE_PERCENT = 0;
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
// top = 4 cells, board = 18 cells, bottom = 5 cells
const unsigned int GUI_PLAYER_WINDOW_HEIGHT = 27 * GUI_CELL_WIDTH;
const unsigned int GUI_WINDOW_HEIGHT =
    (GUI_PLAYER_WINDOW_HEIGHT * 100) / (100 - (2 * GUI_MARGIN_Y_PERCENT));

const std::string GUI_WINDOW_NAME = "Biquardris++";
const std::string GUI_FONT_QUERY = "-*-helvetica-medium-r-*-*-16-*-*-*-*-*-*-*";
const std::string GUI_BIG_FONT_QUERY =
    "-*-helvetica-bold-r-*-*-48-*-*-*-*-*-*-*";

GuiRenderer::GuiRenderer()
    : cacheP1{-1, -1, {}, -1, nullptr, false},
      cacheP2{-1, -1, {}, -1, nullptr, false} {
    display = XOpenDisplay(NULL);
    if (!display) throw std::runtime_error("Couldn't open X display");

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(
        display, RootWindow(display, screen), GUI_WINDOW_Y, GUI_WINDOW_X,
        GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, GUI_WINDOW_BORDER_WIDTH,
        WhitePixel(display, screen), BlackPixel(display, screen));
    XStoreName(display, window, GUI_WINDOW_NAME.c_str());
    pixmap = XCreatePixmap(display, window, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT,
                           DefaultDepth(display, screen));
    gc = XCreateGC(display, pixmap, 0, NULL);

    XMapWindow(display, window);

    XFlush(display);

    // load fonts
    font = XLoadQueryFont(display, GUI_FONT_QUERY.c_str());
    if (!font) {
        font = XLoadQueryFont(display, "fixed");  // fallback
    }
    bigFont = XLoadQueryFont(display, GUI_BIG_FONT_QUERY.c_str());
    if (!bigFont) {
        bigFont = XLoadQueryFont(display, "fixed");  // fallback
    }

    // normal font for gc
    XSetFont(display, gc, font->fid);
    XFlush(display);
    usleep(1000);

    // Make window non-resizeable (stolen from demo)
    XSizeHints hints;
    hints.flags = (USPosition | PSize | PMinSize | PMaxSize);
    hints.height = hints.base_height = hints.min_height = hints.max_height =
        GUI_WINDOW_HEIGHT;
    hints.width = hints.base_width = hints.min_width = hints.max_width =
        GUI_WINDOW_WIDTH;
    XSetNormalHints(display, window, &hints);

    XSynchronize(display, True);
    usleep(10000);
}

GuiRenderer::~GuiRenderer() {
    if (font) XFreeFont(display, font);
    if (bigFont) XFreeFont(display, bigFont);
    XFreePixmap(display, pixmap);
    XFreeGC(display, gc);
    XCloseDisplay(display);
}

void GuiRenderer::clearHalfWindow(const RenderPackage& pkg, bool left,
                                  const RenderPackage& cache) {
    int x = (GUI_WINDOW_WIDTH * GUI_MARGIN_X_PERCENT) / 100;
    int y = (GUI_WINDOW_HEIGHT * GUI_MARGIN_Y_PERCENT) / 100;

    if (!left) {
        x += GUI_PLAYER_WINDOW_WIDTH +
             ((GUI_WINDOW_WIDTH *
               (GUI_SPLIT_LINE_PERCENT + (2 * GUI_MARGIN_X_PERCENT))) /
              100);
    }

    XSetForeground(display, gc, BlackPixel(display, screen));
    std::vector<XRectangle> rects{};

    if (pkg.level != cache.level) {
        rects.emplace_back(
            XRectangle{x, y, GUI_PLAYER_WINDOW_WIDTH, GUI_CELL_WIDTH});
    }
    if (pkg.score != cache.score) {
        int yScore = y + GUI_CELL_WIDTH;
        rects.emplace_back(
            XRectangle{x, yScore, GUI_PLAYER_WINDOW_WIDTH, GUI_CELL_WIDTH});
    }
    if (pkg.highscore != cache.highscore) {
        int yHScore = y + 2 * GUI_CELL_WIDTH;
        rects.emplace_back(
            XRectangle{x, yHScore, GUI_PLAYER_WINDOW_WIDTH, GUI_CELL_WIDTH});
    }
    if (pkg.pixels != cache.pixels) {
        int yBoard = y + 5 * GUI_CELL_WIDTH;
        rects.emplace_back(XRectangle{x, yBoard, GUI_PLAYER_WINDOW_WIDTH,
                                      18 * GUI_CELL_WIDTH});
    }
    if (!pkg.nextBlock.get() || !cache.nextBlock.get() ||
        pkg.nextBlock->getChar() != cache.nextBlock->getChar()) {
        int yNext = y + 23 * GUI_CELL_WIDTH;
        rects.emplace_back(
            XRectangle{x, yNext, GUI_PLAYER_WINDOW_WIDTH, 4 * GUI_CELL_WIDTH});
    }

    XFillRectangles(display, pixmap, gc, rects.data(), rects.size());
}

void GuiRenderer::renderSplitLine() {
    int x = GUI_PLAYER_WINDOW_WIDTH +
            ((GUI_WINDOW_WIDTH * GUI_MARGIN_X_PERCENT) / 100);
    int y = 0;

    XSetForeground(display, gc, WhitePixel(display, screen));
    if (GUI_SPLIT_LINE_PERCENT > 0) {
        unsigned int width =
            ((GUI_WINDOW_WIDTH * GUI_SPLIT_LINE_PERCENT) / 100);
        unsigned int height = GUI_WINDOW_HEIGHT;
        XFillRectangle(display, pixmap, gc, x, y, width, height);
    } else {
        XDrawLine(display, pixmap, gc, x, y, x, y + GUI_WINDOW_HEIGHT);
    }
}

void GuiRenderer::renderHalf(const RenderPackage& pkg, bool left,
                             const RenderPackage& cache) {
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

    std::vector<XSegment> segments{};
    std::vector<XRectangle> rects{};
    std::vector<char> types{};

    // render top part;
    XSetForeground(display, gc, WhitePixel(display, screen));
    y += GUI_CELL_WIDTH;
    if (pkg.level != cache.level) {
        const std::string levelMsg = "Level: " + std::to_string(pkg.level);
        XDrawString(display, pixmap, gc, x, y, levelMsg.c_str(),
                    levelMsg.length());
    }
    y += GUI_CELL_WIDTH;
    if (pkg.score != cache.score) {
        const std::string scoreMsg = "Score: " + std::to_string(pkg.score);
        XDrawString(display, pixmap, gc, x, y, scoreMsg.c_str(),
                    scoreMsg.length());
    }
    y += GUI_CELL_WIDTH;
    if (pkg.highscore != cache.highscore) {
        const std::string highscoreMsg =
            "High score: " + std::to_string(pkg.highscore);
        XDrawString(display, pixmap, gc, x, y, highscoreMsg.c_str(),
                    highscoreMsg.length());
    }

    // draw top part's end line:
    y += GUI_CELL_WIDTH;
    int xBorder =
        left ? 0 : x - ((2 * GUI_WINDOW_WIDTH * GUI_MARGIN_X_PERCENT) / 100);
    segments.emplace_back(
        XSegment{xBorder, y, xBorder + (GUI_WINDOW_WIDTH / 2), y});

    y += GUI_CELL_WIDTH;
    // render board outline:
    int xGrid = x;
    for (int i = 0; i < 12; ++i, xGrid += GUI_CELL_WIDTH) {
        segments.emplace_back(
            XSegment{xGrid, y, xGrid, y + (18 * GUI_CELL_WIDTH)});
    }
    int yGrid = y;
    for (int i = 0; i < 19; ++i, yGrid += GUI_CELL_WIDTH) {
        segments.emplace_back(
            XSegment{x, yGrid, x + (11 * GUI_CELL_WIDTH), yGrid});
    }

    // render blocks:
    for (int row = 0; row < 18; ++row, y += GUI_CELL_WIDTH) {
        int xAlias = x;
        for (int col = 0; col < 11; ++col, xAlias += GUI_CELL_WIDTH) {
            char c = pkg.pixels[row][col];
            if (c == ' ') continue;
            rects.emplace_back(
                XRectangle{xAlias, y, GUI_CELL_WIDTH, GUI_CELL_WIDTH});
            types.emplace_back(c);
        }
    }

    // render bottom part:
    segments.emplace_back(
        XSegment{xBorder, y, xBorder + (GUI_WINDOW_WIDTH / 2), y});
    y += GUI_CELL_WIDTH;
    XDrawString(display, pixmap, gc, x, y, nextMsg.c_str(), nextMsg.length());

    if (pkg.nextBlock.get()) {
        const std::vector<std::pair<int, int>> coords =
            pkg.nextBlock->getCoords();
        const char nextC = pkg.nextBlock->getChar();
        // const unsigned long color = colorMap.at(pkg.nextBlock->getChar());

        // offset to kinda make it at the middle
        // (so that (0, 0) coord can be at the middle bottom)
        const int xOffset = 3;
        const int yOffset = -2;

        // block use (row, col) coord, so it's (y+3, x)
        for (auto& [yCoord, xCoord] : coords) {
            int xAlias = x + ((xCoord + xOffset) * GUI_CELL_WIDTH);
            int yAlias = y + ((yCoord + yOffset) * GUI_CELL_WIDTH);
            rects.emplace_back(
                XRectangle{xAlias, yAlias, GUI_CELL_WIDTH, GUI_CELL_WIDTH});
            types.emplace_back(nextC);
        }
    }

    // render all segments
    XDrawSegments(display, pixmap, gc, segments.data(), segments.size());

    // render all rectangles
    std::unordered_map<char, std::vector<XRectangle>> coloredRects;
    for (unsigned int i = 0; i < rects.size(); ++i) {
        coloredRects[types[i]].emplace_back(rects[i]);
    }

    for (auto& [c, rectG] : coloredRects) {
        unsigned long color = colorMap.at(c);
        XSetForeground(display, gc, color);
        XFillRectangles(display, pixmap, gc, rectG.data(), rectG.size());
    }

    XSetForeground(display, gc, BlackPixel(display, screen));
    XDrawRectangles(display, pixmap, gc, rects.data(), rects.size());

    // deal with lost page
    if (pkg.lost) {
        const unsigned int msgHeight = 60;
        int yMid = (GUI_WINDOW_WIDTH / 2) - (msgHeight / 2);
        XFillRectangle(display, pixmap, gc, x, yMid, GUI_PLAYER_WINDOW_WIDTH,
                       msgHeight);
        XSetFont(display, gc, bigFont->fid);
        XSetForeground(display, gc, WhitePixel(display, screen));
        const std::string msg = "You Lost!";
        // very bad approximation
        XDrawString(display, pixmap, gc, x + 85, yMid + 48, msg.c_str(),
                    msg.length());
        XSetFont(display, gc, font->fid);
    }
}

void GuiRenderer::render(const RenderPackage& p1, const RenderPackage& p2) {
    if (p1 != cacheP1) {
        clearHalfWindow(p1, true, cacheP1);
        renderHalf(p1, true, cacheP1);
        cacheP1 = p1;
    }

    if (p2 != cacheP2) {
        clearHalfWindow(p2, false, cacheP2);
        renderHalf(p2, false, cacheP2);
        cacheP2 = p2;
    }

    renderSplitLine();
    XCopyArea(display, pixmap, window, gc, 0, 0, GUI_WINDOW_WIDTH,
              GUI_WINDOW_HEIGHT, 0, 0);
    XFlush(display);

    if (p1.lost || p2.lost) {
        sleep(2);
    }
}

void TuiRenderer::render(const RenderPackage& p1, const RenderPackage& p2) {
    // Header lines: level / score / highscore
    auto printHeader = [](const RenderPackage& p) {
        auto pad = [](int value, int width) {
            std::string s = std::to_string(value);
            if ((int)s.size() < width)
                s = std::string(width - s.size(), ' ') + s;
            return s;
        };
        std::cout << "Level:" << pad(p.level, 3) << " ";
        std::cout << "Score:" << pad(p.score, 6) << " ";
        std::cout << "High:" << pad(p.highscore, 6);
    };

    // print Level line and Score line separately for each half
    auto padToWidth = [](const std::string& s, int w) {
        if ((int)s.size() >= w) return s.substr(0, w);
        return s + std::string(w - s.size(), ' ');
    };
    auto printLevelLine = [&](const RenderPackage& a, const RenderPackage& b) {
        std::string L = "Level: " + std::to_string(a.level);
        std::string R = "Level: " + std::to_string(b.level);
        std::cout << padToWidth(L, 11) << "  " << padToWidth(R, 11) << '\n';
    };
    auto printScoreLine = [&](const RenderPackage& a, const RenderPackage& b) {
        std::string L = "Score: " + std::to_string(a.score);
        std::string R = "Score: " + std::to_string(b.score);
        std::cout << padToWidth(L, 11) << "  " << padToWidth(R, 11) << '\n';
    };

    printLevelLine(p1, p2);
    printScoreLine(p1, p2);

    // print top dashed separator (centered across the 11 printed columns, with
    // one-space padding each side -> total 13)
    auto printTopBorder = [](void) { std::cout << std::string(11, '-'); };
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

    // build Next: preview for each half and print side-by-side aligned with
    // board (13 chars per half)
    auto buildNextVec = [](const RenderPackage& p) {
        const int halfDisplayWidth = 11;  // match board width
        std::vector<std::string> out;
        // First line: Next:
        std::string line = "Next:";
        if ((int)line.size() < halfDisplayWidth)
            line += std::string(halfDisplayWidth - line.size(), ' ');
        out.push_back(line);

        if (!p.nextBlock) {
            // no block: add a few empty lines
            for (int i = 0; i < 3; ++i)
                out.push_back(std::string(halfDisplayWidth, ' '));
            return out;
        }

        // compute bounding box of the block's coords
        auto coords = p.nextBlock->getCoords();
        char fill = p.nextBlock->getChar();
        int minR = INT_MAX, maxR = INT_MIN, minC = INT_MAX, maxC = INT_MIN;
        for (auto& pr : coords) {
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
                for (auto& pr : coords)
                    if (pr.first == r && pr.second == c) {
                        occupied = true;
                        break;
                    }
                s.push_back(occupied ? fill : ' ');
            }
            if ((int)s.size() < halfDisplayWidth)
                s += std::string(halfDisplayWidth - s.size(), ' ');
            out.push_back(s);
        }
        return out;
    };

    auto leftNext = buildNextVec(p1);
    auto rightNext = buildNextVec(p2);
    for (size_t i = 0; i < leftNext.size() || i < rightNext.size(); ++i) {
        std::string L =
            i < leftNext.size() ? leftNext[i] : std::string(13, ' ');
        std::string R =
            i < rightNext.size() ? rightNext[i] : std::string(13, ' ');
        std::cout << L << "  " << R << '\n';
    }

    std::cout << std::endl;

    // deal with lost player
    if (p1.lost) std::cout << "Player1 Lost!" << std::endl;
    if (p2.lost) std::cout << "Player2 Lost!" << std::endl;
}
