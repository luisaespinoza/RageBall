#ifndef _WORLD_OBJECTS_H
#define _WORLD_OBJECTS_H 

#include "_model.h"
#include <_timerPlusPlus.h>
#include <_animation.h>

// State of the target for hit detection
enum TargetState { ACTIVE, HIT, DEAD };

class _targets : public _model {
    public:
        _targets();
        _targets(const _targets& other);    // copy constructor
        virtual ~_targets();
 
        TargetState state = ACTIVE;
        float leftBound = 0.0f;
        float rightBound = 0.0f;
        float speed = 0; // speed at which target moves
        int direction = 0; // direction of movement: 1 = right, -1 = left
        float deathLength = 2.0; // Length target exists in HIT animation prior to death (seconds)

        virtual void drawModel() override;
        // Handles target update logic (movement, physics, etc) Doesn't handle rendering/drawing
        void updateModel(double dt);
        void targetHit();

    protected:
    private:
            _timerPlusPlus* hitTimer = nullptr;
};

enum BallType { BALL_FRIENDLY, BALL_ENEMY };

class _balls : public _model {
    public:
        _balls();
        _balls(const _balls& other);    // copy constructor
        virtual ~_balls();

        BallType ballType = BALL_FRIENDLY;

        float lifeLength = 5.0f; // length of time ball exists before disappearing (seconds)
        double spawnTime = 0.0; // time when ball was created

        virtual void drawModel() override;
        // Handles ball update logic (movement, physics, etc) Doesn't handle rendering/drawing
        void updateModel(double dt);
        // Used sound cooldown -- prevents multiple rapid sounds on collisions
        _timerPlusPlus* soundCooldownTimer = nullptr;

    protected:
    private:
};

// Simple "enemy" that throws balls at the player -- doesn't really have an AI. Could use its own file but easier to just place here.
class _thrower : public _model {
    public:
        _thrower();
        _thrower(const _thrower& other);    // copy constructor
        virtual ~_thrower();
        // Initializes a thrower with animations -- this should be ran only on the master copy (prototype), never on copies
        void initThrower();
        virtual void drawModel() override;
        void updateModel(double dt);
        // Applies damage effect
        void hitThrower(int modelId);

        enum ThrowerAnimationStates { IDLE, PREPARE, THROW, RESET };
        enum ThrowerExistenceStates { ALIVE, DAMAGE, DYING, DEAD };

        ThrowerAnimationStates currentState = IDLE;
        ThrowerExistenceStates existenceState = ALIVE;

        bool throwBall = false;
        int health = 1;
        float leftBound = -12.5f;
        float rightBound = 12.5f;
        float speed = 0; // speed at which target moves
        int direction = 0; // direction of movement: 1 = right, -1 = left
        float deathLength = 2.0; // Length enemy exists in HIT animation prior to death (seconds)
        float throwInterval = 2.0f; // seconds between throws
    protected:
    private:
        _timerPlusPlus* stateTimer = nullptr;
        _timerPlusPlus* hitTimer = nullptr;
        _timerPlusPlus* deathTimer = nullptr;

        _animation* throw_animation = nullptr;
        _animation* reset_animation = nullptr;
        _animation* prep_animation = nullptr;

        int lastHitIndex = -1; // each model has a uniquie model id, this checks the LAST collision to prevent multiple hits from same ball
        int currentFrame = 0; // number of frames progressed in current animation -- for transitions
        void resetAnimations();
        bool ownsAnimations = true; // for proper deletion of heap-allocated memory
};


#endif // _WORLD_OBJECTS_H