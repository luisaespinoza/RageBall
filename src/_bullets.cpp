#include "_bullets.h"


_bullets::_bullets()
{
    //ctor
    pos.x = pos.y = pos.z =0;
    t=0;
    src.x= src.y= src.z =0;
    des.x=0;
    des.y=20;
    des.z=-50;

    live= false;
}

_bullets::~_bullets()
{
    //dtor
}
vec3 _bullets::anyPerp(const vec3& n)
{
    // pick a non-parallel axis to cross with
    vec3 a = (std::fabs(n.y) < 0.99f) ? vec3{0,1,0} : vec3{1,0,0};
    // u = normalize(cross(a, n)), v = cross(n, u)
    vec3 u{ a.y*n.z - a.z*n.y, a.z*n.x - a.x*n.z, a.x*n.y - a.y*n.x };
    float Lu = std::sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
    if (Lu < 1e-6f) return vec3{1,0,0};
    u.x/=Lu; u.y/=Lu; u.z/=Lu;
    vec3 v{ n.y*u.z - n.z*u.y, n.z*u.x - n.x*u.z, n.x*u.y - n.y*u.x };
    return u; // v can be recomputed on the fly
}

vec3 _bullets::dirInCone(const vec3& axisN, float halfAngleDeg)
{
    if (halfAngleDeg <= 1e-5f) return axisN;

    // uniform over cone: cosθ in [cosA,1], φ in [0,2π)
    const float A = halfAngleDeg * (3.1415926535f/180.f);
    const float cosA = std::cos(A);
    const float u = rng01();                 // [0,1)
    const float cosT = (1.0f - u) + u*cosA;  // linear mix → uniform on cap
    const float sinT = std::sqrt(std::max(0.0f, 1.0f - cosT*cosT));
    const float phi  = 2*PI * rng01();

    // build orthonormal basis around axisN
    vec3 n = axisN;
    vec3 uvec = anyPerp(n);
    // v = n × u
    vec3 vvec{ n.y*uvec.z - n.z*uvec.y, n.z*uvec.x - n.x*uvec.z, n.x*uvec.y - n.y*uvec.x };

    // d = cosT*n + sinT*(cosφ*u + sinφ*v)
    vec3 d{
        cosT*n.x + sinT*( std::cos(phi)*uvec.x + std::sin(phi)*vvec.x ),
        cosT*n.y + sinT*( std::cos(phi)*uvec.y + std::sin(phi)*vvec.y ),
        cosT*n.z + sinT*( std::cos(phi)*uvec.z + std::sin(phi)*vvec.z )
    };
    float L = std::sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
    if (L > 1e-6f) { d.x/=L; d.y/=L; d.z/=L; }
    return d;
}


void _bullets::iniBullet(char* filename)
{
   // if we use texture on bullets
}

void _bullets::drawBullet()
{
    glDisable(GL_TEXTURE_2D);// only if you are using glut sphere
    glPushMatrix();
    if (live) {
        glTranslatef(pos.x, pos.y, pos.z);
        glutSolidSphere(radius, 20, 20);
    }
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);// only if you are using glut sphere
}

void _bullets::bulletActions(double dt) {
    switch (actionTrigger) {
    case READY:
        pos = {0,0,0};
        des = {0,0,0};
        live = false;
        t = 0.0f;
        break;

    case SHOOT:
        if (live) {
            // 1) First tick: initialize directions
            if (t == 0.0f) {
                // exact aim at destination
                vec3 d{ des.x - src.x, des.y - src.y, des.z - src.z };
                float L = std::sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
                if (L < 1e-6f) { actionTrigger = READY; live=false; break; }
                baseDir = { d.x/L, d.y/L, d.z/L };

                // seed rng (optional: use position/time etc.)
                rngState ^= (uint32_t)(*(uint32_t*)&pos.x + GOLDEN_RATIO);

                // apply cone spread in XY-around-Z or, more generally, around forward axis
                // Interpreting "error cone in the xy along z" as: spread around the shot axis,
                // which keeps the XY error while progressing along Z/forward.
                dir = dirInCone(baseDir, coneHalfAngleDeg);

                pos = src; traveled = 0.0f; t = 1.0f; // mark initialized
            }


            if (trajectory_) {
                // Let level define the rules of travel
                trajectory_(*this, dt);
            } else {
                // Fallback: straight step at current speed along 'dir'
                const float step = speed * (float)dt;
                pos.x += dir.x * step;
                pos.y += dir.y * step;
                pos.z += dir.z * step;
                traveled += step;
            }


            // 2) Termination: do NOT snap to destination
            // stop only on range (collisions can deactivate elsewhere)
            if (traveled >= maxRange) {
                live = false;
                actionTrigger = READY;
                t = 0.0f;
            }
            }
        break;

    case HIT:
        break;
    }
}
