#ifndef _PLAYER_H
#define _PLAYER_H
#include<_3dmodelloader.h>
#include<_common.h>
#include<_textureloader.h>
#include<_bullets.h>
#include<_spatialNav.h>
#include<_character.h>

#include<_animation.h>
#include<_timerPlusPlus.h>
#include<_model.h>

enum PlayerAnimation {IDLE, WALK};
enum PlayerThrowAnimation {THROW_NONE, THROW_PREP, THROW_RELEASE};

struct PlayerInput {
    bool moveFwd  = false;
    bool moveBack = false;
    bool moveLeft = false;
    bool moveRight= false;
};
struct Player : public Character {
    _3DModelLoader model;
    vec3 position{0,0,0};
    vec3f rotation = {0.0f,0.0f,0.0f};
    vec3f vec_scale = {1.0f,1.0f,1.0f};
    col3f color = {1.0f,1.0f,1.0f};
    float yawDeg = 0.f;
    float speed  = 6.0f;
    float radius = 0.5f;
    float scale = 0.20f;              // world-units per MD2 unit //set once do not touch
    float baseRadiusAtScale1 = 1.50f; // collider for scale==1; //SAME
    float baseYawMD2 = 90.0f;
    _bullets *ball = nullptr;
    float animDt = 1.0f/60.0f;  // last dt passed from level
    // JANUS - OBJ PLAYER SECTION //
    PlayerAnimation currentAnimation;
    PlayerThrowAnimation currentThrowAnimation = THROW_NONE;
    _model* playerModel = nullptr;
    _model* ballModel = nullptr;
    _model* playerNoArmModel = nullptr; // used for idle throwing
    _animation* walk_body_animation = nullptr;
    _animation* walk_arm_animation = nullptr;
    _animation* walk_ball_animation = nullptr;
    // for throwing animation
    _animation* throw_prep_arm_animation = nullptr;
    _animation* throw_prep_ball_animation = nullptr;
    bool hasBall = true;
    void initPlayer()
    { 
        
        // Initialize player properties
        //hasBall = false;
        currentAnimation = IDLE;
        //health = 100;
        //collisionBoxOffset = {0.0f, 0.8f, 0.0f};
        //rotation = {0.0f, 0.0f, 0.0f};
        //size = {1.0f, 1.0f, 1.0f};
        // main model
        playerModel->initModel("", "models/player.obj", _model::CUSTOM);
        playerModel->enabled = true;
        playerModel->scale = vec_scale;
        // ball
        ballModel->initModel("images/dodgeball.jpg", "models/ball_idle.obj", _model::CUSTOM);
        ballModel->scale = vec_scale;

        playerNoArmModel->initModel("", "models/player_no_arm.obj", _model::CUSTOM);
        playerNoArmModel->scale = vec_scale;
        // collision
        //collisionBox->initBoundingBox({0.7f, 2.0f, 0.7f}, (position+collisionBoxOffset), size);
        // animations
        walk_body_animation->initAnimation("", "models/animations/Walk_Forward/Body/Walk_Forward", 24, 28, 12, playerModel->scale);
        walk_arm_animation->initAnimation("", "models/animations/Walk_Forward/RightArm/Walk_Forward", 24, 28, 12, playerModel->scale);
        walk_ball_animation->initAnimation("images/dodgeball.jpg", "models/animations/Walk_Forward/Ball/Walk_Forward", 24, 28, 12, playerModel->scale);
        throw_prep_arm_animation->initAnimation("", "models/animations/Player_Throw/Prep_Throw_Arm/prep_throw_arm", 48, 24, 1, playerModel->scale);
        throw_prep_ball_animation->initAnimation("images/dodgeball.jpg", "models/animations/Player_Throw/Prep_Throw_Ball/prep_throw_ball", 48, 24, 1, playerModel->scale);
        throw_prep_arm_animation->loopAnimation = false;
        throw_prep_ball_animation->loopAnimation = false;
    }
    void drawPlayer()
    {
        //collisionBox->position = (position + collisionBoxOffset);
        switch (currentAnimation) {
            case WALK:
                walk_body_animation->drawAnimation({position.x, position.y, position.z}, rotation);
                if (currentThrowAnimation == THROW_PREP) {
                    throw_prep_arm_animation->drawAnimation({position.x, position.y, position.z}, rotation);
                    if (hasBall) {
                        throw_prep_ball_animation->drawAnimation({position.x, position.y, position.z}, rotation);
                    }
                    break;
                }
                walk_arm_animation->drawAnimation({position.x, position.y, position.z}, rotation);
                if (hasBall) {
                    walk_ball_animation->drawAnimation({position.x, position.y, position.z}, rotation);
                }
                break;
            case IDLE:
                if (currentThrowAnimation == THROW_PREP) {
                    throw_prep_arm_animation->drawAnimation({position.x, position.y, position.z}, rotation);
                    if (hasBall) {
                        throw_prep_ball_animation->drawAnimation({position.x, position.y, position.z}, rotation);
                    }
                    playerNoArmModel->position = {position.x, position.y, position.z};
                    playerNoArmModel->rotation = rotation;
                    playerNoArmModel->drawModel();
                    break;
                }
                playerModel->position = {position.x, position.y, position.z};
                playerModel->rotation = rotation;
                playerModel->drawModel();
                if (hasBall) {
                    ballModel->position = {position.x, position.y, position.z};
                    ballModel->rotation = rotation;
                    ballModel->drawModel();
                }
                break;
            default:
                break;
        }
    }
    ////////////////////////////////
void applyScale(float s) {
    scale  = s;
    radius = baseRadiusAtScale1 * scale;  // keep collider in sync with visual scale
}
    int life = 5; //abstraction of our collisions. You only get 5 collisions per level
    float hurtCooldown = 0.3f;           // seconds of invulnerability after a hit
    PlayerInput *playerInput= nullptr;
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
        model.Draw(animDt);       // Draw animates and renders current frame; no transforms inside
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
    Player() {
        ball = new _bullets();
        playerInput = new PlayerInput();
        //collisionBox = new _boundingBox();
        playerModel = new _model();
        ballModel = new _model();
        walk_body_animation = new _animation();
        walk_arm_animation = new _animation();
        walk_ball_animation = new _animation();
        throw_prep_arm_animation = new _animation();
        throw_prep_ball_animation = new _animation();
        playerNoArmModel = new _model();

    }
    ~Player() {
        delete ball; 
        ball = nullptr;
        delete playerInput; 
        playerInput = nullptr;

        //delete collisionBox;
        delete playerModel;
        delete ballModel;
        delete walk_body_animation;
        delete walk_arm_animation;
        delete walk_ball_animation;
        delete throw_prep_arm_animation;
        delete throw_prep_ball_animation;
        delete playerNoArmModel;
    }
};
#endif // _PLAYER_H
