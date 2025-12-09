#include "_worldObjects.h"

/*  -------- TARGETS  -------- */

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
        rotation.x -= 50.0f * dt; // spin effect
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
    if (state != DEAD) {
         _model::drawModel();
    }
}

/*  -------- BALLS  -------- */

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

/*  -------- THROWERS  -------- */

_thrower::_thrower()
{
    //ctor
    throw_animation = new _animation();
    reset_animation = new _animation();
    prep_animation = new _animation();

    stateTimer = new _timerPlusPlus();
    hitTimer = new _timerPlusPlus();
    deathTimer = new _timerPlusPlus();
}

_thrower::_thrower(const _thrower& other) : _model(other) // calls the copy constructor of _model 
{
    throw_animation = new _animation(*other.throw_animation);
    reset_animation = new _animation(*other.reset_animation);
    prep_animation = new _animation(*other.prep_animation);

    stateTimer = new _timerPlusPlus(*other.stateTimer);
    hitTimer = new _timerPlusPlus(*other.hitTimer);
    deathTimer = new _timerPlusPlus(*other.deathTimer);
    // copy states
    currentState = other.currentState;
    health = other.health;
    leftBound = other.leftBound;
    rightBound = other.rightBound;
    speed = other.speed;
    direction = other.direction;
    deathLength = other.deathLength;
    throwInterval = other.throwInterval;
    existenceState = other.existenceState;
    lastHitIndex = other.lastHitIndex;
    currentFrame = other.currentFrame;
}
_thrower::~_thrower()
{
    //dtor
    delete throw_animation; throw_animation = nullptr;
    delete reset_animation; reset_animation = nullptr;
    delete prep_animation; prep_animation = nullptr;
    delete stateTimer; stateTimer = nullptr;
    delete hitTimer; hitTimer = nullptr;
    delete deathTimer; deathTimer = nullptr;
}

void _thrower::initThrower() {
    // Initialize the model
    _model::initModel("", "models/thrower.obj", _model::CUSTOM);
    // Initialize animations
    throw_animation->initAnimation("", "models/animations/Thrower_Throw/thrower_throw", 24, 24, 0, scale);
    reset_animation->initAnimation("", "models/animations/Thrower_Reset/thrower_reset", 8, 24, 48, scale);
    prep_animation->initAnimation("", "models/animations/Thrower_Prep/thrower_prep", 12, 24, 56, scale);

    stateTimer->reset();
    hitTimer->reset();
    deathTimer->reset();
}

void _thrower::drawModel()
{
    if (existenceState == DYING) {
        color = {0.5f, 0.5f, 0.5f}; // gray when dying
    } else if (existenceState == DAMAGE) {
        color = {1.0f, 0.2f, 0.2f}; // light red when damaged
    } else {
        color = {1.0f, 1.0f, 1.0f}; // normal color
    }
    switch (currentState) {
        case IDLE:
            _model::drawModel();
            break;
        case PREPARE:
            if (prep_animation->currentFrame == prep_animation->frames - 1) {
                // Transition to THROW state
                currentState = THROW;
            } else {
                prep_animation->drawAnimation(position, rotation);
            }
            break;
        case THROW:
            if (prep_animation->currentFrame == throw_animation->frames - 1) {
                // Transition to RESET state
                currentState = RESET;
            } else {
                throw_animation->drawAnimation(position, rotation);
            }
            break;
        case RESET:
            if (reset_animation->currentFrame == reset_animation->frames - 1) {
                // Transition to IDLE state
                currentState = IDLE;
                stateTimer->reset();
            } else {
                reset_animation->drawAnimation(position, rotation);
            }
            break;
    }
}

void _thrower::updateModel(double dt) {
    if (existenceState == DYING && deathTimer->getTicks() < deathLength * 1000) {
        rotation.x -= 45.0f * dt; // spin effect
        return; // do not update if dying
    } 
    if (existenceState == DYING && deathTimer->getTicks() >= deathLength * 1000) {
        existenceState = DEAD;
        return;
    } 
    if (stateTimer->getTicks() > throwInterval*1000 && currentState == IDLE) {
        // Transition to PREPARE state
        currentState = PREPARE;
        stateTimer->reset();
    } else {
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
}

void _thrower::hitThrower(int modelId) {
    if (lastHitIndex != modelId) {  // check for double collision
        health--;
        lastHitIndex = modelId;
        existenceState = DAMAGE;
        hitTimer->reset();
    }
    if (health <= 0) {
        // Transition to DYING state
        existenceState = DYING;
        deathTimer->reset();
    }
}