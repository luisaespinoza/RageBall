#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <_common.h>
#include <_model.h>
#include <_timerPlusPlus.h>

// Animation Data Container, used for holding the array of models used for drawing frames.
class _animationData {
    // We use a separate class to help with memory, we want each instance of _animation to have its own frames, current frame, FPS, etc but share the data so we dont need t copy it over and over
    public:
        _animationData();
        ~_animationData();
        vector<_model*> frames;       
};

class _animation {
    public:
        _animation();
        _animation(const _animation& other); // copy constructor
        virtual ~_animation();

        // Sets up an animation with the specified parameters
        // -- PARAMETERS --
        // textPath: Path to the texture file
        // modelPath: Path to the model files, should include a printf-style %04d for frame numbering
        // frames: Total number of frames in the animation
        // fps: Frames per second for playback
        // frameOffset: Starting index for frame numbering in the model files
        // scale: Scale to apply to each frame model
        // THIS CAN NEVER BE CALLED ON A COPY, ONLY THE ORIGINAL INSTANCE !!!
        void initAnimation(char* textPath, char* modelPath, int frames, int fps, int frameOffset, vec3f scale);
        // Renders the current frame of the animation at the specified position and rotation
        void drawAnimation(vec3f position, vec3f rotation);
        // Resets animation timer and defaults to first frame
        void resetAnimation();
        // Number of total frames in the animation
        int frames;
        // Current frame being displayed
        int currentFrame = 0;
        // Frames per second for the animation playback
        int FPS;
        // We track who the master copy is for resource management -- this is done higher up 
        bool ownsResources = false;
        // whether the animation should loop or stop at the end -- if disabled requires manual reset
        bool loopAnimation = true; 
        bool animationComplete = false; // set to true when animation reaches the end (non-looping only)
        
    protected:
    private:
        _timerPlusPlus* animationTimer = nullptr;
        //vector<_model*> animationFrames;
        _animationData* animationData = nullptr;

    
};

#endif // _ANIMATION_H