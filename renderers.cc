module;

#include <X11/Xlib.h>

export module Renderers;

import Blocks;
import <unordered_map>;

export struct RenderPackage {
    int score;
    int highscore;
    // 18 x 11 array, use pointer instead to not copy everything
    char (*pixels)[11];
    int level;
    Block* nextBlock;
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
        {'I', 0x0000FF}, {'J', 0x00008B}, {'L', 0xFF8C00}, {'O', 0xFFFF00},
        {'S', 0xFFF44F}, {'Z', 0xFF0000}, {'T', 0x800080}, {'*', 0x8B4513},
    };

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
     void render(const RenderPackage &p1, const RenderPackage &p2) override;
};
