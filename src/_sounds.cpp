#include "_sounds.h"

ISoundEngine* _sounds::eng = nullptr;

_sounds::_sounds()
{
    //ctor
}

_sounds::~_sounds()
{
    //dtor
    //eng->drop();
}

void _sounds::initSoundEngine()
{
    if (!eng) {
        eng = createIrrKlangDevice();
        if (!eng) {
            cerr << "ERROR: **** The sound device could not be created.\n";
        }
    }
}

void _sounds::dropSoundEngine()
{
    if (eng) {
        eng->drop(); 
        eng = nullptr;
    }
}

void _sounds::playMusic(char* filename)
{
    if (eng) { 
        eng->play2D(filename,true); 
    }
}

void _sounds::playSounds(char* filename, bool allowOverlap, float volume, float pitch)
{

    if(eng && (!eng->isCurrentlyPlaying(filename) || allowOverlap)) {
        ISound* sound = eng->play2D(filename,false,false,true);
        if (sound) {
            sound->setVolume(volume);
            sound->setPlaybackSpeed(pitch);
            sound->drop(); 
        }
    }
}

void _sounds::playSounds3D(char* filename, vec3f position, bool allowOverlap, float volume, float pitch)
{
    if(eng && (!eng->isCurrentlyPlaying(filename) || allowOverlap)) {
        ISound* sound = eng->play3D(filename,irrklang::vec3df(position.x,position.y,position.z),false,false,true);
        if (sound) {
            sound->setVolume(volume);
            sound->setPlaybackSpeed(pitch);
            sound->setMinDistance(5.0f);
            sound->drop(); 
        }
    }
}

void _sounds::pauseSound(char* filename)
{
    if (eng) {
        eng->play2D(filename,true,false);
    }
}

void _sounds::updateListener(vec3f position, vec3f look, vec3f up)
{
    if (eng) {
        eng->setListenerPosition(
            irrklang::vec3df(position.x, position.y, position.z),
            irrklang::vec3df(look.x, look.y, look.z),
            irrklang::vec3df(0,0,0),
            irrklang::vec3df(up.x, up.y, up.z)
        );
    }
}
