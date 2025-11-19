#include "_pickup.h"

_pickup::_pickup()
{
    //ctor
    isCollected = false;
    // Values set to 0 as default, should be set on spawn.
    life = 0;
    speed = 0;
}

_pickup::~_pickup()
{
    //dtor
}

void _pickup::applyEffect(Player* player)
{
    switch(pickupType) {
        case HEALTH:
            player->life += life; // Increase player's life by 1 (can be changed)
            break;
        case SPEED:
            player->speed += speed; // Increase player's speed (can be changed)
            break;
    }
    isCollected = true;
}

void _pickup::pickupInit(Type pickupType)
{
    this->pickupType = pickupType;  
    switch(pickupType) {
        case HEALTH:
            spriteInit("images/health.png", 1, 1);          
            break;
        case SPEED:
            spriteInit("images/...", 1, 1);
            break;
        default:
            spriteInit("images/...", 1, 1); // Error case
            break;
    }
}

void _pickup::drawSprite() {
    if (!isCollected) {
        rot.y += 1.0f; 
        _sprite::drawSprite();
    }
}
