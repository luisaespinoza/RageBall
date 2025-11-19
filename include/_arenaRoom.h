#ifndef _ARENAROOM_H
#define _ARENAROOM_H

#include <_common.h>
#include <_player.h>
#include <_textureloader.h>
#include <_geometryUtils.h>

struct RoomTheme {
    const char* floorTex = nullptr;
    const char* wallTex  = nullptr;
    const char* ceilTex  = nullptr;
};

class _arenaRoom {
public:
    _arenaRoom();
    ~_arenaRoom();

    void configure(float width, float height, float depth);
    void setPose(const Pose& wpose);
    void setTransform(float x, float y, float z, float yaw);
    void setSize(float w,float h, float d);
    void setCenter(const vec3& c);
    void attachLoader(_textureLoader* loader);
    bool loadTheme(const RoomTheme& t);
    void render() const;

    vec3 toLocal(const vec3& w) const;
    vec3 toWorld(const vec3& l) const;
    vec3 toWorldDir(const vec3& dirL) const;

    // midline + side logic
    void setMidlineEnabled(bool on);
    void setTeamSide(int side); // -1 left, +1 right
    vec3 clampLocal(vec3 p, float radius) const;
    bool containsLocal(const vec3& p, float radius) const;

    // object composition
    void clearObstacles();
    void addObstacleLocal(const Pose& lp, float radius, void(*drawFn)());
    void forEachObstacleWorld(
        const std::function<void(float wx,float wy,float wz,float radius)>& fn
    ) const;
    Pose pose_{ {0,0,0}, 0.f };
    vec3 center_{0,0,0};
    float w_ = 48.f, h_ = 12.f, d_ = 48.f;
    bool midline_ = true;
    int  teamSide_ = +1;
    std::vector<Obstacle> obstacles_;
    _textureLoader* texLoader_ = nullptr;
    GLuint texFloor_ = 0, texWall_ = 0, texCeil_ = 0;
private:

};
struct AABB { vec3 min, max; };
static inline AABB roomAABB(float w, float h, float d){
    vec3 half{w*0.5f, h*0.5f, d*0.5f};
    return {{-half.x,-half.y,-half.z},{half.x,half.y,half.z}};
}
static inline vec3 clampVec3(const vec3& v, const vec3& lo, const vec3& hi){
    return { std::max(lo.x, std::min(v.x, hi.x)),
             std::max(lo.y, std::min(v.y, hi.y)),
             std::max(lo.z, std::min(v.z, hi.z)) };
}
// Push out of a cylinder on XZ at center c with radius r (actor radius included)
static inline void pushOutOfCylinderXZ(vec3& p, const vec3& c, float r){
    float dx = p.x - c.x, dz = p.z - c.z;
    float d2 = dx*dx + dz*dz;
    if (d2 > 1e-8f && d2 < r*r){
        float d = std::sqrt(d2);
        float nx = dx/d, nz = dz/d;
        float t = r - d;
        p.x += nx * t;
        p.z += nz * t;
    }
}
#endif // _ARENAROOM_H
