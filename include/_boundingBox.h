#ifndef _BOUND_BOX_H
#define _BOUND_BOX_H

#include<_common.h>

// Used by models to check for bounds collision
class _boundingBox
{
    public:
        _boundingBox();
        virtual ~_boundingBox();

        enum collisionType {NONE, POS_X, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z};

        vec3f dimensions; // size of box in x,y,z directions
        vec3f position;   // center position of box in world space
        vec3f scale;      // scale of box in x,y,z directions (used for scaling of model)
        vec3f positionOffset; // offset from model position
        
        void initBoundingBox(vec3f dim, vec3f pos, vec3f scale);
        // Penetration Depth collision detection (allows for directionality)
        collisionType checkCollision(const _boundingBox& otherBox);
        // AABB (T/F) collision detection
        bool isColliding(const _boundingBox& otherBox);
        void displayBoundingBox(); // shows box for debugging

    protected:

    private:
};

#endif // _BOUND_BOX_H