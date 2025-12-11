#define _TRUNCATE ((size_t)-1)

#include "_help.h"

//_help::_help()
//{
//    //ctor
//}
//
//_help::~_help()
//{
//    //dtor
//}
// void HelpScene::drawCenteredText(const char* text, float yNDC) {
//     // Simple screen-space text using GLUT bitmap;
//     glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//     gluOrtho2D(0, width_, 0, height_);
//     glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();
//
//     int textW = 8 * (int)strlen(text); // ~8px per char with HELVETICA_18
//     float x = (width_ - textW) * 0.5f;
//     float y = (height_ * 0.5f) + yNDC;
//
//     glDisable(GL_LIGHTING);
//     glDisable(GL_TEXTURE_2D);
//     glRasterPos2f(x, y);
//     for (const char* p = text; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
//
//     glMatrixMode(GL_MODELVIEW);  glPopMatrix();
//     glMatrixMode(GL_PROJECTION); glPopMatrix();
// }
void HelpScene::onEnter()
{
    cout << "Hit button" << endl;
    helpMenuTex.W = width_;
    helpMenuTex.H = height_;

    helpMenuTex.drawTextureBackground("images/help.jpg");

}

void HelpScene::render() //credit to Lily_Y for the help menu code
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

void HelpScene::recordKey(WPARAM wParam) {
    char buf[64];
std::snprintf(buf, sizeof(buf), "Key VK_%u", static_cast<unsigned>(wParam));

    lastInput_ = buf;
}

void HelpScene::recordMouse(int x, int y) {
    char buf[64];
std::snprintf(buf, sizeof(buf), "Mouse %d, %d", x, y);

    lastInput_ = buf;
}

int HelpScene::winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

