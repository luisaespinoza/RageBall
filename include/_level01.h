#ifndef _LEVEL01_H
#define _LEVEL01_H
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


class _level01 : public ILevel {
public:
    _level01() : _level01(std::string{}) {}
    explicit _level01(const std::string& levelPath) : levelPath_(levelPath) {}
    ~_level01() override = default;

    size_t currentHallIndex = 0;

    void handleKey(UINT uMsg, WPARAM wParam);
    const char* id() const override { return "level01"; }

    void loadAssets() override;
    void unloadAssets() override;
    void reset() override;
    void update(double dt) override;
    void render(const RenderFlags& flags) override;
    void applyCamera() override;

    bool loadFromTextFile(const std::string& path);
    void setNextLevelId(const std::string& id) { nextLevelId_ = id; }
    void setRequestNextLevel(std::function<void(const std::string&) > cb) { requestNextLevel_ = std::move(cb); }
    // From a Scene click (world-unprojected), request the player to throw.
    void throwBallAtWorld(double wx, double wy, double wz);
    //shoot along the click ray (near→far)
    void throwBallFromRay(const vec3& rayOrigin, const vec3& rayDir);

private:
    Player*        player       = nullptr;
    _inputs*       levelInput   = nullptr;
    _textureLoader textures;
    _skyBox        sky;
    _camera*       levelCamera  = nullptr;
    std::vector<_hallway> halls;
    _arenaRoom arena_;
    bool useArena =true;
    struct ThrowerAI { float cooldown = 1.6f; float t = 0.f; };
    std::vector<ThrowerAI> enemyAI_;
    std::vector<std::pair<Pose,float>> hazardsLocal_; // local-space hazards

    bool        skyReady   = false;
    std::string levelPath_;
    std::function<void(const std::string&)> requestNextLevel_;
    std::string nextLevelId_;
    bool firedNext_ = false;
    std::vector<std::unique_ptr<Enemy>> enemies;
};

#endif // _LEVEL01_H
