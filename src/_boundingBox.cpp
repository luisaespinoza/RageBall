#include "_boundingBox.h"

_boundingBox::_boundingBox()
{
    //ctor
    dimensions = {0.0f,0.0f,0.0f};
    position = {0.0f,0.0f,0.0f};
    scale = {0.0f,0.0f,0.0f};
    positionOffset = {0.0f, 0.0f, 0.0f};
}

_boundingBox::~_boundingBox()
{
    //dtor
}

void _boundingBox::initBoundingBox(vec3f dim, vec3f pos, vec3f scale)
{
    this->dimensions = dim;
    this->position = pos;
    this->scale = scale;
}

void _boundingBox::displayBoundingBox()
{
    // For debugging: draw the bounding box as a wireframe cube
    GLfloat currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor); // save current color
    glPushMatrix();
        glTranslatef(position.x + positionOffset.x, position.y + positionOffset.y, position.z + positionOffset.z);
        glScalef(dimensions.x * scale.x, dimensions.y * scale.y, dimensions.z * scale.z);
        glColor3f(1.0f, 0.0f, 0.0f); // Red color for visibility
        glutWireCube(1.0f); // Draw unit cube scaled to bounding box size
    glPopMatrix();
    glColor4fv(currentColor);                 

}

_boundingBox::collisionType _boundingBox::checkCollision(const _boundingBox& otherBox)
{
    float xPos, yPos, zPos;
    float oxPos, oyPos, ozPos;
    
    // this box position with offset
    xPos = position.x + positionOffset.x;
    yPos = position.y + positionOffset.y;
    zPos = position.z + positionOffset.z;
    // other box position with offset
    oxPos = otherBox.position.x + otherBox.positionOffset.x;
    oyPos = otherBox.position.y + otherBox.positionOffset.y;
    ozPos = otherBox.position.z + otherBox.positionOffset.z;

    float xMin, xMax, yMin, yMax, zMin, zMax; // this box
    float oxMin, oxMax, oyMin, oyMax, ozMin, ozMax; // other box    

    // This box bounds
    xMin = xPos - (dimensions.x * scale.x) / 2.0f;
    xMax = xPos + (dimensions.x * scale.x) / 2.0f;
    yMin = yPos - (dimensions.y * scale.y) / 2.0f;
    yMax = yPos + (dimensions.y * scale.y) / 2.0f;
    zMin = zPos - (dimensions.z * scale.z) / 2.0f;
    zMax = zPos + (dimensions.z * scale.z) / 2.0f;

    // Other box bounds
    oxMin = otherBox.position.x - (otherBox.dimensions.x * otherBox.scale.x) / 2.0f;
    oxMax = otherBox.position.x + (otherBox.dimensions.x * otherBox.scale.x) / 2.0f;
    oyMin = otherBox.position.y - (otherBox.dimensions.y * otherBox.scale.y) / 2.0f;
    oyMax = otherBox.position.y + (otherBox.dimensions.y * otherBox.scale.y) / 2.0f;
    ozMin = otherBox.position.z - (otherBox.dimensions.z * otherBox.scale.z) / 2.0f;
    ozMax = otherBox.position.z + (otherBox.dimensions.z * otherBox.scale.z) / 2.0f;

    bool xOverLap = (xMin <= oxMax) && (xMax >= oxMin);
    bool yOverLap = (yMin <= oyMax) && (yMax >= oyMin);
    bool zOverLap = (zMin <= ozMax) && (zMax >= ozMin);

    if (!(xOverLap && yOverLap && zOverLap)) {
        return NONE; // No collision
    }
    // Calculate penetration depths (lets us know direction + depth)
    float xPenetration = std::min(xMax - oxMin, oxMax - xMin);
    float yPenetration = std::min(yMax - oyMin, oyMax - yMin);
    float zPenetration = std::min(zMax - ozMin, ozMax - zMin);

    if (xPenetration < yPenetration && xPenetration < zPenetration) {
        // Collision on X-axis - either + or -
        if (position.x < otherBox.position.x) {
            return POS_X;  
        } else {
            return NEG_X;   
        }
    } 
    else if (yPenetration < zPenetration) {
        // Collision on Y-axis - either + or -
        if (position.y < otherBox.position.y) {
            return POS_Y;    
        } else {
            return NEG_Y; 
        }
    } 
    else {
        // Collision on Z-axis - either + or -
        if (position.z < otherBox.position.z) {
            return POS_Z;  
        } else {
            return NEG_Z;   
        }
    }
}

bool _boundingBox::isColliding(const _boundingBox& otherBox)
{
    float xMin, xMax, yMin, yMax, zMin, zMax; // this box
    float oxMin, oxMax, oyMin, oyMax, ozMin, ozMax; // other box    
    // This box bounds
    xMin = position.x - (dimensions.x * scale.x) / 2.0f;
    xMax = position.x + (dimensions.x * scale.x) / 2.0f;
    yMin = position.y - (dimensions.y * scale.y) / 2.0f;
    yMax = position.y + (dimensions.y * scale.y) / 2.0f;
    zMin = position.z - (dimensions.z * scale.z) / 2.0f;
    zMax = position.z + (dimensions.z * scale.z) / 2.0f;
    // Other box bounds
    oxMin = otherBox.position.x - (otherBox.dimensions.x * otherBox.scale.x) / 2.0f;
    oxMax = otherBox.position.x + (otherBox.dimensions.x * otherBox.scale.x) / 2.0f;
    oyMin = otherBox.position.y - (otherBox.dimensions.y * otherBox.scale.y) / 2.0f;
    oyMax = otherBox.position.y + (otherBox.dimensions.y * otherBox.scale.y) / 2.0f;
    ozMin = otherBox.position.z - (otherBox.dimensions.z * otherBox.scale.z) / 2.0f;
    ozMax = otherBox.position.z + (otherBox.dimensions.z * otherBox.scale.z) / 2.0f;
    // AABB collision check
    return (xMin <= oxMax && xMax >= oxMin) && (yMin <= oyMax && yMax >= oyMin) && (zMin <= ozMax && zMax >= ozMin);
}

