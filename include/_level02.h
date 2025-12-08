#ifndef _LEVEL02_H
#define _LEVEL02_H
#include <_common.h>
#include <_skybox.h>
#include <_textureloader.h>
#include<_hallway.h>
#include<_inputs.h>
#include<_camera.h>
#include<_3dmodelloader.h>
#include<_model.h>
#include<_player.h>
#include<_trajectories.h>
#include<_enemy.h>
#include<_arenaRoom.h>
#include<_sprite.h>
#include<_pickup.h>
#include<_collisioncheck.h>
#include<_timerPlusPlus.h>
#include<_boundingBox.h>
#include<windowsx.h>    
#include<_worldObjects.h>

/* Stages for Level02 -- INIT is the loading stage (models/textures/etc) -- needs a loading screen!
* LEVEL02_INIT = Loading sector. Physics is disabled, controls disabled, etc.
* LEVEL02_PLAYING_0 = Player must shoot down 5 static targets placed around the arena
* LEVEL02_PLAYING_1 = Player must shoot down 5 moving targets that move left and right across the arena
* ...
* LEVEL02_COMPLETE = Level complete, transition to next level
*/
enum Level02Stage { LEVEL02_INIT, LEVEL02_PLAYING_0, LEVEL02_PLAYING_1, LEVEL02_COMPLETE };

class _level02 : public ILevel {
public:
    _level02();
    explicit _level02(const std::string& levelPath);
    ~_level02() override;
    size_t currentHallIndex = 0;

    Level02Stage currentStage = LEVEL02_INIT;

    void handleKey(UINT uMsg, WPARAM wParam);
    int winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam);
    const char* id() const override { return "level02"; }
    void loadAssets() override;
    void unloadAssets() override;
    void reset() override;
    void update(double dt) override;
    void render(const RenderFlags& flags) override;
    void applyCamera() override;
    // Prints Debugging Information to Console (Updates every second)
    void debugPrint();  

    // The director controls the flow of the level, such as spawning targets/enemies, spawning pickups, etc. Function called only once to make changes with stage enum changed prior. If force update is true, then we tell it change the level stage without checks
    void updateDirector(bool forceUpdate = false);

    bool loadFromTextFile(const std::string& path);
    void setNextLevelId(const std::string& id) { nextLevelId_ = id; }
    void setRequestNextLevel(std::function<void(const std::string&) > cb) { requestNextLevel_ = std::move(cb); }
    
    void createBall();
    // creates a target at a given position with a given speed
    void createTarget(vec3f position, float speed, int direction = 0); // direction: 1 = right, -1 = left, 0 = static
private:
    _camera* characterCamera = nullptr;
    _camera* noclipCamera = nullptr;
    // MAP MODELS //
    _model* bleachers = nullptr;
    _model* front_back_walls = nullptr;
    _model* left_wall = nullptr;
    _model* right_wall = nullptr;           
    _model* outer_court = nullptr;          // Outer sector, decorative only
    _model* main_court = nullptr;           // Main stage, only area player can walk.
    // target
    _targets* targetPrototype = nullptr;
    vector<_targets*> targets;

    Player* player = nullptr;
    _boundingBox* playerHitBox = nullptr;
    _timerPlusPlus* debugTimer = nullptr;
    // Used for handling physics updates on player
    _timerPlusPlus* physicsTimer = nullptr;
    _timerPlusPlus* keyTimer = nullptr;
    _timerPlusPlus* ballTimer = nullptr;
    _timerPlusPlus* ballDeleteTimer = nullptr; // Timer for deleting ballz
    _timerPlusPlus* directorTimer = nullptr;
    _timerPlusPlus* ballThrowTimer = nullptr;
    // balls!
    //vector<_model*> balls;
    //_model* ballPrototype = nullptr;
    _balls* ballPrototype = nullptr;
    vector<_balls*> balls;

    // PLAYER INFO //
    vec3f playerAccel = {0.0f, 0.0f, 0.0f};
    vec3f playerVelocity = {0.0f, 0.0f, 0.0f};

    _textureLoader* textureLoader = nullptr;
    int modelIdList = 0;
    int ballDeleteTime = 2500; // milliseconds before a ball is deleted
    // idk what any of this is
    std::string nextLevelId_;
    std::string levelPath_;
    std::function<void(const std::string&)> requestNextLevel_;
    bool levelComplete = false;
 
    // DEBUG INFO //
    int debugPrintInterval = 500; // milliseconds
    bool showBoundingBoxes = true; // Toggle bounding box display for debugging
    bool showMapModels = true;     // Toggle map models display for debugging (good for checking bounding boxes)
    bool noclipEnabled = false;    // Toggle noclip mode for debugging
    void debugPrintCollisionInfo(const _boundingBox::collisionType& collision);

    int lastMouseX = 0;
    int lastMouseY = 0;
    int windowWidth = 0;
    int windowHeight = 0;
    bool firstMouse = true;
    float ballPower = 0.0f;
};

#endif // _LEVEL02_H
