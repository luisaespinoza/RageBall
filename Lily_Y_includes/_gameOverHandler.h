#ifndef _GAMEOVERHANDLER_H
#define _GAMEOVERHANDLER_H

#include <_common.h>
#include <_gameOverScene.h>

class _gameOverHandler : public _SceneInterface
{
    public:
    using GameOverCallback = std::function<void()>;

     _gameOverHandler(GameOverCallback onRestart,
                     GameOverCallback onMainMenu,
                     GameOverCallback onQuit)
        : onRestart_(std::move(onRestart)),
          onMainMenu_(std::move(onMainMenu)),
          onQuit_(std::move(onQuit))
    {}

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

    GameOverCallback onRestart_;
    GameOverCallback onMainMenu_;
    GameOverCallback onQuit_;

    _gameOverScene overScene;

    // void drawCenteredText(const char* text, float yNDC);
    void recordKey(WPARAM wParam);
    void recordMouse(int x, int y);protected:
};

#endif // _GAMEOVERHANDLER_H
