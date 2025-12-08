#include "_landingPageHandler.h"
#include "_textureloader.h"
_landingPageHandler::_landingPageHandler()
{
    //ctor
    LandingPage = new _landingPage();
    myTex = new _textureLoader();
    manager = nullptr; //instantiated in Main.cpp
}

_landingPageHandler::~_landingPageHandler()
{
    //dtor
    delete LandingPage;
    delete myTex;
}

void _landingPageHandler::loadLandingPage(int ScreenW, int ScreenH)
{
    if(!LandingPage) return;

    if(LandingTex == 0)
    {
        LandingTex = myTex->loadTexture("images/landing.jpg");
    }

    // Get the real GL viewport size
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    const int vw = vp[2];
    const int vh = vp[3];

    LandingPage->W = vw;
    LandingPage->H = vh;

    LandingPage->orthoStart();
    LandingPage->drawLandingMenu(LandingTex, vw, vh);
    LandingPage->draw();
    LandingPage->orthoEnd();
    // LandingPage->W = ScreenW;
    // LandingPage->H = ScreenH;
    // LandingPage->updateLayout();;
    // LandingPage->orthoStart();
    //   LandingPage->drawLandingMenu(LandingTex, ScreenW, ScreenH);
    //   LandingPage->draw();
    // LandingPage->orthoEnd();
}

int _landingPageHandler::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_LBUTTONDOWN:
        {
            if (!LandingPage || !manager) return 0;

            int mx = GET_X_LPARAM(lParam);
            int my = GET_Y_LPARAM(lParam);

            // Use the same viewport the landing page draws into
            GLint vp[4];
            glGetIntegerv(GL_VIEWPORT, vp);
            int vh = vp[3];

            // Win32 (0,0 top-left) -> GL (0,0 bottom-left)
            int myGL = vh - my - 1;

            if (LandingPage->hit(LandingPage->btnEnter, mx, myGL))
            {
                LandingPage->isLanding = false;
                LandingPage->start     = true;
                manager->startManager  = true;
            }
            return 0;
        }

        default:
            break;
    }
    return 0;
}
