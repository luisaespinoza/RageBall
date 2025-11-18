#include "_hallway.h"
//for debugging. leave false if pushing to production
static bool kWorldSpaceTest = true;
_hallway::_hallway()
{
    //ctor
}

_hallway::~_hallway()
{
    //dtor
}
vec3 normalize(const vec3& v)
{
    float L = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    return (L > 1e-6f) ? vec3{ v.x/L, v.y/L, v.z/L } : vec3{0,0,0};
}
float _hallway::yawFromDirDeg(const vec3& dirW)
{
    // local forward is -Z; yaw rotates local into world
    // world forward direction is along +dirW (normalized)
    // yaw = atan2(x, -z) in degrees
    float L = sqrtf(dirW.x*dirW.x + dirW.z*dirW.z);
    if (L < 1e-6f) return 0.0f;
    return atan2f(dirW.x / L, -dirW.z / L) * 180.0f / 3.14159265f;
}
void _hallway::forEachObstacleWorld(
    const std::function<void(float wx, float wy, float wz, float radius)>& fn
) const
{
    const float yaw = pose_.yawDeg * 3.1415926f / 180.f;
    const float cs = cosf(yaw), sn = sinf(yaw);

    for (const auto& o : obstacles_) {
        const float lx = o.local.p.x;
        const float ly = o.local.p.y;
        const float lz = o.local.p.z;

        // rotate local (x,z) by yaw, then translate by hallway origin
        const float wx = pose_.p.x + ( lx * cs + lz * sn );
        const float wy = pose_.p.y +   ly;
        const float wz = pose_.p.z + (-lx * sn + lz * cs );

        fn(wx, wy, wz, o.radius);
    }
}
void _hallway::configure(float width, float height, float length, int segments) {
    w_ = width; h_ = height; L_ = length; segs_ = (segments > 0 ? segments : 1);
}
void _hallway::setTiling(float floorU, float floorV, float wallU, float wallV, float ceilU, float ceilV) {
    fU_=floorU; fV_=floorV; wlU_=wallU; wlV_=wallV; cU_=ceilU; cV_=ceilV;
}
void _hallway::setTransform(float x, float y, float z, float yaw) {
    pose_.p = {x,y,z}; pose_.yawDeg = yaw;
}
void _hallway::attachLoader(_textureLoader* loader) { texLoader_ = loader; }

