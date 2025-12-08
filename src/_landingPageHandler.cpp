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

    LandingPage->W = ScreenW;
    LandingPage->H = ScreenH;
    LandingPage->updateLayout();;
    LandingPage->orthoStart();
      LandingPage->drawLandingMenu(LandingTex, ScreenW, ScreenH);
      LandingPage->draw();
    LandingPage->orthoEnd();
}

int _landingPageHandler::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_LBUTTONDOWN:
        {
            if (!LandingPage || !manager) return 0;

            int mx = LOWORD(lParam), my = HIWORD(lParam);
            int myInv = LandingPage->H - my;

            // --- DEBUG: measure button rect by 2 clicks ---
            static int clickStage = 0;
            static buttonPage debugRect;

            if (clickStage == 0) {
                // first click: top-left of desired button region
                debugRect.x = mx;
                debugRect.y = myInv;
                clickStage = 1;
                std::cout << "[CALIB] top-left = (" << mx << ", " << myInv << ")\n";
            } else if (clickStage == 1) {
                // second click: bottom-right of desired button region
                debugRect.w = mx - debugRect.x;
                debugRect.h = myInv - debugRect.y;
                clickStage = 2;
                std::cout << "[CALIB] bottom-right = (" << mx << ", " << myInv << ")\n";
                std::cout << "[CALIB] btnEnter = { "
                          << debugRect.x << ", "
                          << debugRect.y << ", "
                          << debugRect.w << ", "
                          << debugRect.h << " };\n";
            }

            // normal behavior:
            if (LandingPage->hit(LandingPage->btnEnter, mx, myInv))
            {
                LandingPage->isLanding = false;
                LandingPage->start     = true;
                manager->startManager  = true;
            }
            return 0;
        }

            // ... rest unchanged ...
    }
    return 0;
}
