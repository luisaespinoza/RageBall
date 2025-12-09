#include "_model.h"
int _model::nextModelId = 0;
_model::_model()
{
    //ctor
    modelId = nextModelId++;
    ownsResources = true; // single instance owns resources
    texLoader = new _textureLoader();
    model3DLoader = new _objLoader();


}

_model::_model(const _model& other)
{    
    modelId = nextModelId++;
    ownsResources = false; // copies do not own resources
    // copy texture
    this->texLoader = other.texLoader;
    this->model3DLoader = other.model3DLoader;
 
    // Copy transform properties (unique per instance)
    this->rotation = other.rotation;
    this->position = other.position;
    this->scale = other.scale;
    this->color = other.color;
    this->velocity = other.velocity;
    this->acceleration = other.acceleration;
    this->enabled = other.enabled;
    this->currentModel = other.currentModel;
    
    // Copy bounding boxes
    for (int i = 0; i < other.boundingBoxes.size(); i++) {
        this->boundingBoxes.push_back(other.boundingBoxes[i]);
    }
}

_model::~_model()
{
    //dtor
    if (ownsResources) {
        delete texLoader;
        delete model3DLoader;
        texLoader = nullptr;
        model3DLoader = nullptr;
    }
}
void _model::drawModel()
{
    if (!enabled) { return; }   // skip drawing if not enabled
    GLfloat currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor); // save current color
    glPushMatrix();          
        glColor3f(color.r, color.g, color.b);             //set colors

        glTranslated(position.x,position.y,position.z); 

        glRotated(rotation.x,1,0,0);     
        glRotated(rotation.y,0,1,0);     
        glRotated(rotation.z,0,0,1);    

        glScaled(scale.x,scale.y,scale.z);  

        texLoader->bindTexture();     // load teaxture onto given mode;
        switch (currentModel) {
            case TEAPOT:
                glutSolidTeapot(1.0);
                break;
            case TORUS:
                glutSolidTorus(0.5, 1.0, 20, 20);
                break;
            case CUBE:
                glutSolidCube(1.0);
                break;
            case SPHERE:
                glutSolidSphere(1.0, 20, 20);
                break;
            case CUSTOM:
                model3DLoader->drawObj();
                break;
            default:
                glutSolidTeapot(1.0);
                break;
        }
    glPopMatrix();
    glColor4fv(currentColor);                 
}

void _model::initModel(char* texPath, char* modelPath, modelType currentModel)
{
    texLoader->loadTexture(texPath);
    this->currentModel = currentModel;
    if (currentModel == CUSTOM) {
        model3DLoader->initObj(modelPath);
        model3DLoader->buildBuffer();
        //model3DLoader->debug();
    }
    enabled = true;
}

void _model::initModel(char* texPath, modelType currentModel)
{
    texLoader->loadTexture(texPath);
    this->currentModel = currentModel;
    enabled = true;
}

void _model::addBoundingBox(vec3f dim, vec3f pos, vec3f scale)
{
    _boundingBox box;
    box.initBoundingBox(dim, pos, scale);
    boundingBoxes.push_back(box);
}

void _model::addBoundingBox(vec3f dim)
{
    _boundingBox box;
    box.initBoundingBox(dim, position, scale);
    boundingBoxes.push_back(box);
}

void _model::displayBoundingBoxes()
{
    for (int i = 0; i < boundingBoxes.size(); i++) {
        boundingBoxes[i].displayBoundingBox();
    }
}

bool _model::isCollidingWith(const _boundingBox& otherBox)
{
    for (int i = 0; i < boundingBoxes.size(); i++) {
        if (boundingBoxes[i].isColliding(otherBox)) {
            return true;
        }
    }
    return false;
}

vector<_boundingBox::collisionType> _model::checkCollisionWith(const _boundingBox& otherBox)
{
    vector<_boundingBox::collisionType> collisions;
    for (int i = 0; i < boundingBoxes.size(); i++) {
        _boundingBox::collisionType colType = boundingBoxes[i].checkCollision(otherBox);
        if (colType != _boundingBox::NONE) {
            collisions.push_back(colType);
        }
    }
    return collisions;
}

// Reusable teapot instance (no per-frame realloc)
static _model& TeapotModel() {
    static _model m;
    // Keep the model at origin; hallway handles placement
    m.position = {0.f, 0.f, 0.f};
    m.rotation = {0.f, 0.f, 0.f};
    return m;
}

// Obstacle draw function compatible with your addObstacleLocal(..., drawFn)
void DrawTeapot() {
    auto& m = TeapotModel();
    m.scale = {0.5f, 0.5f, 0.5f};            // pick a size that fits the hallway
    m.drawModel();             // calls glutSolidTeapot(...) internally
}
