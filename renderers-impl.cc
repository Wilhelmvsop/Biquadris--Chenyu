module Renderers;

#include <X11/Xlib.h>

import <string>;

const int GUI_WINDOW_X = 100;
const int GUI_WINDOW_Y = 100;
const unsigned int GUI_WINDOW_WIDTH = 500;
const unsigned int GUI_WINDOW_HEIGHT = 900;
const std::string GUI_WINDOW_NAME = "Biquardris++";

GuiRenderer::GuiRenderer() {
    display = XOpenDisplay(NULL);
    if (!display) throw "Couldn't open X display";

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 GUI_WINDOW_Y, GUI_WINDOW_X, GUI_WINDOW_WIDTH,
                                 GUI_WINDOW_HEIGHT, BlackPixel(display, screen),
                                 WhitePixel(display, screen));
    XStoreName(display, window, GUI_WINDOW_NAME);
    gc = XCreateGC(display, window, 0, NULL);

    XMapWindow(display, window);
    XFlush(display);
}

GuiRenderer::~GuiRenderer() {
    XFreeGC(gc);
    XCloseDisplay(display);
}

void GuiRenderer::render(const RenderPackage& p1, const RenderPackage& p2) {}
