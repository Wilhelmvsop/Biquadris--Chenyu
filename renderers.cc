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

export class GuiRenderer : public Renderer {
    inline static const std::unordered_map<char, unsigned long> colorMap = {
        {'I', 0x00E5E5},  // Cyan
        {'J', 0x4169E1},  // Blue
        {'L', 0xFF8C00},  // Orange
        {'O', 0xFFD700},  // Gold
        {'S', 0x32CD32},  // Green
        {'Z', 0xFF3333},  // Red
        {'T', 0x9370DB},  // Purple
        {'*', 0xD2691E},  // brown
    };

    Display* display;
    Window window;
    int screen;
    GC gc;
    XFontStruct* bigFont;
    XFontStruct* normalFont;

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
