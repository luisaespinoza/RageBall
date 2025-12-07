#include <_animation.h>

_animation::_animation()
{
    animationTimer = new _timerPlusPlus();
}

_animation::~_animation()
{
    delete animationTimer;
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
        animationFrames.push_back(frameModel);
        cout << "Loaded animation frame: " << filePath << endl;
    }
    cout << "Animation loading complete. Total frames: " << animationFrames.size() << endl;
    animationTimer->reset();
}

void _animation::drawAnimation(vec3f position, vec3f rotation)
{
    // Update frame based on timer
    if (animationTimer->getTicks() >= (1000.0 / FPS)) {
        currentFrame = (currentFrame + 1) % frames;
        animationTimer->reset();
    }
    // draw animation frame
    animationFrames[currentFrame]->position = position;
    animationFrames[currentFrame]->rotation = rotation;
    animationFrames[currentFrame]->drawModel();
}

void _animation::resetAnimation()
{
    animationTimer->reset();
}