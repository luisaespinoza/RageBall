#ifndef _LANDINGPAGEHANDLER_H
#define _LANDINGPAGEHANDLER_H

#include <_landingPage.h>
#include <_textureloader.h>
#include <windows.h>
#include <_sceneManager.h>

class _landingPageHandler
{
    public:
        _landingPageHandler();
        virtual ~_landingPageHandler();


    //Landing Page Testing Site
    _landingPage *LandingPage = new _landingPage();
    _textureloader myTex;
    _sceneManager *manager = new _sceneManager();
    GLuint LandingTex = 0;
    void loadLandingPage(int, int);
    int winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    protected:

    private:
};

#endif // _LANDINGPAGEHANDLER_H
