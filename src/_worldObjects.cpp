#include "_worldObjects.h"

/* TARGETS */

_targets::_targets()
{
    //ctor
    hitTimer = new _timerPlusPlus();
}

_targets::_targets(const _targets& other) : _model(other) // calls the copy constructor of _model 
{
    hitTimer = new _timerPlusPlus();
    // copy states
    state = other.state;
    speed = other.speed;
    direction = other.direction;
    leftBound = other.leftBound;
    rightBound = other.rightBound;
    deathLength = other.deathLength;
}

_targets::~_targets()
{
    //dtor
    delete hitTimer;
    hitTimer = nullptr;
}

void _targets::targetHit()
{
    hitTimer->reset();
    state = HIT;
    color = {1.0f, 0.0f, 0.0f}; // change color to red when hit
}

void _targets::updateModel(double dt) {
    // life
    if (state == HIT) {
        if (hitTimer->getTicks() >= deathLength * 1000) {
            state = DEAD;
        }
        return; // skip movement if hit
    }
    // direction
    if (position.x > rightBound) {
        direction = -1; 
    } else if (position.x < leftBound) {
        direction = 1;  
    }
    // movement
    if (direction == 1) {
        position.x += speed * dt;
    } else if (direction == -1) {
        position.x -= speed * dt;
    }
}

void _targets::drawModel()
{
    switch (state) {
        case ACTIVE:
            // Normal drawing
            _model::drawModel();
            break;
        case HIT:
            // Draw with hit effect
            rotation.x += 10.0f; // spin effect
            _model::drawModel();
            break;
        case DEAD:
            // Do not draw if dead
            break;
    }
}

/* BALLS */

_balls::_balls()
{
    //ctor
}

_balls::_balls(const _balls& other) : _model(other) // calls the copy constructor of _model 
{
    // copy states
    ballType = other.ballType;
    lifeLength = other.lifeLength;
}   

_balls::~_balls()
{
    //dtor
}

void _balls::updateModel(double dt) {
    // physics update
    velocity.x += acceleration.x * dt;
    velocity.y += acceleration.y * dt;
    velocity.z += acceleration.z * dt;

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
    position.z += velocity.z * dt;
}

void _balls::drawModel()
{
    if (ballType == BALL_FRIENDLY) {
        color = {0.0f, 1.0f, 0.0f}; // green for friendly
    } else if (ballType == BALL_ENEMY) {
        color = {1.0f, 0.0f, 0.0f}; // red for enemy
    }
    _model::drawModel();
}