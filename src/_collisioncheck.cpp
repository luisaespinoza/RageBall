#include "_collisioncheck.h"

_collisionCheck::_collisionCheck()
{
    //ctor
}

_collisionCheck::~_collisionCheck()
{
    //dtor
}

bool _collisionCheck::isRadialCol(vec2 p1, vec2 p2, float r1, float r2, float thrhld)
{
   return(sqrt(pow((p1.x-p2.x),2)+pow((p1.y-p2.y),2))-(r1+r2)< thrhld);

}

bool _collisionCheck::isSphereCol(vec3 p1, vec3 p2, float r1, float r2, float thrhld)
{
  return(sqrt(pow((p1.x-p2.x),2)+pow((p1.y-p2.y),2)+pow((p1.z-p2.z),2))-(r1+r2)< thrhld);
}

bool _collisionCheck::isLinearCol(vec3 a, vec3 b)
{
    const float eps = 1e-4f;
    const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
    return (dx*dx + dy*dy + dz*dz) <= eps*eps;
}

bool _collisionCheck::isPlanoCol(vec2 a, vec2 b)
{
    const float eps = 1e-4f;
    const float dx = a.x - b.x, dy = a.y - b.y;
    return (dx*dx + dy*dy) <= eps*eps;
}

bool _collisionCheck::isCubicCol(vec3 a, vec3 b)
{
    const float eps = 1e-4f;
    const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
    return (dx*dx + dy*dy + dz*dz) <= eps*eps;
}
