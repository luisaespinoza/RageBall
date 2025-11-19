#include "_landingPage.h"

_landingPage::_landingPage(int width, int height)
{
    //ctor
    W = width;
    H = height;
    start = false;

    btnEnter = {W/2 - 300, 200, 240, 60};
}

_landingPage::~_landingPage()
{
    //dtor
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
    glColor4f(1.0f, 1.0f, 1.0f, 0.0f);  //alpha = 0 -> transparent
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
