#ifndef _LEVEL00_H
#define _LEVEL00_H

#include <_common.h>

#include <_textureloader.h>
#include<_camera.h>
#include<_model.h>
#include<_player.h>
#include<_sprite.h>
#include<_pickup.h>
#include<_collisioncheck.h>
#include<_timerPlusPlus.h>  // variation of _timer.h class -- uses Chrono instead of time.h
#include<_boundingBox.h>
#include<windowsx.h>    
#include<_worldObjects.h>
#include<_sounds.h>

/* Stages for Level00 -- INIT is the loading stage (models/textures/etc) -- needs a loading screen!
* LEVEL00_INIT = Loading sector. Physics is disabled, controls disabled, etc.
* LEVEL00_PLAYING_0 = Player must shoot down 5 static targets placed around the arena
* LEVEL00_PLAYING_1 = Player must shoot down 5 moving targets that move left and right across the arena
* LEVEL00_PLAYING_2 = Player must shoot down 5 moving throwers that throw balls at the player (not aimed at player, just straight)
* LEVEL00_COMPLETE = Level complete, transition to next level
*/

/* Cotrols:
* Toggle Bounding Box Display: "["
* Toggle Map Models Display:   "]"
* Toggle Noclip Mode:          "\"
* Spawn Thrower (for testing): "/" DIVIDE key on numpad
* WASD + Space -- default movement/jump
* Hold LMB for ball throw -- the longer held the more powerful the shot. Maxes about after ~2 seconds
*/
enum Level00Stage { LEVEL00_INIT, LEVEL00_PLAYING_0, LEVEL00_PLAYING_1, LEVEL00_PLAYING_2, LEVEL00_COMPLETE };

class _level00 : public ILevel {
public:
    _level00();
    explicit _level00(const std::string& levelPath);
    virtual ~_level00() override;

    Level00Stage currentStage = LEVEL00_INIT;
    // Input handler -- used for keys that don't require being held down
    void handleKey(UINT uMsg, WPARAM wParam);
    // Mouse handler
    int winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam);
    const char* id() const override { return "level00"; }
    // Level Init
    void loadAssets() override;
    // Level Destructor
    void unloadAssets() override;
    // Level Reset on player death
    void reset() override;
    // Level Update Loop -- Used for game logic, not rendering/drawing
    void update(double dt) override;
    // Level Render Loop -- Used for rendering/drawing, not game logic
    void render(const RenderFlags& flags) override;
    // Prints Debugging Information to Console (Updates every second)
    void debugPrint();  
    // The director controls the flow of the level, such as spawning targets/enemies, spawning pickups, etc. Function called only once to make changes with stage enum changed prior. If force update is true, then we tell it change the level stage without checks
    void updateDirector(bool forceUpdate = false);
    // UNUSED -- REQUIRED BY LEVEL INTERFACE //
    void applyCamera() override;
    bool loadFromTextFile(const std::string& path);
    void setNextLevelId(const std::string& id) { nextLevelId_ = id; }
    void setRequestNextLevel(std::function<void(const std::string&) > cb) { requestNextLevel_ = std::move(cb); }
    // ------------------------------------- //
    // Creates a ball -- change type for friendly/enemy ball
    void createBall(BallType type, vec3f position, float velocity); 
    // Creates a target -- only faces forward + only enemy no friendly targets
    // direction: 1 = right, -1 = left, 0 = static
    void createTarget(vec3f position, float speed, int direction = 0); 
    // Create a thrower -- VERY simple AI, moves left/right and throws balls at intervals forward (no aiming)
    // direction: 1 = right, -1 = left, 0 = static
    void createThrower(vec3f position, float speed, int direction = 0); 
private:
    // CAMERAS //
    _camera* characterCamera = nullptr;
    _camera* noclipCamera = nullptr;
    // MAP MODELS //
    _model* bleachers = nullptr;
    _model* front_back_walls = nullptr;
    _model* left_wall = nullptr;
    _model* right_wall = nullptr;           
    _model* outer_court = nullptr;          // Outer sector, decorative only
    _model* main_court = nullptr;           // Main stage, only area player can walk.
    // TARGETS //
    _targets* targetPrototype = nullptr;
    vector<_targets*> targets;
    // THROWERS //
    _thrower* throwerPrototype = nullptr;
    vector<_thrower*> throwers;
    // PLAYER //
    Player* player = nullptr;
    _boundingBox* playerHitBox = nullptr;
    // TIMERS //
    _timerPlusPlus* physicsTimer = nullptr;
    _timerPlusPlus* keyTimer = nullptr;
    _timerPlusPlus* ballTimer = nullptr;
    _timerPlusPlus* ballDeleteTimer = nullptr; // Timer for deleting ballz
    _timerPlusPlus* directorTimer = nullptr;
    _timerPlusPlus* ballThrowTimer = nullptr;
    _timerPlusPlus* debugTimer = nullptr;
    _timerPlusPlus* playerDeathTimer = nullptr;
    // BALLS!!//
    _balls* ballPrototype = nullptr;
    vector<_balls*> balls;
    // PLAYER INFO //
    vec3f playerAccel = {0.0f, 0.0f, 0.0f};
    vec3f playerVelocity = {0.0f, 0.0f, 0.0f};

    _textureLoader* textureLoader = nullptr; // unused

    int ballDeleteTime = 2500; // milliseconds before a ball is deleted

    // idk what any of this is
    std::string nextLevelId_;
    std::string levelPath_;
    std::function<void(const std::string&)> requestNextLevel_;
    bool levelComplete = false;
 
    // DEBUG INFO //
    int debugPrintInterval = 2000; // milliseconds
    bool enableDebugging = false;   // allows for debugging controls + options
    bool showBoundingBoxes = false; // Toggle bounding box display for debugging
    bool showMapModels = true;     // Toggle map models display for debugging (good for checking bounding boxes)
    bool noclipEnabled = false;    // Toggle noclip mode for debugging
    // Debug print collision info
    void debugPrintCollisionInfo(const _boundingBox::collisionType& collision);
    // Mouse stuff
    int lastMouseX = 0;
    int lastMouseY = 0;
    int windowWidth = 0;
    int windowHeight = 0;
    bool firstMouse = true;
    float ballPower = 0.0f;

    int playerHealth = 3; // Player health -- could be inside Player class, but trivial
    int playerLastHitid = -1; // to prevent multiple hits from same ball (player only)
    // SOUNDS //
    int targetsHit = 0; // changes sound pitch for cool effect
    _sounds* soundEngine = nullptr;
    // sound - settings wa
    float walkVolume = 0.25f;
    float ballBounceVolume = 0.3f;
    float targetHitVolume = 1.0f;
    float playerHitVolume = 1.0f;
    float playerDeathVolume = 1.0f;
    float throwerHitVolume = 1.0f;
    float throwerDieVolume = 1.0f;
    float musicVolume = 0.15f;
    float transitionLevelVolume = 0.5f;
};

#endif // _LEVEL00_H
