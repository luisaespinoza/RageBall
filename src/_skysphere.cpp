#include "_skysphere.h"

_skySphere::_skySphere()
{
    //ctor
    texLoader = new _textureLoader();
    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE); // Enable texturing
    gluQuadricNormals(quadric, GLU_SMOOTH); // Smooth normals
}
_skySphere::~_skySphere()
{
    //dtor
    gluDeleteQuadric(quadric);
    delete texLoader;
}

void _skySphere::initSkySphere(char* skyspherePath)
{
    texLoader->loadTexture(skyspherePath);

    position = {0.0f,0.0f,0.0f};
    rotation = {0.0f,0.0f,0.0f};
    scale = {1.0f,1.0f,1.0f};
    color = {1.0f,1.0f,1.0f};

    LOD = 25;
}

void _skySphere::drawSkySphere()
{
    glPushMatrix();
        glDisable(GL_LIGHTING); // Disable lighting for sky sphere
        glColor3f(color.r, color.g, color.b);             //set colors

        glTranslatef(position.x,position.y,position.z); //translation

        glRotatef(rotation.x,1,0,0);     //rotate around X-Axis
        glRotatef(rotation.y,0,1,0);     //rotate around Y-Axis
        glRotatef(rotation.z,0,0,1);     //rotate around Z-Axis

        glRotatef(90,1,0,0); // Align sphere texture properly

        glScalef(scale.x,scale.y,scale.z);  // Scale your model

        glDepthMask(GL_FALSE); // Disable depth writing -- we do this since skysphere is furthest object, everythig writes OVER it
        glEnable(GL_TEXTURE_2D); // Enable texturing -- 

        glBindTexture(GL_TEXTURE_2D, texLoader->textID);

        glFrontFace(GL_CW); // Invert front face for inside view -- we are INSIDE the sphere so we need to flip normals for textures
        gluSphere(quadric, 100.0, LOD, LOD); // Draw the sphere
        glFrontFace(GL_CCW); // Restore front face orientation

        glDisable(GL_TEXTURE_2D);
        glDepthMask(GL_TRUE); // Enable depth writing
        glEnable(GL_LIGHTING); // Enable lighting after drawing sky sphere
    glPopMatrix();
}