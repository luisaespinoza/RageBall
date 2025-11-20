#include "_enemy.h"

static inline float dot2D(const vec3& a, const vec3& b) { return a.x*b.x + a.z*b.z; }
static inline float len2D(const vec3& v) { return std::sqrt(v.x*v.x + v.z*v.z); }



bool Enemy::init(const char* md2Path, const char* texturePath, _textureLoader& loader) {
    // Ensure the model can use the same loader path you use for Player
    model.myTex = &loader;                                           //
    int ok = model.initModel(md2Path);                               //
    if (!ok) {
        std::cerr << "[Enemy] initModel failed for: " << md2Path << "\n";
        return false;
    }

    GLuint tid = 0;
    if (texturePath && texturePath[0] != '\0') {
        tid = loader.loadTexture(const_cast<char*>(texturePath));    //
        model.md2file.tex_id = tid;                                  //
    }
    model.md2file.tex_id = tid;                                      // keep consistent with Player
    model.Actions();                                                 // set initial anim (STAND)

    // sync collider to current scale (same convention as Player)
    radius = baseRadiusAtScale1 * scale;                             //
//    return tid != 0;
    return ok;
}

void Enemy::setAnimForVelocity(const vec3& vW) {
    // same heuristic as Player: RUN if moving on XZ, else STAND
    bool moving = (std::fabs(vW.x) + std::fabs(vW.z)) > 0.01f;       //
    model.actionTrigger = moving ? model.RUN : model.STAND;          //
    model.Actions();                                                 // updates StartFrame/EndFrame
}

void Enemy::render() {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glRotatef(baseYawMD2 + yawDeg, 0, 1, 0);                         // same as Player
    glRotatef(-90.0f, 1, 0, 0);                                      // MD2 -> your up-axis
    glScalef(scale, scale, scale);                                   // visual scale
    model.Draw(animDt);                                                    // draw current frame (and animate)
    glPopMatrix();

    // draw projectile too (unchanged)
    ball.drawBullet();
}
