#ifndef _PICKUP_H
#define _PICKUP_H

#include<_common.h>
#include<_sprite.h>
#include<_player.h>

class _pickup : public _sprite
{
    public:
        _pickup();
        virtual ~_pickup();
        bool isCollected;
        enum Type {HEALTH, SPEED }; // Add more as needed, none is error checked
        Type pickupType;
        float life, speed;
        void applyEffect(Player* player);
        void pickupInit(Type type); // Textures hard coded to corresponding texture files
        void drawSprite();
    protected:

    private:
};

#endif // _PICKUP_H