#include "_landingPage.h"
namespace {
    // “design resolution” used for the button math
    constexpr int BASE_W = 3840;
    constexpr int BASE_H = 2160;

    // button rect in that base resolutio
    // constexpr buttonPage BASE_BTN_ENTER = { 70, 332, 325, 78 };
    constexpr buttonPage BASE_BTN_ENTER = { 89, 525, 482, 114 };
}
_landingPage::_landingPage(int width, int height)
{
     //ctor
    W = width;
    H = height;
    start = false;
    isLanding = true;
    // myTex = _textureLoader();
    btnEnter = BASE_BTN_ENTER;
}

_landingPage::~_landingPage()
{
    //dtor
    // delete myTex;
    // myTex = nullptr;
}

void _landingPage::drawText(int x, int y, char* s)
{
    glRasterPos2i(x, y);
    for(char* p = s; *p; p++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
    }

}

bool _landingPage::hit(const buttonPage& r, int mx, int my)
{
    return mx >= r.x && mx<= r.x + r.w &&
           my >= r.y && my <= r.y + r.h;
}

void _landingPage::drawButton(const buttonPage& r)
{
    //Make transparent
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //This draws the box
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  //alpha = 0 -> transparent
    glBegin(GL_QUADS);
      glVertex2i(r.x, r.y);
      glVertex2i(r.x+r.w, r.y);
      glVertex2i(r.x+r.w, r.y+r.h);
      glVertex2i(r.x, r.y+r.h);
    glEnd();

    /*
    glColor3f(1, 1, 1);
    drawText(r.x+(r.w/2 - (int)strlen(label) * 5),
             r.y+r.h/2 + 5, label);
    glColor3f(1,1,1);
    */
    glDisable(GL_BLEND);

}

void _landingPage::draw()
{

    glColor3f(1,1,1);
    //drawText(W/2-60, H -120, "Game Title");

    glColor3f(0.9f, 0.9f, 0.95f);
    drawButton(btnEnter);
}

void _landingPage::drawLandingMenu(GLuint tex, int width, int height)
{
   glBindTexture(GL_TEXTURE_2D, tex);

    //Does the full screen quads on image
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);  //bottom left
      glVertex2i(0, 0);
      glTexCoord2f(1.0f, 1.0f); //bottom right
      glVertex2i(W, 0);
      glTexCoord2f(1.0f, 0.0f); //top right
      glVertex2i(W, H);
      glTexCoord2f(0.0f, 0.0f); //top left
      glVertex2i(0, H);
    glEnd();

    //Enables textures and avoids texture bleed.
    glBindTexture(GL_TEXTURE_2D, 0);
}

void _landingPage::orthoStart()
{
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); //saves 3D state
      glLoadIdentity();
      gluOrtho2D(0,W, 0, H); //2D view
      glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); //saves camera state
    glLoadIdentity();

}

void _landingPage::orthoEnd()
{
    // glPopMatrix();
    // glMatrixMode(GL_PROJECTION);
    // glPopMatrix();
    // glMatrixMode(GL_MODELVIEW);
    // glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void _landingPage::updateLayout()
{
    //Update button positions based on window size
    float scaleX = static_cast<float>(W) / static_cast<float>(BASE_W);
    float scaleY = static_cast<float>(H) / static_cast<float>(BASE_H);

    btnEnter.x = static_cast<int>(BASE_BTN_ENTER.x * scaleX);
    btnEnter.y = static_cast<int>(BASE_BTN_ENTER.y * scaleY);
    btnEnter.w = static_cast<int>(BASE_BTN_ENTER.w * scaleX);
    btnEnter.h = static_cast<int>(BASE_BTN_ENTER.h * scaleY);
}

