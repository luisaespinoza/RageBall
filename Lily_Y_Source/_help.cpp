#define _TRUNCATE ((size_t)-1)
#include "_help.h"

#include "_helpMenu.h"
#include "_landingPage.h"

_helpMenu helpMenuTex;
_landingPage landingHelper;
/*
_help::_help()
{
    //ctor
}

_help::~_help()
{
    //dtor
}
*/

void _help::onEnter()
{
    cout << "Hit button" << endl;
    helpMenuTex.W = width_;
    helpMenuTex.H = height_;

    helpMenuTex.drawTextureBackground("images/help.jpg");

}

void _help::render()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); //saves 3D state
      glLoadIdentity();
      gluOrtho2D(0,helpMenuTex.W, 0, helpMenuTex.H); //2D view
      glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); //saves camera state
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

     helpMenuTex.drawHelpMenu();
     helpMenuTex.draw();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}


void _help::recordKey(WPARAM wParam) {
    char buf[64];
std::snprintf(buf, sizeof(buf), "Key VK_%u", static_cast<unsigned>(wParam));

    lastInput_ = buf;
}

void _help::recordMouse(int x, int y) {
    char buf[64];
std::snprintf(buf, sizeof(buf), "Mouse %d, %d", x, y);

    lastInput_ = buf;
}

int _help::winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_LBUTTONDOWN:
            recordMouse(LOWORD(lParam), HIWORD(lParam));
            if (onClose_) onClose_();      // leave Help immediately
            return 0;
        case WM_MOUSEMOVE:
            recordMouse(LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            recordKey(wParam);
            if (onClose_) onClose_();      // leave Help immediately
            return 0;
        default:
            return 0;
    }
}

