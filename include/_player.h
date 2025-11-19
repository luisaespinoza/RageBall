#ifndef _PLAYER_H
#define _PLAYER_H
#include<_3dmodelloader.h>
#include<_common.h>
#include<_textureloader.h>
#include<_bullets.h>
#include<_spatialNav.h>


struct PlayerInput {
    bool moveFwd  = false;
    bool moveBack = false;
    bool moveLeft = false;
    bool moveRight= false;
};
struct Player {
    _3DModelLoader model;
    vec3 position{0,0,0};
    float yawDeg = 0.f;
    float speed  = 6.0f;
    float radius = 0.5f;
    float scale = 0.20f;              // world-units per MD2 unit //set once do not touch
    float baseRadiusAtScale1 = 1.50f; // collider for scale==1; //SAME
    float baseYawMD2 = 90.0f;
    _bullets *ball = new _bullets();
void applyScale(float s) {
    scale  = s;
    radius = baseRadiusAtScale1 * scale;  // keep collider in sync with visual scale
}
    int life = 5; //abstraction of our collisions. You only get 5 collisions per level
    float hurtCooldown = 0.3f;           // seconds of invulnerability after a hit
    PlayerInput *playerInput= new PlayerInput();
    inline void playerResetLife( int newLife=5) {
        life = newLife;
        hurtCooldown = 0.f;
    }
    // call once after GL is ready
    bool init(const char* md2Path, const char* texturePath, _textureLoader& loader) {
        int ok = model.initModel(md2Path);
        if (!ok) {
            std::cerr << "initModel failed for: " << md2Path << "\n";
            return false;
        }
        if (texturePath && texturePath[0] != '\0') {
            GLuint tid = loader.loadTexture(const_cast<char*>(texturePath));
            if (tid != 0) model.md2file.tex_id = tid;
        }
        model.Actions(); // set initial animation
        return ok && (model.md2file.tex_id != 0);
    }

    // interpret input flags → velocity
    vec3 inputDelta(double dt, const PlayerInput* in) const {
        vec3 v{0,0,0};
        if (in->moveFwd)  v.z -= speed * (float)dt;
        if (in->moveBack) v.z += speed * (float)dt;
        if (in->moveLeft) v.x -= speed * (float)dt;
        if (in->moveRight)v.x += speed * (float)dt;
        return v;
    }

    void setAnimForVelocity(const vec3& v) {
//        if (std::abs(v.x) + std::abs(v.z) > 0.01f) model.actionTrigger = model.RUN;
//        else                                       model.actionTrigger = model.STAND;
//        model.Actions();  // updates StartFrame/EndFrame for the selected anim
        bool moving = (std::abs(v.x) + std::abs(v.z)) > 0.01f;
        model.actionTrigger = moving ? model.RUN : model.STAND;
        model.Actions();
    }
    template<typename Space>
    void moveAndClamp(double /*dt*/, const vec3& vel, Space& space) {
        vec3 newPos{ position.x + vel.x, position.y + vel.y, position.z + vel.z };
        vec3 pL = space.toLocal(newPos);
        pL = space.clampLocal(pL, radius);
        position = space.toWorld(pL);
    }

    void render() {

        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        glRotatef(baseYawMD2 + yawDeg, 0,1,0);
        glRotatef(-90.0f,1,0,0);
        glScalef(scale,scale,scale);
        // scale if needed
        model.Draw();       // Draw animates and renders current frame; no transforms inside
        glPopMatrix();
    }
    // Fire a throw toward a world-space point, with speed (units/sec) and optional spread
    inline void throwAt(const vec3& worldTarget, float throwSpeed, float spreadDeg) {
        if (!ball) return;
        ball->src  = position;
        ball->des  = worldTarget;
        ball->pos  = ball->src;
        ball->speed = throwSpeed;
        ball->coneHalfAngleDeg = spreadDeg;

        vec3 d{ worldTarget.x - position.x, worldTarget.y - position.y, worldTarget.z - position.z };
        float L = std::sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
        if (L > 1e-6f) { d.x/=L; d.y/=L; d.z/=L; }
        ball->dir = d;

        ball->t = 0.0f;
        ball->traveled = 0.0f;
        ball->live = true;
        ball->actionTrigger = ball->SHOOT;
    }

    // Step ball physics (dt is already propagated in your engine)
    inline void updateBall(double dt) {
        ball->bulletActions(dt); // uses the dt-based constant-velocity we set earlier
    }

    // Draw the ball (uses your existing drawBullet)
    inline void renderBall() {
        ball->drawBullet();  // draws a GLUT sphere when live
    }
    ~Player() {
        delete ball; ball = nullptr;
        delete playerInput; playerInput = nullptr;
    }
};
#endif // _PLAYER_H
