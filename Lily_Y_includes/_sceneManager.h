#ifndef _SCENEMANAGER_H
#define _SCENEMANAGER_H
#include<_common.h>
#include<_light.h>
#include<_menuScene.h>
#include<_scene.h>
#include<_level00.h>
#include<_level01.h>
#include<_landingPage.h>
#include<_level02.h>
#include<_gameOverHandler.h>

using LevelFactory = std::function<std::unique_ptr<ILevel>()>;

class LevelRegistry {
public:
  static LevelRegistry& instance();

  void registerLevel(const std::string& id, LevelFactory f);
  std::unique_ptr<ILevel> create(const std::string& id);

private:
  std::unordered_map<std::string, LevelFactory> factories;
};

class LoadLevelScene : public _SceneInterface {
public:
    std::unique_ptr<ILevel> level;
    std::function<void(const std::string&)> loadLevelFn;

    explicit LoadLevelScene(std::unique_ptr<ILevel> lvl);
    LoadLevelScene(std::unique_ptr<ILevel> lvl,
                   std::function<void(const std::string&)> fn);
    // lifecycle
    void onEnter() override;
    void onExit() override;
      //  std::function<void(const std::string&)> loadLevelFn;
    // game loop
    void update(double dt) override;
    void render() override;
    int winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM /*lParam*/)override ;
    const char* name() const override {return "LoadLevelScene";}

private:
    //std::unique_ptr<ILevel> level;
};
//class MenuScene : public _SceneInterface {
//public:
//  using StartCallback = std::function<void(const std::string& levelId)>;
//
//  explicit MenuScene(StartCallback onStart) : onStartLevel(std::move(onStart)) {}
//  void render() override;                 // draws your UI blocks (reusing your code)
//  int  winMsg(HWND, UINT, WPARAM, LPARAM) override; // fire bullets / select items
//
//private:
//  StartCallback onStartLevel;
//};
class _sceneManager
{
    public:
        _sceneManager();
        virtual ~_sceneManager();

        void initlGL(); //global GL init. This is our top level.
        void applyPerspective(int w, int h); //same reasoning
        void setCurrentScene(std::unique_ptr<_SceneInterface> newScene);
        void pushScene(std::unique_ptr<_SceneInterface> newScene);
        void popScene();
        void showInGameMenu();
        void resumeFromInGameMenu();
        //load level by id(LevelRegistry -> LoadLevelScene)
        void setCurrentLevel(const std::string& levelId);

        void handleResizeEvent(int windowWidth, int windowHeight);
        void updateActiveScene(double dt);
        void renderActiveScene();
        int forwardWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
        void togglePauseOverlay();
        void clearAllScenes();
        void setCurrentSceneNoEnter(std::unique_ptr<_SceneInterface> newScene);
        void bootMainMenu(const std::string& firstLevelId = "level01");
        void showHelpOverlay();
        void showGameOverlay(string restartLevelId);
        _light globalLight;
        bool escWasDown_ = false;
        bool suppressEnterNext_ = false;
        bool isTopInGameMenu() const;
        bool startManager = false;
    protected:

    private:

        std::vector<std::unique_ptr<_SceneInterface>> sceneStack_;
        std::unique_ptr<_SceneInterface> preservedScene_;
        std::vector<std::function<void()>> pendingOps_;
        void post(std::function<void()> op) { pendingOps_.push_back(std::move(op)); }
};

#endif // _SCENEMANAGER_H
