#include "_sounds.h"

_sounds::_sounds()
{
    //ctor
}

_sounds::~_sounds()
{
    //dtor
    eng->drop();
}

void _sounds::playMusic(char* filename)
{
    eng->play2D(filename,true);
}

void _sounds::playSounds(char* filename)
{
   if(!eng->isCurrentlyPlaying(filename))
        eng->play2D(filename,false,false);
}

void _sounds::pauseSound(char* filename)
{
    eng->play2D(filename,true,false);
}

void _sounds::initSounds()
{
   if(!eng)
    cout<< "ERROR: **** The sound could not Load\n";
}
