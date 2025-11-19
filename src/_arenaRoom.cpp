#include "_arenaRoom.h"

_arenaRoom::_arenaRoom()
{
    //ctor
}

_arenaRoom::~_arenaRoom()
{
    //dtor
}

// ---- configuration & theming ----
void _arenaRoom::configure(float width, float height, float depth) {
    w_ = width; h_ = height; d_ = depth;
}

void _arenaRoom::setPose(const Pose& wpose) {
    pose_ = wpose;
}

void _arenaRoom::setTransform(float x, float y, float z, float yawDeg) {
    pose_.p = {x,y,z};
    pose_.yawDeg = yawDeg;
}

void _arenaRoom::attachLoader(_textureLoader* loader) {
    texLoader_ = loader;
}

bool _arenaRoom::loadTheme(const RoomTheme& t) {
    if (!texLoader_) return false;
    if (t.floorTex) texFloor_ = texLoader_->loadTexture(const_cast<char*>(t.floorTex));
    if (t.wallTex)  texWall_  = texLoader_->loadTexture(const_cast<char*>(t.wallTex));
    if (t.ceilTex)  texCeil_  = texLoader_->loadTexture(const_cast<char*>(t.ceilTex));
    return texFloor_ && texWall_ && texCeil_;
}

// ---- local/world helpers (same math as hallway) ----
vec3 _arenaRoom::toLocal(const vec3& w) const {
    float r = -pose_.yawDeg * (PI/180.f);
    vec3 t = { w.x - pose_.p.x, w.y - pose_.p.y, w.z - pose_.p.z };
    return { t.x*cosf(r) - t.z*sinf(r), t.y, t.x*sinf(r) + t.z*cosf(r) };
}

vec3 _arenaRoom::toWorld(const vec3& l) const {
    float r =  pose_.yawDeg * (PI/180.f);
    vec3 xz = { l.x*cosf(r) - l.z*sinf(r), l.x*sinf(r) + l.z*cosf(r) };
    return { xz.x + pose_.p.x, l.y + pose_.p.y, xz.y + pose_.p.z };
}

vec3 _arenaRoom::toWorldDir(const vec3& dirL) const {
    vec3 p0 = toWorld({0,0,0});
    vec3 p1 = toWorld(dirL);
    vec3 d  = { p1.x - p0.x, p1.y - p0.y, p1.z - p0.z };
    float L = sqrtf(d.x*d.x + d.y*d.y + d.z*d.z);
    return (L>1e-6f) ? vec3{d.x/L, d.y/L, d.z/L} : vec3{0,0,0};
}

// ---- arena rules ----
void _arenaRoom::setMidlineEnabled(bool on) { midline_ = on; }
void _arenaRoom::setTeamSide(int s)         { teamSide_ = (s >= 0 ? +1 : -1); }

vec3 _arenaRoom::clampLocal(vec3 p, float rad) const {
    const float hx = 0.5f * w_, hz = 0.5f * d_;

    // --- 1) base box clamp (X/Y) ---
    if (p.x < -hx + rad) p.x = -hx + rad;
    if (p.x >  hx - rad) p.x =  hx - rad;
    if (p.y <     0 + rad) p.y = 0 + rad;
    if (p.y >    h_ - rad) p.y = h_ - rad;

    // --- 2) midline clamp (Z=0 split) ---
    if (midline_) {
        if (teamSide_ >= 0) {          // home side is z ≥ 0
            if (p.z < 0 + rad) p.z = 0 + rad;
        } else {                       // home side is z ≤ 0
            if (p.z > 0 - rad) p.z = 0 - rad;
        }
    }

    // --- 3) back-wall depth clamp (Z) ---
    if (p.z < -hz + rad) p.z = -hz + rad;
    if (p.z >  hz - rad) p.z =  hz - rad;

    // --- 4) obstacle push-out (XZ cylinders), inflated by actor radius ---
    // NOTE: we only nudge on XZ so Y stays box-clamped.
    for (const auto& o : obstacles_) {
        const float R = o.radius + rad;
        // local pose: o.lp.pos is already LOCAL; no transform needed
        pushOutOfCylinderXZ(p, o.local.p, R);

        // after each push, re-enforce midline & depth so obstacles don’t shove across the line
        if (midline_) {
            if (teamSide_ >= 0 && p.z < 0 + rad) p.z = 0 + rad;
            if (teamSide_  < 0 && p.z > 0 - rad) p.z = 0 - rad;
        }
        if (p.z < -hz + rad) p.z = -hz + rad;
        if (p.z >  hz - rad) p.z =  hz - rad;

        // also re-enforce X bounds in case push moved us out
        if (p.x < -hx + rad) p.x = -hx + rad;
        if (p.x >  hx - rad) p.x =  hx - rad;
    }

    return p;
}

