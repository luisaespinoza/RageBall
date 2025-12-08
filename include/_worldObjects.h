#ifndef _WORLD_OBJECTS_H
#define _WORLD_OBJECTS_H 

#include "_model.h"
#include <_timerPlusPlus.h>

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

    protected:
    private:
};


#endif // _WORLD_OBJECTS_H