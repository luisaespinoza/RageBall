#include "_helpMenu.h"
#include "_textureloader.h"
_helpMenu::_helpMenu(int width, int height)
{
     //ctor
    W = width;
    H = height;
    btnBack = {W/2 - 300, 370, 225, 60};
}

_helpMenu::~_helpMenu()
{
    //dtor
}

void _helpMenu::drawText(int x, int y, char* s)
{
    glRasterPos2i(x, y);
    for(char* p = s; *p; p++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
    }
}

void _helpMenu::drawTextureBackground(const char* path)
{
    if(backgroundTex != 0)
        return;

    backgroundTex = myTex.loadTexture(const_cast<char*> (path));
}

void _helpMenu::drawButton(const buttonHelp& r)
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
    glColor3f(0, 0, 0);
    drawText(r.x+(r.w/2 - (int)strlen(label) * 5),
             r.y+r.h/2 + 5, label);
    glColor3f(1,1,1);
    */
    glDisable(GL_BLEND);

}

bool _helpMenu::hit(const buttonHelp& r, int mx, int my)
{
    return mx >= r.x && mx<= r.x + r.w &&
           my >= r.y && my <= r.y + r.h;
}

void _helpMenu::draw()
{

    glColor3f(1,1,1);
    //drawText(W/2-60, H -120, "Help");
/*
    //Shows text on screen
    glColor3f(1, 1, 1);
    drawText(80, H-180, "WASD: MOVE");
    drawText(80, H-210, "Mouse: LOOK");
    drawText(80, H-240, "Left Click: Shoot");
    drawText(80, H-270, "ESC: Pause");
*/
    //Back used to go to main menu;
    glColor3f(0.9f, 0.9f, 0.95f);
    drawButton(btnBack);
}

void _helpMenu::drawHelpMenu()
{
    if(backgroundTex == 0)
        return;

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTex);

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
}
