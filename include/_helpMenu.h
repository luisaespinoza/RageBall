#ifndef _HELPMENU_H
#define _HELPMENU_H

#include <_common.h>
// #include <_textureloader.h>
class _textureLoader;
struct buttonHelp
{
    int x, y, w, h;
};

class _helpMenu
{
    public:
        _helpMenu(int width = 800, int height = 600);
        virtual ~_helpMenu();

        int W, H;
        buttonHelp btnBack;

        void drawText(int, int, char*);
        void drawButton(const buttonHelp&);
        bool hit(const buttonHelp&, int, int);
        void draw();
        void drawTextureBackground(const char* path);
        void drawHelpMenu();
    protected:

    private:
        // _textureloader myTex;
        GLuint backgroundTex = 0;
};

#endif // _HELPMENU_H
