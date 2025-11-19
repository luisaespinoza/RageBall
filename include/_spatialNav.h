#ifndef _SPATIALNAV_H
#define _SPATIALNAV_H
#include<_common.h>

// -- Concept-ish: any Space type with these member fns works:
//    space.toLocal(vec3), space.toWorld(vec3), space.toWorldDir(vec3),
//    space.clampLocal(vec3,float), space.containsLocal(vec3,float)

// degrees↔radians helpers
inline float DegToRad(float d){ return d * 3.1415926535f / 180.f; }
inline float RadToDeg(float r){ return r * 180.f / 3.1415926535f; }

// Convert a local yaw (0° = local -Z forward) into a world yaw using only toWorldDir
template<class Space>
inline float worldYawFromLocal(const Space& S, float yawLocalDeg)
{
    // local forward for a given yaw about +Y in your coordinate system:
    // x = sin(yaw), z = -cos(yaw)  (because 0° faces local -Z)
    const float r = DegToRad(yawLocalDeg);
    vec3 dirL{ std::sin(r), 0.f, -std::cos(r) };
    vec3 dW   = S.toWorldDir(dirL);
    return RadToDeg(std::atan2(dW.x, -dW.z));
}

// Move a world-position by a world delta and clamp inside the Space
template<class Space>
inline vec3 moveAndClamp(const Space& S, const vec3& worldPos, const vec3& worldDelta, float radius)
{
    vec3 pW  = { worldPos.x + worldDelta.x, worldPos.y + worldDelta.y, worldPos.z + worldDelta.z };
    vec3 pL  = S.toLocal(pW);
    vec3 cL  = S.clampLocal(pL, radius);
    return S.toWorld(cL);
}

// World-space containment test using Space’s local predicates
template<class Space>
inline bool containsWorld(const Space& S, const vec3& worldPos, float radius)
{
    return S.containsLocal(S.toLocal(worldPos), radius);
}

// Scale-aware “muzzle” computed from Space forward (local -Z)
template<class Space>
inline vec3 computeMuzzle(const Space& S, const vec3& actorPos, float actorRadius)
{
    vec3 fwd = S.toWorldDir({0,0,-1});
    return {
        actorPos.x + fwd.x * (0.30f * actorRadius),
        actorPos.y + 1.20f  * actorRadius,
        actorPos.z + fwd.z * (0.30f * actorRadius)
    };
}

#endif // _SPATIALNAV_H
