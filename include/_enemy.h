#ifndef _ENEMY_H
#define _ENEMY_H

#include <_common.h>
#include <_trajectories.h>
#include <_spatialNav.h>
#include "_character.h"
#include<_3dmodelloader.h>
#include<_player.h>
template <typename T>
inline const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (v > hi ? hi : v);
}
class Enemy : public Character {
public:
    enum class State { Patrol, Chase, Windup, Throw, Cooldown, Stunned };

    // -------- MD2 model (mirrors Player) --------
    _3DModelLoader model{};
    float scale = 0.20f;                // world-units per MD2 unit (same default as Player)
    float baseRadiusAtScale1 = 1.50f;   // collider for scale==1 (same)
    float baseYawMD2 = 90.0f;           // face +X in MD2, convert to your world forward like Player

    float animDt = 1.0f/60.0f;  // last dt from level

    // call once after GL is ready (mirrors Player::init)
    bool init(const char* md2Path, const char* texturePath, _textureLoader& loader);

    // keep collider in sync with visual scale (mirrors Player::applyScale)
    inline void applyScale(float s) {
        scale  = s;
        radius = baseRadiusAtScale1 * scale;  // same relationship as Player
    }

    // animation hint from movement (mirrors Player::setAnimForVelocity)
    void setAnimForVelocity(const vec3& vW) override;

    // dependencies
    // const Character* target = nullptr;
    const Player* target = nullptr;
    std::function<_bullets::TrajectoryFn()> makeTrajectory = nullptr;

    // tuning
    float viewDistance = 15.f;
    float fovDeg       = 120.f;
    float throwPeriod  = 1.4f;
    float windupTime   = 0.25f;
    float preferredMin = 6.0f;
    float preferredMax = 16.0f;
    float strafeSpeed  = 1.2f;
    float dangerRadius = 1.2f;
    // runtime
    State state = State::Patrol;
    float stateT = 0.f;

    void render() override;
    template<typename Space>
 void updateAI(double dt, const Space& space) {
        // Local intent velocity in WORLD space now
        vec3 vW{0,0,0};

        // --- Sense player in WORLD space ---
        bool  sees         = false;
        float distToTarget = 1e9f;
        vec3  toT{0,0,0};

        if (target) {
            toT = { target->position.x - position.x,
                    0.f,
                    target->position.z - position.z };

            distToTarget = std::sqrt(toT.x*toT.x + toT.z*toT.z);

            if (distToTarget > 1e-3f) {
                // FOV check using enemy yaw + world-space to-target
                const float yr = yawDeg * (3.1415926535f / 180.f);
                vec3 fwdW{ std::sin(yr), 0.f, -std::cos(yr) };

                const float fL   = std::sqrt(fwdW.x*fwdW.x + fwdW.z*fwdW.z);
                const float cosA = (fwdW.x*toT.x + fwdW.z*toT.z)
                                 / (std::max(1e-6f, fL) * std::max(1e-6f, distToTarget));
                const float angDeg = std::acos(clamp(cosA, -1.f, 1.f))
                                   * 180.f / 3.1415926535f;

                sees = (distToTarget < viewDistance && angDeg <= 0.5f * fovDeg);
            }
        }

        // advance state timer
        stateT += (float)dt;

        // --- State transitions using LOS + radius ---
        if (!target || !sees) {
            // lost sight: go back to Patrol unless in Cooldown or Stunned
            if (state != State::Cooldown && state != State::Stunned) {
                state = State::Patrol;
            }
        } else {
            // see the player
            if (state == State::Patrol || state == State::Chase) {
                if (distToTarget > preferredMax) {
                    state = State::Chase;   // too far: keep closing in
                } else if (state != State::Cooldown) {
                    state  = State::Windup; // close enough: start attack
                    stateT = 0.f;
                }
            }
        }

        // --- Per-state behavior ---
        switch (state) {
            case State::Patrol:
                vW = {0,0,0};
                break;

            case State::Chase:
                if (target && distToTarget > 1e-3f) {
                    // world-space chase direction
                    vec3 dir = { toT.x / distToTarget, 0.f, toT.z / distToTarget };
                    vW = { dir.x * speed * (float)dt,
                           0.f,
                           dir.z * speed * (float)dt };

                    // once inside radius, go to windup
                    if (distToTarget <= preferredMax) {
                        state  = State::Windup;
                        stateT = 0.f;
                    }
                }
                break;

            case State::Windup:
                vW = {0,0,0};
                if (stateT >= windupTime) {
                    state  = State::Throw;
                    stateT = 0.f;
                }
                break;

            case State::Throw:
                if (target) {
                    // aim at chest
                    vec3 tgt{
                        target->position.x,
                        target->position.y + 1.2f * target->radius,
                        target->position.z
                    };

                    throwAt(tgt, /*speed*/18.f, /*spreadDeg*/3.f);

                    // muzzle along facing
                    const float yr = yawDeg * (3.1415926535f / 180.f);
                    vec3 fwdW{ std::sin(yr), 0.f, -std::cos(yr) };
                    vec3 muzzle{
                        position.x + fwdW.x * (0.30f * radius),
                        position.y + 1.20f * radius,
                        position.z + fwdW.z * (0.30f * radius)
                    };

                    ball.src    = muzzle;
                    ball.pos    = muzzle;
                    ball.radius = std::max(0.05f, 2.5f * radius);
                    ball.setTrajectory(makeTrajectory ? makeTrajectory()
                                                      : Trajectory_Straight());

                    state  = State::Cooldown;
                    stateT = 0.f;
                }
                vW = {0,0,0};
                break;

            case State::Cooldown:
                vW = {0,0,0};
                if (stateT >= throwPeriod) {
                    state  = State::Windup;
                    stateT = 0.f;
                }
                break;

            case State::Stunned:
                vW = {0,0,0};
                break;
        }

        // --- Apply movement in WORLD space ---
        position.x += vW.x;
        position.y += vW.y;
        position.z += vW.z;

        // Use nav space ONLY to clamp back into the arena
        vec3 pL = space.toLocal(position);
        pL = space.clampLocal(pL, radius);
        position = space.toWorld(pL);

        // tick projectile
        ball.bulletActions(dt);
    }
};
#endif // _ENEMY_H

