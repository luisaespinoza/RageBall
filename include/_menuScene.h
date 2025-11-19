#ifndef _MENUSCENE_H
#define _MENUSCENE_H
#include <_common.h>
#include <_camera.h>
#include <_bullets.h>
#include<_skybox.h>
#include <_collisioncheck.h>
#include <_help.h>
#include <_3dmodelloader.h>
class _menuScene : public _SceneInterface {
public:
    enum class Mode { MainMenu, InGameMenu };

    using StartCallback     = std::function<void(const std::string& levelId)>;
    using ResumeCallback    = std::function<void()>;
    using MainMenuCallback  = std::function<void()>;
    using QuitCallback      = std::function<void()>;
    using HelpCallback = std::function<void()>;

    // Main menu constructor (needs Start + Help + Quit)
explicit _menuScene(StartCallback onStart,
                    HelpCallback onHelp,
                    QuitCallback onQuit,
                    const std::string& defaultLevelId = "level01");

    // Pause overlay constructor (TODO:Resume + MainMenu + Help + Quit)
explicit _menuScene(ResumeCallback onResume,
                    MainMenuCallback onGoMainMenu,
                    HelpCallback onHelp,
                    QuitCallback onQuit);
//    _menuScene() override = default;
    ~_menuScene() override = default;

    // lifecycle
    void onEnter() override;
    void onResize(int w, int h) override;
    //bool isPauseOverlay() const { return mode == Mode::PauseOverlay; }
    // loop
    void update(double dt) override;
    void render() override;

    // input
    int  winMsg(HWND, UINT, WPARAM, LPARAM) override;

    // mouse→world mapping (expects GL matrices to be current)
    void mouseMapping(int x, int y);
        Mode mode;
            void setMode(Mode m) { mode = m; }
    bool isInGameMenu() const { return mode == Mode::InGameMenu; }

private:
    // ----- shared menu data -----
      //  Mode mode = Mode::MainMenu;  // default
    struct MenuBlock {
        enum class Type { StartGame, Help, Quit, Resume, MainMenu };
        vec3  centerPosition{};
        float collisionRadius = 1.0f;
        Type  kind = Type::StartGame;
    };

    struct MenuContext {
        _camera*           camera   = nullptr;
        _bullets*          bullet   = nullptr;
        std::vector<MenuBlock>* blocks   = nullptr;
        _collisionCheck*   collision= nullptr;
    };

    // state

    std::string defaultLevelId;
    _3DModelLoader *menuModel = new _3DModelLoader();
    // callbacks
    StartCallback     onStartLevel;
    ResumeCallback    onResume;
    MainMenuCallback  onGoMainMenu;
    QuitCallback      onQuit;
    HelpCallback      onHelp;

    // per-mode sets
    _camera          menuCamera;
    _bullets         menuBullet;
    std::vector<MenuBlock> menuBlocks;
    _collisionCheck  menuCollision;

    _camera          pauseCamera;
    _bullets         pauseBullet;
    std::vector<MenuBlock> pauseBlocks;
    _collisionCheck  pauseCollision;

    // visuals
    _skyBox*         menuBackground = new _skyBox();
    // window + mouse
    int   width = 1, height = 1;
    double msX = 0.0, msY = 0.0, msZ = 0.0;

    // helpers
    void enterMainMenu();
    void enterPause();
    MenuContext activeMenuContext();
//    bool  shouldDrawDimOverlay() const { return mode == Mode::PauseOverlay; }
    const char* menuBlockLabel(MenuBlock::Type t) const;

    void menuUpdate(double dt);
    void menuRender();

    void spawnBullet();
    void handleBulletHits();

    // text helper
    void drawBitmapText3D(const char* text, float x, float y, float z);
};

//static bool isPauseOverlayOnTop(const std::vector<std::unique_ptr<_SceneInterface>>& stack) {
//    if (stack.empty()) return false;
//    if (auto ms = dynamic_cast<_menuScene*>(stack.back().get()))
//        return ms->isPauseOverlay();
//    return false;
//}

//static bool topIsPauseOverlay(const std::vector<std::unique_ptr<_SceneInterface>>& st) {
//    if (st.empty()) return false;
//    if (auto ms = dynamic_cast<_menuScene*>(st.back().get())) return ms->isPauseOverlay();
//    return false;
//}

#endif // _MENUSCENE_H
