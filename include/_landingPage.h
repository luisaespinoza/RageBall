#ifndef _LANDINGPAGE_H
#define _LANDINGPAGE_H

#include <_common.h>
// #include <_textureloader.h>
#include <windows.h>
#include "_textureloader.h"

struct buttonPage
{
    int x, y, w, h;
};

class _landingPage
{
    public:
        _landingPage(int width = 800, int height = 600);
        virtual ~_landingPage();

        _textureLoader myTex; //unused variable
        buttonPage btnEnter;
        GLuint texLandingPage = 0;

        int W, H;
        bool start;
        bool isLanding;
        void drawText(int, int, char*);
        void drawButton(const buttonPage&);
        bool hit(const buttonPage&, int, int);
        void draw();
        void drawLandingMenu(GLuint, int, int);
        void orthoStart();
        void orthoEnd();
        void updateLayou();
    protected:

    private:
};

#endif // _LANDINGPAGE_H
