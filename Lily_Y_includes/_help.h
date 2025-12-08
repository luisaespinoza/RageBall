#ifndef _HELP_H
#define _HELP_H

#include <_common.h>

class _help : public _SceneInterface
{
    public:
    using CloseCallback = std::function<void()>;

    explicit _help(CloseCallback onClose)
        : onClose_(std::move(onClose)) {}

    // lifecycle
    void onEnter() override;
    void onExit() override {}
    void onResize(int w, int h) override
    {
        width_ = (w>0)?w:1; height_ = (h>0)?h:1;
    }
    const char* name() const override { return "HelpScene"; }

    // loop
    void update(double /*dt*/) override {}
    void render() override;

    // input
    int winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
private:
    int  width_ = 1, height_ = 1;
    std::string lastInput_ = "(none)";
    CloseCallback onClose_;

    void recordKey(WPARAM wParam);
    void recordMouse(int x, int y);
};

#endif // _HELP_H
