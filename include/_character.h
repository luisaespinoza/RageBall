#ifndef _CHARACTER_H
#define _CHARACTER_H
#include<_common.h>
#include<_bullets.h>
#include<_spatialNav.h>


struct Character {
    // pose / physics
    vec3  position{0,0,0};
    float yawDeg = 0.f;
    float radius = 0.1f;
    float speed  = 3.0f;

    // health
    int   life = 3;
    float hurtCooldown = 0.f;

    // projectile
    _bullets ball;

    // visuals (optional hooks)
    virtual void render() {}                // model draw
    virtual void setAnimForVelocity(const vec3& /*vW*/) {}

    // throw toward world point (keeps your player semantics)
    void throwAt(const vec3& worldTarget, float speed, float spreadDeg);

    // spawn/muzzle helper (scale-aware, uses space forward)
    template<typename Space>
    vec3 computeMuzzle(const Space& space) const;

    // projectile tick
    void updateBall(double dt);

    virtual ~Character() = default;
};
#endif // _CHARACTER_H
