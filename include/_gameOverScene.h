#ifndef _GAMEOVERSCENE_H
#define _GAMEOVERSCENE_H

#include <_common.h>
#include <_textureloader.h>

/*
Holds button data for game over screen -- clicks etc
Position refers to min coordinate (top-left corner)
Dimensions refers to width and height
*/

enum GameOverButton { 
    GAMEOVER_NONE = 0, 
    GAMEOVER_RESTART = 1, 
    GAMEOVER_MAINMENU = 2, 
    GAMEOVER_QUIT = 3 
};

struct button {
    GameOverButton type;
    vec2f original_position;     // where is the button located on its original canvas
    vec2f original_dimensions;   // what is the size of the button on its original canvas
    vec2f adjusted_position;     // where is the button located on its original canvas
    vec2f adjusted_dimensions;   // what is the size of the button on its original canvas
};


class _gameOverScene
{
    public:
        _gameOverScene(int width = 1920, int height = 1080);
        virtual ~_gameOverScene();

        int W, H;

        vec2f original_canvas_size = {1920,1080};
        button restart;
        button mainMenu;
        button quit;
        vector<button> buttons;

        // Sets screen dimensions and adjusts button positions/sizes accordingly
        void setDimensions(int width, int height);
        void drawText(int, int, char*);
        GameOverButton hit(int mouseX, int mouseY);
        void drawTextureBackground(const char* path);
        void drawGameOver();
    protected:

    private:
        _textureLoader* myTex;
        GLuint backgroundTex = 0;
        bool enableDebugging = true;
};

#endif // _GAMEOVERSCENE_H
