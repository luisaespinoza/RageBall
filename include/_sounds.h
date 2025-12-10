#ifndef _SOUNDS_H
#define _SOUNDS_H

#include<_common.h>
#include<SNDS/irrKlang.h>

using namespace irrklang;

// Sound management class for irrKlang library -- handles music and sound effects
class _sounds
{
    public:
        _sounds();
        virtual ~_sounds();

        /*
        Main sound engine for irrKlang library.
        Why static? There is only need for one engine, adding more can cause segfaults and other issues.
        */
        static ISoundEngine *eng;

        // Play music -- this loops by default with no overlap
        void playMusic(char* filename);
        /*
        Plays a given sound effect with fun options!
        -> allowOverlap :: True/False -- Allows multiple instances of the same sound to play over each other
        -> volume       :: 0.0f to 1.0f -- Volume control for sound effect
        -> pitch        :: 0.5f to infinity -- Pitch control for sound effect
        */
        void playSounds(char* filename, bool allowOverlap = false, float volume = 1.0f, float pitch = 1.0f);
        /*
        Plays a given sound effect with 3D spacial positioning and fun options!
        You should be setting updateListener() during every update loop for proper 3D sound rendering.
        -> position     :: vec3f -- 3D position of the sound in world space
        -> allowOverlap :: True/False -- Allows multiple instances of the same sound to play over each other
        -> volume       :: 0.0f to 1.0f -- Volume control for sound effect
        -> pitch        :: 0.5f to infinity -- Pitch control for sound effect
        */
        void playSounds3D(char* filename, vec3f position, bool allowOverlap = false, float volume = 1.0f, float pitch = 1.0f);
        void pauseSound(char* filename);
        // Adjusts position for 3D sound rendering -- should be called every update loop
        void updateListener(vec3f position, vec3f look, vec3f up);

        // Initialize the sound engine -- only needed once, add to loadAssets();
        static void initSoundEngine();
        // Called in WinMain at end before KillWindow() to drestroy static sound engine
        static void dropSoundEngine();

    protected:

    private:
};

#endif // _SOUNDS_H