bool _hallway::loadTheme(const HallwayTheme& t) {
    if (!texLoader_) return false;
    if (t.floorTex) texFloor_ = texLoader_->loadTexture(const_cast<char*>(t.floorTex));
    if (t.wallTex)  texWall_  = texLoader_->loadTexture(const_cast<char*>(t.wallTex));
    if (t.ceilTex)  texCeil_  = texLoader_->loadTexture(const_cast<char*>(t.ceilTex));
    printf("[theme] floor=%u wall=%u ceil=%u\n", texFloor_, texWall_, texCeil_);
    return texFloor_ && texWall_ && texCeil_;
}
void _hallway::render() const {
    glMatrixMode(GL_TEXTURE);  glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();

    if (!kWorldSpaceTest) {
        glTranslatef(pose_.p.x, pose_.p.y, pose_.p.z);
        glRotatef(pose_.yawDeg + debugExtraYawDeg, 0, 1, 0);
    }

    for (int i = 0; i < segs_; ++i) {
        if (kWorldSpaceTest) renderSegmentWorld(i);
        else                 renderSegmentLocal(i);
    }

    // Draw local-space obstacles (converted to world)
    for (const auto& obs : obstacles_) {                 // obstacles_ exists here
        glPushMatrix();
        vec3 w = toWorld(obs.local.p);
        glTranslatef(w.x, w.y, w.z);
        glScalef(obs.radius, obs.radius, obs.radius);
        if (obs.drawFn) obs.drawFn();                   // e.g., DrawCubeInstance()
        glPopMatrix();
    }

    glPopMatrix();
    glPopAttrib();  // restore texture/enable state
}
void _hallway::renderSegment(int i) const {
    // segment bounds along +Z (you can flip if desired)
    const float segLen = L_ / segs_;
    const float z0 = -i * segLen;
    const float z1 = -(i+1) * segLen;
    const float xL = -w_ * 0.5f;
    const float xR =  w_ * 0.5f;
    const float yB = 0.0f;
    const float yT = h_;

    //LightNormals
    // floor   n=(0,1,0)
    // ceiling n=(0,-1,0)
    // right   n=(+1,0,0)
    // left    n=(-1,0,0)

    // ---- FLOOR ----
    GLint b = 0; glGetIntegerv(GL_TEXTURE_BINDING_2D, &b);
//std::cout << "[Hall floor pre] bound=" << b << " mine=" << texFloor_ << "\n";
glBindTexture(GL_TEXTURE_2D, texFloor_);
glGetIntegerv(GL_TEXTURE_BINDING_2D, &b);
//std::cout << "[Hall floor post] bound=" << b << "\n";

    if (texFloor_) glBindTexture(GL_TEXTURE_2D, texFloor_);
    glBegin(GL_QUADS);
      glNormal3f(0,1,0);
      glTexCoord2f(0,     0);     glVertex3f(xL, yB, z0);
      glTexCoord2f(fU_,   0);     glVertex3f(xR, yB, z0);
      glTexCoord2f(fU_, fV_);     glVertex3f(xR, yB, z1);
      glTexCoord2f(0,   fV_);     glVertex3f(xL, yB, z1);
    glEnd();

    // ---- CEILING ----
    if (texCeil_) glBindTexture(GL_TEXTURE_2D, texCeil_);
    glBegin(GL_QUADS);
      glNormal3f(0,-1,0);
      glTexCoord2f(0,     0);     glVertex3f(xL, yT, z1);
      glTexCoord2f(cU_,   0);     glVertex3f(xR, yT, z1);
      glTexCoord2f(cU_, cV_);     glVertex3f(xR, yT, z0);
      glTexCoord2f(0,   cV_);     glVertex3f(xL, yT, z0);
    glEnd();

    // ---- RIGHT WALL (x = +w/2) ----
    if (texWall_) glBindTexture(GL_TEXTURE_2D, texWall_);
    glBegin(GL_QUADS);
      glNormal3f(-1,0,0);
      glTexCoord2f(0,     0);     glVertex3f(xR, yB, z1);
      glTexCoord2f(wlU_,  0);     glVertex3f(xR, yB, z0);
      glTexCoord2f(wlU_, wlV_);   glVertex3f(xR, yT, z0);
      glTexCoord2f(0,   wlV_);    glVertex3f(xR, yT, z1);
    glEnd();

    // ---- LEFT WALL (x = -w/2) ----
    if (texWall_) glBindTexture(GL_TEXTURE_2D, texWall_);
    glBegin(GL_QUADS);
      glNormal3f(1,0,0);
      glTexCoord2f(0,     0);     glVertex3f(xL, yB, z0);
      glTexCoord2f(wlU_,  0);     glVertex3f(xL, yB, z1);
      glTexCoord2f(wlU_, wlV_);   glVertex3f(xL, yT, z1);
      glTexCoord2f(0,   wlV_);    glVertex3f(xL, yT, z0);
    glEnd();

}
void _hallway::getEndPose(Pose& out) const {
    float yawRad = pose_.yawDeg * 3.14159265f / 180.f;
    float dx =  L_ * std::sin(yawRad);
    float dz = -L_ * std::cos(yawRad);
    out.p.x = pose_.p.x + dx;  out.p.y = pose_.p.y;  out.p.z = pose_.p.z + dz;
    out.yawDeg = pose_.yawDeg;
}

void _hallway::attachNext(_hallway& next, float turnDeg) const {
    Pose end{}; getEndPose(end);
    end.yawDeg += turnDeg;
    next.setPose(end);
}

void _hallway::setPose(const Pose& wpose)
{
     pose_ = wpose;
}

