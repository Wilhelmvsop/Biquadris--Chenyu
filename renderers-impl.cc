module;

#include <X11/Xlib.h>

module Renderers;

import <string>;

const int GUI_WINDOW_X = 100;
const int GUI_WINDOW_Y = 100;
const unsigned int GUI_WINDOW_WIDTH = 800;
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
    XFlush(display);
    // might draw line here

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

    XFlush(display);
}

void GuiRenderer::render(const RenderPackage& p1, const RenderPackage& p2) {
    clearWindow();
    renderHalf(p1, true);
    renderSplitLine();
    renderHalf(p2, false);
}
