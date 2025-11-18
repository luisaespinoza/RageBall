#ifndef _SCENE_H
#define _SCENE_H

#include<_common.h>
#include<_light.h>
#include<_model.h>
#include<_inputs.h>
#include<_textureloader.h>
#include<_parallax.h>
#include<_skybox.h>
#include<_sprite.h>
#include<_timer.h>
#include<_3dmodelloader.h>
#include<_camera.h>
#include<_bullets.h>
#include<_collisioncheck.h>
#include<_level01.h>
enum class SceneMode {LandingPage, MainMenu, Gameplay, PauseOverlay };

struct MenuBlock {
    vec3  centerPosition;
    float collisionRadius;
    enum class Type { StartGame, Help, Quit, Resume, MainMenu } kind;
};
class _Scene : public _SceneInterface {
public:
    explicit _Scene(std::unique_ptr<ILevel> lvl);
    _Scene();
    ~_Scene();
    // lifecycle
    void onEnter() override;
    void onExit() override;
//    void onResize(int w, int h) override;

    //void reSizeScene(int width, int height);  // resize window rewritten in a modular way see: onResize() and applyProjection()
    void initGL();                            // initialize GL graphics
    // loop
    void update(double dt) override;
    void render() override;

    int  winMsg(HWND, UINT, WPARAM, LPARAM) override;
        void mouseMapping(int,int);
        double msX,msY,msZ;

    // allow manager to swap/assign a new level if desired
    void setLevel(std::unique_ptr<ILevel> lvl);

private:
    std::unique_ptr<ILevel> level;
    _camera camera;
    bool lightingEnabled = true;

    // small helpers
    void applyProjection(int w, int h);
    void applyFrameCameraAndLight();
};

//previous version before refactor
//class _Scene
//{
//    public:
//        _Scene();           //constructor
//        virtual ~_Scene();  //Destructor
//
//        int clickCnt =0;
//
//        void reSizeScene(int width, int height);  // resize window
//        void initGL();                            // initialize GL graphics
//        void drawScene();                         // render scene
//        int winMsg(HWND,UINT,WPARAM,LPARAM);      // to get keyboard interrupts and pass it to inputs
//        void mouseMapping(int,int);
//        double msX,msY,msZ;
//
//        int width, height;  // keep record of the screen size
//
//        void setMode(SceneMode newMode);
//    protected:
//
//    private:
//        SceneMode currentMode = SceneMode::MainMenu;  //Default = Menu
//
//        //Briefly considered pointers here...
//        //but, realistically, lifetime of these objects is tied to the scene itself. Would I really need TWO cameras in a single scene?
//
//        // --- Main Menu state ---
//        _camera         menuCamera;
//        _bullets        menuBullet;
//        _collisionCheck menuCollision;
//        _skyBox*         menuBackground=new _skyBox();
//        std::vector<MenuBlock> menuBlocks;
//
//        // --- Pause Overlay state ---
//        _camera         pauseCamera;
//        _bullets        pauseBullet;
//        _collisionCheck pauseCollision;
//        _skyBox*        pauseBackground=new _skyBox();
//        std::vector<MenuBlock> pauseBlocks;
//
//        // --- Gameplay state ---
//        _camera         gameplayCamera;
//        _bullets        gameplayBullet;
//        _collisionCheck gameplayCollision;
//        _skyBox*        gameplayBackground = new _skyBox();
//
//        //
//        _light globalLight;
//
//        void menuUpdate();
//        void menuRender();
//        void spawnBullet();
//        bool isAnyMenu() const;
//        bool shouldDrawDimOverlay() const;
//        void enterMainMenu();
//        void enterPause();
//
//        //Here pointers DO make sense. I have ONE object for each instance and a pointer that switches between
//        struct MenuContext
//        {
//            _camera*               camera;
//            _bullets*              bullet;
//            std::vector<MenuBlock>* blocks;
//            _collisionCheck*       collision;
//        };
//        MenuContext activeMenuContext();
//            static const char* menuBlockLabel(MenuBlock::Type t) {
//            switch (t) {
//                case MenuBlock::Type::StartGame: return "Start";
//                case MenuBlock::Type::Help:   return "Help";
//                case MenuBlock::Type::Quit:      return "Quit";
//                case MenuBlock::Type::Resume:    return "Resume";
//                case MenuBlock::Type::MainMenu:   return "Menu Menu";
//            default:                         return "";
//            }
//        }
//        static void drawBitmapText3D(const char* text, float x, float y, float z);
//
//};

#endif // _SCENE_H
