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
    float preferredMin = 7.0f;
    float preferredMax = 12.0f;
    float strafeSpeed  = 1.2f;
    float dangerRadius = 1.2f;
    // runtime
    State state = State::Patrol;
    float stateT = 0.f;

    void render() override;
    template<typename Space>
    void updateAI(double dt, const Space& space) {
        // cooldown tick
        if (hurtCooldown > 0.f) hurtCooldown = std::max(0.f, hurtCooldown - (float)dt);

        // local intent velocity for this frame (in 'space' local coords)
        vec3 vL{0,0,0};

        // ---- SENSING (FOV uses ENEMY FACING, not space forward) ----
        bool sees = false;
        if (target) {
            // world vector to target (XZ plane)
            vec3 toT{ target->position.x - position.x, 0.f, target->position.z - position.z };
            float d = std::sqrt(toT.x*toT.x + toT.z*toT.z);

            if (d < viewDistance) {
                // enemy's world forward from its own yaw
                const float yr = yawDeg * (3.1415926535f / 180.f);
                vec3 fwdW{ std::sin(yr), 0.f, -std::cos(yr) };

                const float fL = std::sqrt(fwdW.x*fwdW.x + fwdW.z*fwdW.z);
                const float cosA = (fwdW.x*toT.x + fwdW.z*toT.z) / (std::max(1e-6f, fL) * std::max(1e-6f, d));
                const float angDeg = std::acos(clamp(cosA, -1.f, 1.f)) * 180.f / 3.1415926535f;

                sees = (angDeg <= 0.5f * fovDeg);
            }
        }

        if (sees) state = (state == State::Cooldown ? State::Cooldown : State::Chase);

        stateT += (float)dt;

        switch (state) {
        case State::Patrol:
            // simple idle: no movement by default (plug your own patrol later)
            vL = {0,0,0};
            if (sees) { state = State::Windup; stateT = 0.f; }
            break;

        case State::Chase:
            if (target) {
                // chase in LOCAL coordinates of the space
                vec3 pL = space.toLocal(position);
                vec3 tL = space.toLocal(target->position);
                vec3 delta{ tL.x - pL.x, 0.f, tL.z - pL.z };
                float L = std::sqrt(delta.x*delta.x + delta.z*delta.z);
                if (L > 1e-3f) { delta.x/=L; delta.z/=L; }
                vL = { delta.x * speed * (float)dt, 0.f, delta.z * speed * (float)dt };
            }
            if (stateT > windupTime) { state = State::Throw; stateT = 0.f; }
            break;

        case State::Windup:
            vL = {0,0,0};
            if (stateT >= windupTime) { state = State::Throw; stateT = 0.f; }
            break;

        case State::Throw:
            if (target) {
                // aim at target chest in WORLD space
                vec3 tgt{ target->position.x, target->position.y + 1.2f*target->radius, target->position.z };

                // init projectile
                throwAt(tgt, /*speed*/18.f, /*spreadDeg*/3.f);

                // muzzle along ENEMY FACING (not space forward)
                const float yr = yawDeg * (3.1415926535f / 180.f);
                vec3 fwdW{ std::sin(yr), 0.f, -std::cos(yr) };
                vec3 muzzle{
                    position.x + fwdW.x * (0.30f * radius),
                    position.y + 1.20f * radius,
                    position.z + fwdW.z * (0.30f * radius)
                };
                ball.src = muzzle; ball.pos = muzzle;

                // dodgeball size (same logic you chose)
                ball.radius = std::max(0.05f, 2.5f * radius);

                // level-defined trajectory (fallback: straight)
                ball.setTrajectory(makeTrajectory ? makeTrajectory()
                                                  : Trajectory_Straight());

                state = State::Cooldown; stateT = 0.f;
            }
            break;

        case State::Cooldown:
            vL = {0,0,0};
            if (stateT >= throwPeriod) { state = State::Windup; stateT = 0.f; }
            break;

        case State::Stunned:
            vL = {0,0,0};
            break;
        }
                // --- DODGE INCOMING PLAYER BALL (simple evasion) ---
        if (target && target->ball && target->ball->live) {
            const _bullets* b = target->ball;

            // Ball world direction on XZ
            vec3 dirW{ b->dir.x, 0.f, b->dir.z };
            float dirLen = std::sqrt(dirW.x*dirW.x + dirW.z*dirW.z);
            if (dirLen > 1e-6f) {
                dirW.x /= dirLen;
                dirW.z /= dirLen;

                // Vector from ball to enemy on XZ
                vec3 toEnemy{ position.x - b->pos.x, 0.f, position.z - b->pos.z };

                // Distance along ray
                float ahead = toEnemy.x*dirW.x + toEnemy.z*dirW.z;

                // Perpendicular distance to the ball path
                vec3 closest{
                    dirW.x * ahead,
                    0.f,
                    dirW.z * ahead
                };
                vec3 offset{
                    toEnemy.x - closest.x,
                    0.f,
                    toEnemy.z - closest.z
                };
                float distToLine = std::sqrt(offset.x*offset.x + offset.z*offset.z);

                float safeRadius = dangerRadius + b->radius + 0.5f * radius;

                // If we are in front of the ball and close to its path, strafe
                if (ahead > 0.f && distToLine < safeRadius) {
                    // World-space strafe direction (perpendicular to ball path)
                    vec3 strafeW{ -dirW.z, 0.f, dirW.x };

                    // Flip so we move away from the path
                    float side = (offset.x*strafeW.x + offset.z*strafeW.z) >= 0.f ? 1.f : -1.f;
                    strafeW.x *= side;
                    strafeW.z *= side;

                    // Map this to local space to keep using vL
                    vec3 pL = space.toLocal(position);
                    vec3 qL = space.toLocal(vec3{
                        position.x + strafeW.x,
                        position.y,
                        position.z + strafeW.z
                    });
                    vec3 strafeL{
                        qL.x - pL.x,
                        0.f,
                        qL.z - pL.z
                    };
                    float sLen = std::sqrt(strafeL.x*strafeL.x + strafeL.z*strafeL.z);
                    if (sLen > 1e-6f) {
                        strafeL.x /= sLen;
                        strafeL.z /= sLen;

                        // Add dodge component to our local velocity
                        vL.x += strafeL.x * strafeSpeed * (float)dt;
                        vL.z += strafeL.z * strafeSpeed * (float)dt;
                    }
                }
            }
        }


                // ---- Move & clamp using SPACE HELPERS ----
        // Convert the local intent to a world delta via the space transform
        vec3 pL = space.toLocal(position);
        vec3 w0 = space.toWorld(pL);
        vec3 w1 = space.toWorld({ pL.x + vL.x, pL.y + vL.y, pL.z + vL.z });
        vec3 vW{ w1.x - w0.x, w1.y - w0.y, w1.z - w0.z };

        position = moveAndClamp(space, position, vW, radius);

        // Face toward LOCAL motion (if any), mapped to WORLD yaw via the space
        if (std::abs(vL.x) + std::abs(vL.z) > 1e-4f) {
            const float yawLocalDeg = std::atan2(/*x*/vL.x, /*-z*/-vL.z) * 180.f / 3.1415926535f;
            yawDeg = worldYawFromLocal(space, yawLocalDeg);
        }

        // tick projectile
        ball.bulletActions(dt);
    }
        template<typename Space>
    inline void faceSpaceBackward(Enemy& e, const Space& space) {
        e.yawDeg = worldYawFromLocal(space, 180.0f);
    }

};



#endif // _ENEMY_H

