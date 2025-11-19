#ifndef _HALLWAY_H
#define _HALLWAY_H
#include <_common.h>
#include<_player.h>
#include<_geometryUtils.h>
struct HallwayTheme {
    const char* floorTex = nullptr;
    const char* wallTex  = nullptr; // used for both left/right
    const char* ceilTex  = nullptr;
};

vec3 normalize(const vec3& v);
//static float yawFromDirDeg(const vec3& dirW);
class _hallway
{
    public:
        _hallway();
        ~_hallway();
        static float yawFromDirDeg(const vec3& dirW);
        void setPose(const Pose& wpose);
        void addObstacleLocal(const Pose& lp, float radius, void (*drawFn)());
    // dimensions in world units, segments splits length into equal chunks
        void configure(float width, float height, float length, int segments);

    // UV tiling (how many repeats across a single segment)
        void setTiling(float floorU, float floorV, float wallU, float wallV, float ceilU, float ceilV);

    // set transform
        void setTransform(float x, float y, float z, float yaw = 0.f);

    // connect loader and load a theme (floor/wall/ceiling)
        void attachLoader(_textureLoader* loader);
        bool loadTheme(const HallwayTheme& t);

            void forEachObstacleWorld(
        const std::function<void(float wx, float wy, float wz, float radius)>& fn
    ) const;

    // draw
        void render() const;
        void renderSegmentLocal(int i) const;
        void renderSegmentWorld(int i )const;
          // World-space pose of the far end (where the next hallway should attach)
        void getEndPose(Pose& out) const;

  // Position & rotate 'next' so it starts where *this* ends, then turn by deg
        void attachNext(_hallway& next, float turnDeg /* e.g., +90 or -90 */) const;
        void attachNextExact(_hallway& next, float turnDeg) const;
        vec3 toLocal(const vec3& w) const;
        vec3 toWorld(const vec3& l) const;
        vec3 toWorldDir(const vec3& dirL) const;
        bool containsLocal(const vec3& p, float rad) const ;

        vec3 clampLocal(vec3 p, float rad) const ;
        float length() const {return L_;};
    // Compute this hallway's world yaw (degrees) from its current transform.
    // We map local forward (-Z) to world space and read yaw via atan2.
    inline float hallwayWorldYawDeg() const {
        // World positions of local origin and a point one unit forward (-Z) in local space
        vec3 wO = toWorld(vec3{0.0f, 0.0f,  0.0f});
        vec3 wF = toWorld(vec3{0.0f, 0.0f, -1.0f});
        vec3 d; d.x = wF.x - wO.x; d.y = wF.y - wO.y; d.z = wF.z - wO.z;

        // Heading angle where forward is -Z (so yaw = atan2(x, -z))
        float yawRad = atan2f(d.x, -d.z);
        return yawRad * 180.0f / PI;
    }

    // Convert a local yaw (degrees, where 0° faces local -Z) to world yaw.
    inline float worldYawFromLocal(float localYawDeg) const {
        return hallwayWorldYawDeg() + localYawDeg;
    }

    // Convert a world yaw back into this hallway's local frame.
    inline float localYawFromWorld(float worldYawDeg) const {
        return worldYawDeg - hallwayWorldYawDeg();
    }

    // Place the player using hallway-local pose (position & local yaw), converted once to world.
    inline void placePlayerLocal(Player& p, const vec3& pLocal, float yawLocalDeg) const {
    p.position = toWorld(pLocal);
    p.yawDeg   = worldYawFromLocal(yawLocalDeg);
}
        float debugExtraYawDeg= 0.0f;

        Pose pose_{ {0,0,0}, 0.f };
        float w_ = 4.f, h_ = 3.f, L_ = 20.f;
        std::vector<Obstacle> obstacles_;
        _textureLoader* texLoader_ = nullptr;
        GLuint texFloor_ = 0, texWall_ = 0, texCeil_ = 0;

        int   segs_ = 10;

// tiling per segment
        float fU_ = 1.f, fV_ = 1.f;
        float wlU_= 1.f, wlV_= 1.f;
        float cU_ = 1.f, cV_ = 1.f;
        void offsetYaw(float deltaDeg);
        //void getTransform(float& x, float& y, float& z, float& yawDeg) const;
        // transform
       //float px_=0, py_=0, pz_=0, ry_=0;
        struct Theme {
            GLuint floor = 0, wall = 0, ceil = 0;
        };
        Theme theme_;
        int tileFx=2,tileFy=2, tileWx=2,tileWy=2, tileCx=2,tileCy=2;

        void setThemeIds(GLuint f, GLuint w, GLuint c) {
            theme_.floor = f; theme_.wall = w; theme_.ceil = c;
        }

        void setTilingAll(int fx,int fy,int wx,int wy,int cx,int cy){
            tileFx=fx; tileFy=fy; tileWx=wx; tileWy=wy; tileCx=cx; tileCy=cy;
        }


    private:






    // helpers
        void renderSegment(int i) const;
};

#endif // _HALLWAY_H_H
