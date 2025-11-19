#ifndef _BULLETS_H
#define _BULLETS_H

#include<_common.h>
#include<_textureloader.h>
#include<_timer.h>

class _bullets
{
    public:
        _bullets();
        virtual ~_bullets();

        _textureLoader *tex = new _textureLoader();
        _timer         *mytime   = new _timer();

        vec3 pos;
        vec3 des;
        vec3 src;
        vec3 scale;

        vec3 dir{0,0,0};      // normalized travel direction
        vec3  baseDir{0,0,0};       // exact aim (src -> des), normalized
        float speed = 1.0f;   // units per tick
        float coneHalfAngleDeg = 0.0f; // 0 = perfect accuracy
        float maxRange = 200.0f;  // safety cap so missed shots don’t go forever
        float traveled = 0.0f;    // how far the shot has flown
        float radius = 0.03f;

        void setInaccuracyCone(float deg) { coneHalfAngleDeg = deg; }//per agent or level inaccuracy
        uint32_t rngState = 0x9E3779B9u;
        inline uint32_t rngU32() { rngState = rngState*1664525u + 1013904223u; return rngState; }
        inline float rng01()     { return (rngU32() >> 8) * (1.0f / 16777216.0f); } // [0,1)
// ---- helper to build a perturbed direction inside a cone ----
        static inline vec3 anyPerp(const vec3& n);
        vec3 dirInCone(const vec3& axisN, float halfAngleDeg);
        using TrajectoryFn = std::function<void(_bullets& b, double dt)>;
        void setTrajectory(TrajectoryFn fn) { trajectory_ = std::move(fn); }
        float t=0; // for state flag we use global dt now
        bool live = true;

        void iniBullet(char *);
        void drawBullet();
        void bulletActions(double dt);

        int actionTrigger;

        enum{READY,SHOOT,HIT};
    protected:

    private:
        TrajectoryFn trajectory_;
};

#endif // _BULLETS_H
