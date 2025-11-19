#ifndef _LANDINGPAGE_H
#define _LANDINGPAGE_H

#include <_common.h>

struct buttonPage
{
    int x, y, w, h;
};

class _landingPage
{
    public:
        _landingPage(int width = 800, int height = 600);
        virtual ~_landingPage();

        int W, H;
        buttonPage btnEnter;
        bool start;

        void drawText(int, int, char*);
        void drawButton(const buttonPage&);
        bool hit(const buttonPage&, int, int);
        void draw();


    protected:

    private:
};

#endif // _LANDINGPAGE_H
