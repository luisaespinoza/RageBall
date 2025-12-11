#include "_gameOverHandler.h"
#define _TRUNCATE ((size_t)-1)

void _gameOverHandler::onEnter()
{
    overScene.W = width_;
    overScene.H = height_;

    overScene.drawTextureBackground("images/game_over.jpg");

}

void _gameOverHandler::render() //credit to Lily_Y for the help menu code
{
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    overScene.W = vp[2]; // width
    overScene.H = vp[3]; // height

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); //saves 3D state
    glLoadIdentity();
    gluOrtho2D(0, overScene.W, 0, overScene.H); //2D view now using real size

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); //saves camera state
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    overScene.drawGameOver();
    overScene.draw();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void _gameOverHandler::recordKey(WPARAM wParam) {
    char buf[64];
std::snprintf(buf, sizeof(buf), "Key VK_%u", static_cast<unsigned>(wParam));

    lastInput_ = buf;
}

void _gameOverHandler::recordMouse(int x, int y) {
    char buf[64];
std::snprintf(buf, sizeof(buf), "Mouse %d, %d", x, y);

    lastInput_ = buf;
}

int _gameOverHandler::winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_LBUTTONDOWN:
            {
                int mx = LOWORD(lParam), my = HIWORD(lParam);
                int myInv = overScene.H - my; //Helps to match the windows mouse Y to openGl's Y

                //Restart
                if(overScene.hit(overScene.btnRestart, mx, myInv))
                {
                    if(onRestart_) onRestart_();
                }

                //MainMenu
                if(overScene.hit(overScene.btnMainMenu, mx, myInv))
                {
                    if(onMainMenu_) onMainMenu_();
                }

                //Quit
                if(overScene.hit(overScene.btnQuit, mx, myInv))
                {
                    if(onQuit_) onQuit_();
                }

                return 0;
                break;
            }
        case WM_MOUSEMOVE:
            return 0;
            break;
        case WM_KEYDOWN:
            return 0;
            break;
        case WM_SYSKEYDOWN:
            return 0;
            break;
        default:
            return 0;
    }
}
