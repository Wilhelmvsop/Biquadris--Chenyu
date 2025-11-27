module;

#include <X11/Xlib.h>

export module Renderers;

import Blocks;
import <unordered_map>;
import <memory>;
import <vector>;

export struct RenderPackage {
    int score;
    int highscore;
    std::vector<std::vector<char>> pixels;
    int level;
    std::shared_ptr<Block> nextBlock;
    bool lost;
};

export class Renderer {
   public:
    virtual void render(const RenderPackage& p1, const RenderPackage& p2) = 0;
    virtual ~Renderer() = default;
};

// I: blue
// J: dark blue
// L: orange
// O: yellow
// S: lemon
// Z: red
// T: purple
// *: brown
export class GuiRenderer : public Renderer {
    inline static const std::unordered_map<char, unsigned long> colorMap = {
        {'I', 0x00F0F0},  // Cyan
        {'J', 0x0000F0},  // Blue
        {'L', 0xF0A000},  // Orange
        {'O', 0xF0F000},  // Yellow
        {'S', 0x00F000},  // Green
        {'Z', 0xF00000},  // Red
        {'T', 0xA000F0},  // Purple
        {'*', 0x8B4513}};

    Display* display;
    Window window;
    int screen;
    GC gc;

    void clearWindow();
    void renderSplitLine();
    void renderHalf(const RenderPackage& pkg, bool left);

   public:
    // Ctor will prepare everything we need for rendering and create
    // a window for the first time.
    GuiRenderer();
    ~GuiRenderer() override;
    void render(const RenderPackage& p1, const RenderPackage& p2) override;
};

// Text-based (terminal) renderer
export class TuiRenderer : public Renderer {
   public:
    TuiRenderer() = default;
    ~TuiRenderer() override = default;
    void render(const RenderPackage& p1, const RenderPackage& p2) override;
};
