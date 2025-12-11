#ifndef _LEVEL02_H
#define _LEVEL02_H

#include <_common.h>
#include <_scene.h>
#include <_player.h>
#include <_model.h>
#include <_boundingBox.h>
#include <_textureloader.h>
#include <_timerPlusPlus.h>
#include <_arenaRoom.h>
#include<_sounds.h>

using namespace std;

class _level02 : public ILevel
{
    public:
        //Set Up for level 3 using the scene managers stack method
        _level02();
        explicit _level02(const string& levelPath);
        ~_level02() override;

        using RequestGameOverFun = std::function<void()>;
        size_t currentHallIndex = 0;

        const char* id() const override { return "level02"; }

        //Basic game applications override between levels
        void loadAssets() override;
        void unloadAssets() override;
        void reset() override;
        void update(double dt) override;
        void render(const RenderFlags& flags) override;
        void applyCamera() override;

        //Handles User controls
        void handleKey(UINT uMsg, WPARAM wParam);

        //Sets the next level
        void setNextLevelId(const string& id) { nextLevelId_ = id; }
        void setRequestNextLevel(function<void(const string&) > cb) { requestNextLevel_ = std::move(cb); }
        void setRequestGameOver(RequestGameOverFun fn) { requestGameOver_ = std::move(fn); }
        void createBall();

    private:
        //State concepts
        unique_ptr<Player> player;
        unique_ptr<_camera> characterCam;
        _textureLoader* textureLoader = nullptr;
        _boundingBox* playerHitBox = nullptr;
        _timerPlusPlus* ballDeleteTimer = nullptr;
        _timerPlusPlus* stunnedTimer = nullptr;
        _timerPlusPlus* enemyMoveTimer = nullptr;
        _timerPlusPlus* ballTimer = nullptr;
        _timerPlusPlus* ballThrowTimer = nullptr;
        float ballPower = 0.0f;


        RequestGameOverFun requestGameOver_;

        //Dodge ball mechanics
        _model ballPrototype;
        struct Ball
        {
            _model* model;
            _boundingBox hitBox;
            vec3f position;
            vec3f velocity;
            double spawnTime; // <- gets the time of item spawn
            bool active;
            bool fromEnemy;
        };
        vector<Ball> balls;
        double lastThrownTime = 0.3f;
        double throwCoolDown = 1.0f;
        double currentBallTime;

        //CUBE Blockage
        _model cubeModel;
        _boundingBox cubeBox;

        vec3f cubeStartPosition;
        vec3f cubeEndPosition;
        vec3f cubeCurrentPosition;
        vec3f cubeVelocity;

        //Determine the cubes current state
        enum class Cubestate {Waiting, Moving, Locked};
        Cubestate currentState;
        double cubeDelay = 2.0; // <- waits then follows the player
        double cubeTimer = 0.0;

        //ENEMY AI from level 01
        _model EnemyPrototype;
        struct Enemy
        {
            _model* model;
            _boundingBox enemyBox;
            vec3f position;
            vec3f velocity;
            vec3f faceForward;
            float yaw;
            float radius = 0.5f;
            int enemyLife;
            bool stunned;
            double stunTimer;
            bool alive;
            double nextDirChange;
            double nextShootTime;
        };
        vector<Enemy> enemies;

        //Arena room for enemies and player
        _arenaRoom arena;
        vec3f arenaCenter;
        float arenaDoubleSizeX = 10.0f; //Doubles the size of the arena
        float arenaDoubleSizeZ = 10.0f;
        _boundingBox arenaHitBox;
        _boundingBox arenaMidlineHitBox;

        //LEVEL changing system
        string nextLevelId_;
        function<void(const string&)> requestNextLevel_;

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

        //HELPERS
        void spawnInitEnemies();
        void createEnemyBall(const Enemy&);
        void updateEnemies(double dt);
        void updateBalls(double dt);
        void updateCubeBlockage(double dt);
        void updatePlayer(double dt);
        void ballHitsEnemies();
        void playerHitCube();
        void ballHitsCube();
        void enemyBallHitPlayer();
        void playerHitEnemy();
};

#endif // _LEVEL02_H
