#ifndef _SOUNDS_H
#define _SOUNDS_H

#include<_common.h>
#include<SNDS/irrKlang.h>

using namespace irrklang;

class _sounds
{
    public:
        _sounds();
        virtual ~_sounds();

        ISoundEngine *eng = createIrrKlangDevice();

        void playMusic(char *);
        void playSounds(char *);
        void pauseSound(char *);
        void initSounds();

    protected:

    private:
};

#endif // _SOUNDS_H
