#include "_model.h"

_model::_model()
{
    //ctor
        rotation = {0.0f,0.0f,0.0f};
        position = {0.0f,1.0f,0.0f};
        scale = {1.0f,1.0f,1.0f};
        color = {1.0f,1.0f,1.0f};
        velocity = {0.0f,0.0f,0.0f};
        acceleration = {0.0f,0.0f,0.0f};
}

_model::_model(const _model& other)
{    
    // copy texture
    this->texLoader = new _textureLoader();
    if (other.texLoader) {
        this->texLoader->textID = other.texLoader->textID;  
        this->texLoader->width = other.texLoader->width;
        this->texLoader->height = other.texLoader->height;
    }
    
    // copy model
    this->model3DLoader = new _objLoader();
    if (other.model3DLoader) {
        this->model3DLoader->vbo = other.model3DLoader->vbo;  
        this->model3DLoader->vertices = other.model3DLoader->vertices;
        this->model3DLoader->normals = other.model3DLoader->normals;
        this->model3DLoader->texCords = other.model3DLoader->texCords;
        this->model3DLoader->faces3 = other.model3DLoader->faces3;
        this->model3DLoader->faces4 = other.model3DLoader->faces4;
        this->model3DLoader->vertexCount = other.model3DLoader->vertexCount;
        this->model3DLoader->normalCount = other.model3DLoader->normalCount;
        this->model3DLoader->texcoordCount = other.model3DLoader->texcoordCount;
        this->model3DLoader->faceCount = other.model3DLoader->faceCount;
        this->model3DLoader->modelSize = other.model3DLoader->modelSize;
    }
 
    // copy model properties
    this->rotation = other.rotation;
    this->position = other.position;
    this->scale = other.scale;
    this->color = other.color;
    this->velocity = other.velocity;
    this->acceleration = other.acceleration;
    this->enabled = other.enabled;
    this->currentModel = other.currentModel;
    
    for (int i = 0; i < other.boundingBoxes.size(); i++) {
        this->boundingBoxes.push_back(other.boundingBoxes[i]);
    }
}

_model::~_model()
{
    //dtor
    delete texLoader;
    delete model3DLoader;
}
void _model::drawModel()
{
    if (!enabled) { return; }   // skip drawing if not enabled
    glPushMatrix();          
        glColor3f(color.r, color.g, color.b);             //set colors

        glTranslated(position.x,position.y,position.z); //translation

        glRotated(rotation.x,1,0,0);     //rotate around X-Axis
        glRotated(rotation.y,0,1,0);     //rotate around Y-Axis
        glRotated(rotation.z,0,0,1);     //rotate around Z-Axis

        glScaled(scale.x,scale.y,scale.z);  // Scale your model

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
}

void _model::initModel(char* texPath, char* modelPath, modelType currentModel)
{
    texLoader->loadTexture(texPath);
    this->currentModel = currentModel;
    if (currentModel == CUSTOM) {
        model3DLoader->initObj(modelPath);
        model3DLoader->buildBuffer();
        model3DLoader->debug();
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
