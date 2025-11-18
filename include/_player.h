#ifndef _PLAYER_H
#define _PLAYER_H
#include<_3dmodelloader.h>
#include<_textureloader.h>

struct _hallway;


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
void applyScale(float s) {
    scale  = s;
    radius = baseRadiusAtScale1 * scale;  // keep collider in sync with visual scale
}
    int life = 5; //abstraction of our collisions. You only get 5 collisions per level
    float hurtCooldown = 0.3f;           // seconds of invulnerability after a hit
    PlayerInput *playerInput= new PlayerInput();
    inline void playerResetLife( int newLife=5) {
        life = life;
        hurtCooldown = 0.f;
    }
    // call once after GL is ready
    bool init(const char* md2Path, const char* texturePath, _textureLoader& loader) {
        // Make sure model has a usable texture:
        // either tweak _3DModelLoader to accept a path, or load with your loader
        // and set mdl.tex_id. (Its current code always loads a hardcoded blade.jpg.)
       // std:: cout << "entering init"<<endl;
        model.myTex = &loader;                // ensure non-null before ReadMD2Model
        //std::cout <<"thisthisthisthis"<<endl;
        int ok = model.initModel(md2Path);
        if (!ok) {
            std::cerr << "initModel failed for: " << md2Path << "\n";
            return false;                // <-- early out: don’t load texture, don’t proceed
        }           // reads MD2 & sets md2file
        //std:: cout<< "init paased"<<endl;
        // If needed, force a texture via your loader:
        GLuint tid = 0;
        if (texturePath && texturePath[0] != '\0') {
            tid = loader.loadTexture(const_cast<char*>(texturePath));
            model.md2file.tex_id = tid;  // assign only if we actually loaded it
        }

        model.md2file.tex_id = tid;           // ensure the model uses your texture

        model.Actions();                      // set initial animation (defaults to STAND)
        return tid != 0;
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
    void moveAndClamp(double dt, const vec3& vel, _hallway& hall);

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
};
#endif // _PLAYER_H
