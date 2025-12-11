#include "_gameOverScene.h"

_gameOverScene::_gameOverScene(int width, int height)
{
  //ctor
    W = width;
    H = height;
    // Restart Button
    restart.type = GAMEOVER_RESTART;
    restart.original_position = {105,285};
    restart.original_dimensions = {485,120}; 
    buttons.push_back(restart);
    // Main Menu Button
    mainMenu.type = GAMEOVER_MAINMENU;
    mainMenu.original_position = {105,460};
    mainMenu.original_dimensions = {485,120};
    buttons.push_back(mainMenu);
    // Quit Button
    quit.type = GAMEOVER_QUIT;
    quit.original_position = {105,630};
    quit.original_dimensions = {485,120};
    buttons.push_back(quit);
    
    myTex = new _textureLoader();
}

_gameOverScene::~_gameOverScene()
{
    //dtor
}

void _gameOverScene::drawText(int x, int y, char* s)
{
    glRasterPos2i(x, y);
    for(char* p = s; *p; p++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
    }
}

void _gameOverScene::drawTextureBackground(const char* path)
{
    if(backgroundTex != 0)
        return;
    backgroundTex = myTex->loadTexture(const_cast<char*> (path));
}

GameOverButton _gameOverScene::hit(int mouseX, int mouseY)
{
    //mouseY = H - mouseY; //Invert Y to match OpenGL coordinates
    if (enableDebugging) { cout << "Mouse Click at: " << mouseX << ", " << mouseY << endl; }
    for (int i = 0; i < buttons.size(); i++) {
        button& btn = buttons[i];
        if (mouseX >= btn.adjusted_position.x &&
            mouseX <= btn.adjusted_position.x + btn.adjusted_dimensions.x &&
            mouseY >= btn.adjusted_position.y &&
            mouseY <= btn.adjusted_position.y + btn.adjusted_dimensions.y) {
            if (enableDebugging) { cout << "Button Hit: " << btn.type << endl; }
            return btn.type;
        }
    }
    if (enableDebugging) { cout << "No Button Hit" << endl; }
    return GAMEOVER_NONE;
}

void _gameOverScene::setDimensions(int width, int height)
{
    if (enableDebugging) {
        cout << "Setting Game Over Scene Dimensions to: " << width << "x" << height << endl;
        cout << "Original Screen Size: " << W << "x" << H << endl;
    }
    W = width;
    H = height;
    float scaleX = W / original_canvas_size.x;
    float scaleY = H / original_canvas_size.y;
    if (enableDebugging) {
        cout << "Scale Factors - X: " << scaleX << ", Y: " << scaleY << endl;
    }
    for (int i = 0; i < buttons.size(); i++) {
        button& btn = buttons[i];
        btn.adjusted_position.x = btn.original_position.x * scaleX;
        btn.adjusted_position.y = btn.original_position.y * scaleY;
        btn.adjusted_dimensions.x = btn.original_dimensions.x * scaleX;
        btn.adjusted_dimensions.y = btn.original_dimensions.y * scaleY;
    }
}

void _gameOverScene::drawGameOver()
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
