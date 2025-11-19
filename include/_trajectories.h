#ifndef _TRAJECTORIES_H
#define _TRAJECTORIES_H
#include <_common.h>
#include<_bullets.h>
// Simple straight-line motion (no gravity, constant speed)
inline _bullets::TrajectoryFn Trajectory_Straight() {
    return [](_bullets& b, double dt) {
        const float step = b.speed * static_cast<float>(dt);
        b.pos.x += b.dir.x * step;
        b.pos.y += b.dir.y * step;
        b.pos.z += b.dir.z * step;
        b.traveled += step;
    };
}

// Parabolic arc (gravity acting along -Y)
inline _bullets::TrajectoryFn Trajectory_Parabola(float gravityY = 9.8f) {
    return [gravityY](_bullets& b, double dt) {
        // simple gravity integration
        b.dir.y -= gravityY / b.speed * static_cast<float>(dt);
        const float step = b.speed * static_cast<float>(dt);
        b.pos.x += b.dir.x * step;
        b.pos.y += b.dir.y * step;
        b.pos.z += b.dir.z * step;
        b.traveled += step;
    };
}

// Gentle “lob” trajectory (slower, higher arc)
inline _bullets::TrajectoryFn Trajectory_Lob(float gravityY = 4.8f, float lift = 8.0f) {
    return [gravityY, lift](_bullets& b, double dt) {
        b.dir.y += lift * static_cast<float>(dt);             // add upward bias
        b.dir.y -= gravityY / b.speed * static_cast<float>(dt);
        const float step = b.speed * static_cast<float>(dt);
        b.pos.x += b.dir.x * step;
        b.pos.y += b.dir.y * step;
        b.pos.z += b.dir.z * step;
        b.traveled += step;
    };
}

//TODO: placeholder for bounce / ricochet behavior
inline _bullets::TrajectoryFn Trajectory_Bounce(float gravityY = 9.8f) {
    return [gravityY](_bullets& b, double dt) {
        const float step = b.speed * static_cast<float>(dt);
        b.pos.x += b.dir.x * step;
        b.pos.y += b.dir.y * step;
        b.pos.z += b.dir.z * step;
        b.traveled += step;

        // simple ground bounce
        if (b.pos.y < 0.0f) {
            b.pos.y = 0.0f;
            b.dir.y = -b.dir.y * 0.6f; // lose energy on bounce
        }
        b.dir.y -= gravityY / b.speed * static_cast<float>(dt);
    };
}


#endif // _TRAJECTORIES_H
