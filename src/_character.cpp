#include "_character.h"

void Character::throwAt(const vec3& worldTarget, float speed, float spreadDeg)
{
        // mirror Player::throwAt semantics so Enemy can reuse it
    ball.src   = position;
    ball.des   = worldTarget;
    ball.pos   = ball.src;
    ball.speed = speed;
    ball.coneHalfAngleDeg = spreadDeg;

    vec3 d{ worldTarget.x - position.x,
            worldTarget.y - position.y,
            worldTarget.z - position.z };
    float L = std::sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
    if (L > 1e-6f) { d.x /= L; d.y /= L; d.z /= L; }
    ball.dir = d;

    ball.t = 0.0f;
    ball.traveled = 0.0f;
    ball.live = true;
}

void Character::updateBall(double dt)
{
        ball.bulletActions(dt);
}