void _hallway::addObstacleLocal(const Pose& lp, float radius, void(* drawFn)())
{
    obstacles_.push_back(Obstacle{lp, radius, drawFn});
}
vec3 _hallway::toLocal(const vec3& w) const
{
    float r = -pose_.yawDeg * (3.14159265f/180.f);
    vec3 t = { w.x - pose_.p.x, w.y - pose_.p.y, w.z - pose_.p.z };
    return { t.x*cosf(r) - t.z*sinf(r), t.y, t.x*sinf(r) + t.z*cosf(r) };
}

vec3 _hallway::toWorld(const vec3& l) const
{
    float r =  pose_.yawDeg * (3.14159265f/180.f);
    vec3 xz = { l.x*cosf(r) - l.z*sinf(r), l.x*sinf(r) + l.z*cosf(r) };
    return { xz.x + pose_.p.x, l.y + pose_.p.y, xz.y + pose_.p.z };
}

vec3 _hallway::clampLocal(vec3 p, float rad) const
{
        float half = 0.5f*w_;
    if (p.x < -half+rad) p.x = -half+rad;
    if (p.x >  half-rad) p.x =  half-rad;
    if (p.y < 0+rad)     p.y = 0+rad;
    if (p.y > h_-rad)    p.y = h_-rad;
    // z: only clamp if this end has no neighbor; otherwise allow crossing
    return p;
}
bool _hallway::containsLocal(const vec3& p, float rad) const
{
    float half = 0.5f*w_;
    return (p.x > -half+rad && p.x < half-rad &&
            p.y > 0+rad     && p.y < h_-rad     &&
            p.z > -L_       && p.z < 0); // relax z if neighbor exists
}


vec3 _hallway::toWorldDir(const vec3& dirL) const
{
    vec3 p0 = toWorld({0,0,0});
    vec3 p1 = toWorld(dirL);
    vec3 d  = { p1.x - p0.x, p1.y - p0.y, p1.z - p0.z };
    return normalize(d);
}

void _hallway::attachNextExact(_hallway& next, float turnDeg) const
{
    // 1) Snap next hall's position to THIS hall's far cap (local (0,0,-L))
    vec3 A_endW = toWorld({0,0,-L_});

    // 2) Base yaw equals this hallway's world yaw, then apply the turn
    float baseYaw = hallwayWorldYawDeg();
    float yawNext = baseYaw + turnDeg;

    // 3) Place and orient 'next' exactly at the seam
    next.setTransform(A_endW.x, A_endW.y, A_endW.z, yawNext);

    // 4)  pull 'next' backward by a tiny epsilon along its local +Z
    //        (local forward is -Z; we want a slight overlap into A)
    const float eps = 0.005f; // tweak between 0.001–0.02 depending on your scale
    vec3 backW = next.toWorldDir({0,0, +1});  // local +Z
    next.setTransform(next.pose_.p.x - backW.x * eps,
                      next.pose_.p.y - backW.y * eps,
                      next.pose_.p.z - backW.z * eps,
                      next.pose_.yawDeg);
}


