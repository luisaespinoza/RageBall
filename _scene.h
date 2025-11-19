#ifndef _SCENE_H
#define _SCENE_H

//Basics
#include <_common.h>
#include <_light.h>
#include <_skybox.h>
#include <_parallax.h>
#include <_textureloader.h>
#include <_collisioncheck.h>
#include <_timer.h>
#include <_groundParallax.h>
#include <randomNumGen.h>

//Models
#include <_sprite.h>
#include <_3dmodelloader.h>
#include <_model.h>

//Controls
#include <_inputs.h>
#include <_camera.h>
#include <_bullets.h>

//Front game
#include <_landingPage.h>
#include <_menu.h>
#include <_helpMenu.h>
#include <_pauseMenu.h>

//Falling objects
#include <_fallingModel.h>
#include <_fallingObj.h>
#include <windows.h>	// Header File For Windows


enum SceneStage {LANDING, MENU, HELP_MENU, START_GAME, PAUSE_GAME};
class _scene
{
    public:
        _scene();           //constructor
        virtual ~_scene();  //Destructor

        //Basics
        _textureloader *myTexture = new _textureloader();
        _timer *myTime = new _timer();
        _skybox *mySkyBox = new _skybox();
        _collisioncheck *myCol = new _collisioncheck();
        _groundParallax *ground = new _groundParallax();
        randomNumGen *randomNum = new randomNumGen();

        //Game Front
        _landingPage *landing = new _landingPage();
        _menu *menuScreen = new _menu();
        _helpMenu *help = new _helpMenu();
        _pauseMenu *pause = new _pauseMenu();
        SceneStage currentStage = LANDING;

        //models
        _3dmodelloader *mdl3D = new _3dmodelloader();
        _3dmodelloader *mdl3DW = new _3dmodelloader();
        _light *myLight = new _light();

        //Controls
        _camera *myCam = new _camera();
        _inputs *myInputs = new _inputs();
        _bullets bullet[10];

        //falling objects
        _fallingModel *fallingMdl = new _fallingModel();
        _fallingObj *fallingObj = new _fallingObj();

        int clickCnt =0;
        int W, H;

        GLuint texLandingPage = 0;
        GLuint texMenu = 0;
        GLuint texHelp = 0;

        //Functions for screen
        void reSizeScene(int width, int height);  // resize window
        void initGL();                            // initialize GL graphics
        void drawScene();                         // render scene
        int winMsg(HWND,UINT,WPARAM,LPARAM);      // to get keyboard interrupts and pass it to inputs
        void mouseMapping(int,int);
        double msX,msY,msZ;
        void drawFullscreenTex(GLuint, int, int);  //image loader for full screen

        void orthoStart();
        void orthoEnd();

        int width, height;  // keep record of the screen size
    protected:

    private:
};

#endif // _SCENE_H
