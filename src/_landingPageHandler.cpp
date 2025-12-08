#include "_landingPageHandler.h"

_landingPageHandler::_landingPageHandler()
{
    //ctor
    LandingPage = new _landingPage();
    myTex = _textureloader();
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

    LandingPage->W = ScreenW;
    LandingPage->H = ScreenH;

    LandingPage->orthoStart();
      LandingPage->drawLandingMenu(LandingTex, ScreenW, ScreenH);
      LandingPage->draw();
    LandingPage->orthoEnd();
}

int _landingPageHandler::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_KEYUP:
        break;

        case WM_LBUTTONDOWN:
            {
                if(!LandingPage || !manager) return 0;

                int mx = LOWORD(lParam), my = HIWORD(lParam);
                int myInv = LandingPage->H - my; //Helps to match the windows mouse Y to openGl's Y
                    if(LandingPage->hit(LandingPage->btnEnter, mx, myInv))
                    {
                        LandingPage->isLanding = false;
                        LandingPage->start = true;
                        manager->startManager = true;

                        return 0;
                    }
                    return 0; //if the play doesn't click the button and on somewhere else.
            }
        break;

        case WM_RBUTTONDOWN:
        break;
    }
    return 0;
}