void _hallway::renderSegmentLocal(int i) const
{
    const float segLen = L_ / segs_;
    const float z0 = -i * segLen, z1 = -(i+1) * segLen;
    const float xL = -w_*0.5f, xR = w_*0.5f, yB = 0.0f, yT = h_;

        if (texFloor_) glBindTexture(GL_TEXTURE_2D, texFloor_);
    glBegin(GL_QUADS);
      glNormal3f(0,1,0);
      glTexCoord2f(0,     0);     glVertex3f(xL, yB, z0);
      glTexCoord2f(fU_,   0);     glVertex3f(xR, yB, z0);
      glTexCoord2f(fU_, fV_);     glVertex3f(xR, yB, z1);
      glTexCoord2f(0,   fV_);     glVertex3f(xL, yB, z1);
    glEnd();

    // ---- CEILING ----
    if (texCeil_) glBindTexture(GL_TEXTURE_2D, texCeil_);
    glBegin(GL_QUADS);
      glNormal3f(0,-1,0);
      glTexCoord2f(0,     0);     glVertex3f(xL, yT, z1);
      glTexCoord2f(cU_,   0);     glVertex3f(xR, yT, z1);
      glTexCoord2f(cU_, cV_);     glVertex3f(xR, yT, z0);
      glTexCoord2f(0,   cV_);     glVertex3f(xL, yT, z0);
    glEnd();

    // ---- RIGHT WALL (x = +w/2) ----
    if (texWall_) glBindTexture(GL_TEXTURE_2D, texWall_);
    glBegin(GL_QUADS);
      glNormal3f(-1,0,0);
      glTexCoord2f(0,     0);     glVertex3f(xR, yB, z1);
      glTexCoord2f(wlU_,  0);     glVertex3f(xR, yB, z0);
      glTexCoord2f(wlU_, wlV_);   glVertex3f(xR, yT, z0);
      glTexCoord2f(0,   wlV_);    glVertex3f(xR, yT, z1);
    glEnd();

    // ---- LEFT WALL (x = -w/2) ----
    if (texWall_) glBindTexture(GL_TEXTURE_2D, texWall_);
    glBegin(GL_QUADS);
      glNormal3f(1,0,0);
      glTexCoord2f(0,     0);     glVertex3f(xL, yB, z0);
      glTexCoord2f(wlU_,  0);     glVertex3f(xL, yB, z1);
      glTexCoord2f(wlU_, wlV_);   glVertex3f(xL, yT, z1);
      glTexCoord2f(0,   wlV_);    glVertex3f(xL, yT, z0);
    glEnd();
}

void _hallway::renderSegmentWorld(int i) const
{
    {
    const float segLen = L_ / segs_;
    const float z0 = -i * segLen, z1 = -(i+1) * segLen;
    const float xL = -w_*0.5f, xR = w_*0.5f, yB = 0.0f, yT = h_;
    auto W = [&](float x, float y, float z){ return toWorld({x,y,z}); };

    if (texFloor_) glBindTexture(GL_TEXTURE_2D, texFloor_);
    glBegin(GL_QUADS);
      glNormal3f(0,1,0);
      {auto v=W(xL,yB,z0); glTexCoord2f(0,   0);   glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yB,z0); glTexCoord2f(fU_, 0);   glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yB,z1); glTexCoord2f(fU_, fV_); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yB,z1); glTexCoord2f(0,   fV_); glVertex3f(v.x,v.y,v.z);}
    glEnd();

    if (texCeil_) glBindTexture(GL_TEXTURE_2D, texCeil_);
    glBegin(GL_QUADS);
      glNormal3f(0,-1,0);
      {auto v=W(xL,yT,z1); glTexCoord2f(0,   0);   glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,z1); glTexCoord2f(cU_, 0);   glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,z0); glTexCoord2f(cU_, cV_); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yT,z0); glTexCoord2f(0,   cV_); glVertex3f(v.x,v.y,v.z);}
    glEnd();

    if (texWall_) glBindTexture(GL_TEXTURE_2D, texWall_);
    // RIGHT wall
    glBegin(GL_QUADS);
      glNormal3f(-1,0,0);
      {auto v=W(xR,yB,z1); glTexCoord2f(0,    0);    glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yB,z0); glTexCoord2f(wlU_, 0);    glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,z0); glTexCoord2f(wlU_, wlV_); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xR,yT,z1); glTexCoord2f(0,    wlV_); glVertex3f(v.x,v.y,v.z);}
    glEnd();
    // LEFT wall
    glBegin(GL_QUADS);
      glNormal3f(1,0,0);
      {auto v=W(xL,yB,z0); glTexCoord2f(0,    0);    glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yB,z1); glTexCoord2f(wlU_, 0);    glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yT,z1); glTexCoord2f(wlU_, wlV_); glVertex3f(v.x,v.y,v.z);}
      {auto v=W(xL,yT,z0); glTexCoord2f(0,    wlV_); glVertex3f(v.x,v.y,v.z);}
    glEnd();
    }

}





