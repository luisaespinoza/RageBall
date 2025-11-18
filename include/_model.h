#ifndef _MODEL_H
#define _MODEL_H

#include<_common.h>

class _model
{
    public:
        _model();
        virtual ~_model();

        double rotateX;
        double rotateY;
        double rotateZ;

        double posX;
        double posY;
        double posZ;

        double scale;

        vec3 p;

        void drawModel();
 /*

 for testing. Not a final production solution

 */
// Reusable teapot instance (no per-frame realloc)
static _model& TeapotModel() {
    static _model m;
    // Keep the model at origin; hallway handles placement
    m.posX = 0.f; m.posY = 0.f; m.posZ = 0.f;
    m.rotateX = m.rotateY = m.rotateZ = 0.f;
    return m;
}

// Obstacle draw function compatible with your addObstacleLocal(..., drawFn)
void DrawTeapot() {
    auto& m = TeapotModel();
    m.scale = 0.5f;            // pick a size that fits the hallway
    m.drawModel();             // calls glutSolidTeapot(...) internally
}
    protected:

    private:
};

#endif // _MODEL_H
