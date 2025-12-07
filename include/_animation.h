#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <_common.h>
#include <_model.h>
#include <_timerPlusPlus.h>

class _animation {
    public:
        _animation();
        virtual ~_animation();

        // Sets up an animation with the specified parameters
        // -- PARAMETERS --
        // textPath: Path to the texture file
        // modelPath: Path to the model files, should include a printf-style %04d for frame numbering
        // frames: Total number of frames in the animation
        // fps: Frames per second for playback
        // frameOffset: Starting index for frame numbering in the model files
        // scale: Scale to apply to each frame model
        void initAnimation(char* textPath, char* modelPath, int frames, int fps, int frameOffset, vec3f scale);
        // Renders the current frame of the animation at the specified position and rotation
        void drawAnimation(vec3f position, vec3f rotation);
        // Resets animation timer and defaults to first frame
        void resetAnimation();
        // Number of total frames in the animation
        int frames;
        // Current frame being displayed
        int currentFrame;
        // Frames per second for the animation playback
        int FPS;
        
    protected:
    private:
        _timerPlusPlus* animationTimer = nullptr;
        vector<_model*> animationFrames;

    
};

#endif // _ANIMATION_H