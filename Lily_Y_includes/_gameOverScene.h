#ifndef _GAMEOVERSCENE_H
#define _GAMEOVERSCENE_H

#include <_common.h>
#include <_textureloader.h>

struct buttonGameOver
{
    int x, y, w, h;
};

class _gameOverScene
{
    public:
        _gameOverScene(int width = 800, int height = 600);
        virtual ~_gameOverScene();

        int W, H;
        buttonGameOver btnRestart;
        buttonGameOver btnMainMenu;
        buttonGameOver btnQuit;

        void drawText(int, int, char*);
        void drawButton(const buttonGameOver&);
        bool hit(const buttonGameOver&, int, int);
        void draw();
        void drawTextureBackground(const char* path);
        void drawGameOver();
    protected:

    private:
        _textureLoader* myTex;
        GLuint backgroundTex = 0;
};

#endif // _GAMEOVERSCENE_H