bool _arenaRoom::containsLocal(const vec3& p, float rad) const {
    const float hx = 0.5f * w_, hz = 0.5f * d_;

    // box bounds (X/Y)
    if (p.x <= -hx + rad || p.x >=  hx - rad) return false;
    if (p.y <=    0 + rad || p.y >=   h_ - rad) return false;

    // midline split
    if (midline_) {
        if (teamSide_ >= 0 && p.z < 0 + rad) return false;
        if (teamSide_  < 0 && p.z > 0 - rad) return false;
    }

    // depth (Z) walls
    if (p.z <= -hz + rad || p.z >= hz - rad) return false;

    // obstacle overlap (inflate by actor radius)
    for (const auto& o : obstacles_) {
        const float R = o.radius + rad;
        const float dx = p.x - o.local.p.x;
        const float dz = p.z - o.local.p.z;
        if (dx*dx + dz*dz < R*R) return false;
    }

    return true;
}
// ---- composition ----
void _arenaRoom::addObstacleLocal(const Pose& lp, float radius, void(*drawFn)()) {
    obstacles_.push_back(Obstacle{lp, radius, drawFn});
}

void _arenaRoom::forEachObstacleWorld(
    const std::function<void(float wx, float wy, float wz, float radius)>& fn
) const {
    const float yaw = pose_.yawDeg * PI / 180.f;
    const float cs = cosf(yaw), sn = sinf(yaw);

    for (const auto& o : obstacles_) {
        const float lx = o.local.p.x;
        const float ly = o.local.p.y;
        const float lz = o.local.p.z;

        const float wx = pose_.p.x + ( lx * cs + lz * sn );
        const float wy = pose_.p.y +   ly;
        const float wz = pose_.p.z + (-lx * sn + lz * cs );

        fn(wx, wy, wz, o.radius);
    }
}

// ---- render (single box with floor/ceiling/walls; local then transformed via toWorld) ----
void _arenaRoom::render() const {
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glEnable(GL_TEXTURE_2D);

    auto W = [&](float x, float y, float z){ return toWorld({x,y,z}); };

    const float xL = -w_*0.5f, xR =  w_*0.5f;
    const float zB = -d_*0.5f, zF =  d_*0.5f;
    const float yB = 0.0f,     yT = h_;

    // FLOOR
    if (texFloor_) glBindTexture(GL_TEXTURE_2D, texFloor_);
    glBegin(GL_QUADS);
      glNormal3f(0,1,0);
      {auto v=W(xL,yB,zB); glTexCoord2f(0,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yB,zB); glTexCoord2f(1,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yB,zF); glTexCoord2f(1,1); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yB,zF); glTexCoord2f(0,1); glVertex3f(v.x,v.y,v.z);}
    glEnd();

    // CEILING
    if (texCeil_) glBindTexture(GL_TEXTURE_2D, texCeil_);
    glBegin(GL_QUADS);
      glNormal3f(0,-1,0);
      {auto v=W(xL,yT,zF); glTexCoord2f(0,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,zF); glTexCoord2f(1,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,zB); glTexCoord2f(1,1); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yT,zB); glTexCoord2f(0,1); glVertex3f(v.x,v.y,v.z);}
    glEnd();

    // RIGHT WALL (x = +)
    if (texWall_) glBindTexture(GL_TEXTURE_2D, texWall_);
    glBegin(GL_QUADS);
      glNormal3f(1,0,0);
      {auto v=W(xR,yB,zB); glTexCoord2f(0,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yB,zF); glTexCoord2f(1,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,zF); glTexCoord2f(1,1); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,zB); glTexCoord2f(0,1); glVertex3f(v.x,v.y,v.z);}
    glEnd();

    // LEFT WALL (x = -)
    if (texWall_) glBindTexture(GL_TEXTURE_2D, texWall_);
    glBegin(GL_QUADS);
      glNormal3f(-1,0,0);
      {auto v=W(xL,yB,zF); glTexCoord2f(0,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yB,zB); glTexCoord2f(1,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yT,zB); glTexCoord2f(1,1); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yT,zF); glTexCoord2f(0,1); glVertex3f(v.x,v.y,v.z);}
    glEnd();

    // BACK WALL (z = -)
    if (texWall_) glBindTexture(GL_TEXTURE_2D, texWall_);
    glBegin(GL_QUADS);
      glNormal3f(0,0,1);
      {auto v=W(xL,yB,zB); glTexCoord2f(0,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yB,zB); glTexCoord2f(1,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,zB); glTexCoord2f(1,1); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yT,zB); glTexCoord2f(0,1); glVertex3f(v.x,v.y,v.z);}
    glEnd();

    // FRONT WALL (z = +)
    if (texWall_) glBindTexture(GL_TEXTURE_2D, texWall_);
    glBegin(GL_QUADS);
      glNormal3f(0,0,-1);
      {auto v=W(xR,yB,zF); glTexCoord2f(0,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yB,zF); glTexCoord2f(1,0); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yT,zF); glTexCoord2f(1,1); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,zF); glTexCoord2f(0,1); glVertex3f(v.x,v.y,v.z);}
    glEnd();

    // (Optional) draw local obstacles
    for (const auto& obs : obstacles_) {
        glPushMatrix();
        vec3 w = toWorld(obs.local.p);
        glTranslatef(w.x, w.y, w.z);
        glScalef(obs.radius, obs.radius, obs.radius);
        if (obs.drawFn) obs.drawFn();
        glPopMatrix();
    }

    glPopAttrib();
}
void _arenaRoom::clearObstacles(){ obstacles_.clear(); }

void _arenaRoom::setSize(float w,float h,float d){ w_ = w; h_ = h; d_ = d; }
void _arenaRoom::setCenter(const vec3& c){ center_ = c; }
