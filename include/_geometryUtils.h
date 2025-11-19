#ifndef _GEOMETRYUTILS_H
#define _GEOMETRYUTILS_H
#include <_common.h>
struct Pose //Position + Orientation
{
    vec3  p;        // position
    float yawDeg;   // heading around Y (right-handed)
};
struct Obstacle {
    Pose local;
    float radius;
    void (*drawFn)();
};
#endif // _GEOMETRYUTILS_H
