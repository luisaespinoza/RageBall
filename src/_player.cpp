

#include <_player.h>
#include <_hallway.h>

void Player::moveAndClamp(double /*dt*/, const vec3& vel, _hallway& hall) {
    // integrate in world
    vec3 newPos{ position.x + vel.x, position.y + vel.y, position.z + vel.z };

    // hallway-local clamp
    vec3 pL = hall.toLocal(newPos);
    pL = hall.clampLocal(pL, radius);
    position = hall.toWorld(pL);
}
