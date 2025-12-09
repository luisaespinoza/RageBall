#include <_animation.h>

/* ----- ANIMATION DATA ------ */

_animationData::_animationData()
{
    //ctor
}

_animationData::~_animationData()
{
    //dtor
    for (int i = 0; i < frames.size(); i++) {
        delete frames[i];
        frames[i] = nullptr;
    }
    frames.clear();
}

/* ----- ANIMATION ------ */

_animation::_animation()
{
    animationTimer = new _timerPlusPlus();
    animationData = new _animationData();   // this is a master copy (prototype) -- needs the animation data
    ownsResources = true; // single instance owns resources
}

_animation::_animation(const _animation& other) 
{
    animationTimer = new _timerPlusPlus();
    animationData = other.animationData; // share the data
    frames = other.frames;
    FPS = other.FPS;
    currentFrame = 0;   // cleaner -- copies always start at frame 0
    ownsResources = false; // copies do not own resources 
}

_animation::~_animation()
{
    delete animationTimer; animationTimer = nullptr;
    if (ownsResources) {
        delete animationData;
        animationData = nullptr;
    }
}

void _animation::initAnimation(char* textPath, char* modelPath, int frames, int fps, int frameOffset, vec3f scale)
{
    currentFrame = 0;
    this->frames = frames;
    FPS = fps;
    animationTimer->reset();

    for (int i = 0; i < frames; i++) {
        _model* frameModel = new _model();
        char filePath[512];
        sprintf(filePath, "%s%04d.obj", modelPath, i + frameOffset);
        frameModel->initModel(textPath, filePath, _model::CUSTOM);
        frameModel->scale = scale;
        if (ownsResources) {
            animationData->frames.push_back(frameModel);
        }
        //animationFrames.push_back(frameModel);
        cout << "Loaded animation frame: " << filePath << endl;
    }
    cout << "Animation loading complete. Total frames: " << animationData->frames.size() << endl;
    animationTimer->reset();
}

void _animation::drawAnimation(vec3f position, vec3f rotation)
{
    // Update frame based on timer
    if (animationTimer->getTicks() >= (1000.0 / FPS)) {
        if (loopAnimation) {
            currentFrame = (currentFrame + 1) % frames;
        } else {
            if (currentFrame < frames - 1) {
                animationComplete = false;
                currentFrame++;
            } else {
                animationComplete = true;
            }
        } 
        animationTimer->reset();       
    }
    // draw animation frame
    animationData->frames[currentFrame]->position = position;
    animationData->frames[currentFrame]->rotation = rotation;
    animationData->frames[currentFrame]->drawModel();
}

void _animation::resetAnimation()
{
    currentFrame = 0;
    animationTimer->reset();
}