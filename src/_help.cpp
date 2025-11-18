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
void HelpScene::drawCenteredText(const char* text, float yNDC) {
    // Simple screen-space text using GLUT bitmap;
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, width_, 0, height_);
    glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();

    int textW = 8 * (int)strlen(text); // ~8px per char with HELVETICA_18
    float x = (width_ - textW) * 0.5f;
    float y = (height_ * 0.5f) + yNDC;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glRasterPos2f(x, y);
    for (const char* p = text; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);

    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
}

void HelpScene::render() {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    int W = vp[2] > 0 ? vp[2] : 800;
    int H = vp[3] > 0 ? vp[3] : 600;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, W, 0, H);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    const char* lines[] = {
        "HELP SCREEN",
        "-------------",
        "Main Menu",
        "Click a cube to select an action.",
        "Press ESC to open the in-game menu.",
        "Press any key or click to go back.",
        "Game",
        "Use the W,A,S,D keys to control your character.",
        "Try to reach the end of the level without hitting obstacles.",
        "Five collisions starts the level over.",
        "Use ESC key anytime to bring up the in-game menu."

    };

    glColor3f(1,1,1);
    float y = H * 0.6f;
    for (auto s : lines) {
        glRasterPos2f((W - 9 * (int)std::strlen(s)) * 0.5f, y);
        for (const char* p = s; *p; ++p)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
        y -= 30.f;
    }

    glPopMatrix(); glMatrixMode(GL_PROJECTION);
    glPopMatrix(); glMatrixMode(GL_MODELVIEW);
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

