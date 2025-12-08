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


class _level02 : public ILevel {
public:
    _level02();
    explicit _level02(const std::string& levelPath);
    ~_level02() override;
    size_t currentHallIndex = 0;

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

    bool loadFromTextFile(const std::string& path);
    void setNextLevelId(const std::string& id) { nextLevelId_ = id; }
    void setRequestNextLevel(std::function<void(const std::string&) > cb) { requestNextLevel_ = std::move(cb); }
    
    void createBall();
    // creates a target at a given position with a given speed
    void createTarget(vec3f position, float speed);
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
    // balls!
    vector<_model*> balls;
    _model* ballPrototype = nullptr;

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
};

#endif // _LEVEL02_H
