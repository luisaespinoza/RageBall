#ifndef _SKYSPHERE_H
#define _SKYSPHERE_H

#include<_common.h>
#include<_textureloader.h>

class _skySphere
{
    public:
        _skySphere();
        virtual ~_skySphere();

        _textureLoader *texLoader;

        vec3f position;
        vec3f rotation;
        vec3f scale;
        col3f color;

        GLint LOD;

        void initSkySphere(char* skyspherePath); 
        void drawSkySphere();

    protected:

    private:
        GLUquadric* quadric;
};

#endif // _SKYSPHERE_H