#include "_sceneManager.h"
#include<windowsx.h>
_sceneManager::_sceneManager()
{
    //ctor
    // level01 -> level02
    LevelRegistry::instance().registerLevel("level01", [](){
        auto L = std::make_unique<_level01>("levels/level01.txt");
        L->setNextLevelId("level02");
        return L;
    });

    // level02 -> level03
    LevelRegistry::instance().registerLevel("level02", [](){
        auto L = std::make_unique<_level01>("levels/level02.txt");
        L->setNextLevelId("level03");
        return L;
    });

    // level03 -> back to main menu
    LevelRegistry::instance().registerLevel("level03", [](){
        auto L = std::make_unique<_level01>("levels/level03.txt");
        L->setNextLevelId("");
        return L;
    });

//     // Boot into main menu with a default level id so first Start works
// this->setCurrentScene(std::make_unique<_menuScene>(
//     /* onStart */ [this](const std::string& lvl){ this->setCurrentLevel(lvl); },
//     /* onHelp  */ [this]{
//         this->pushScene(std::make_unique<HelpScene>(
//             /* onClose */ [this]{ this->popScene(); }
//         ));
//     },
//     /* onQuit  */ [this]{ PostQuitMessage(0); },
//     "level01"
// ));/
}

_sceneManager::~_sceneManager()
{
    //dtor
}
void _sceneManager::setCurrentScene(std::unique_ptr<_SceneInterface> newScene)
{
   // std::cout << "[mgr] setCurrentScene(type=" << typeid(*newScene).name()
             // << ", suppressEnterNext_=" << (suppressEnterNext_?"T":"F") << ")\n";
    clearAllScenes();
    sceneStack_.emplace_back(std::move(newScene));
    if (!suppressEnterNext_) {
     //   std::cout << "[mgr] -> calling onEnter() of " << typeid(*sceneStack_.back()).name() << "\n";
        sceneStack_.back()->onEnter();
    } else {
       // std::cout << "[mgr] -> SKIP onEnter()\n";
        suppressEnterNext_ = false;
    }
}
void _sceneManager::setCurrentSceneNoEnter(std::unique_ptr<_SceneInterface>newScene)
{
    suppressEnterNext_ = true;
    setCurrentScene(std::move(newScene));   // will skip onEnter() once
}

void _sceneManager::pushScene(std::unique_ptr<_SceneInterface> newScene)
{
    if (!newScene) return;
    newScene->onEnter();
    sceneStack_.emplace_back(std::move(newScene));
}

void _sceneManager::popScene()
{
    if (sceneStack_.empty()) return;
    sceneStack_.back()->onExit();
    sceneStack_.pop_back();
}

void _sceneManager::handleResizeEvent(int windowWidth, int windowHeight)
{
//    for (auto& s : sceneStack_) s->onResize(windowWidth, windowHeight);
    applyPerspective(windowWidth, windowHeight);           // NEW
    for (auto& s : sceneStack_) s->onResize(windowWidth, windowHeight);
}
void _sceneManager::updateActiveScene(double dt)
{

    static bool bootOnce=false;
    if (!bootOnce) { //std::cout << "[mgr] updateActiveScene: POLLING BUILD\n"; bootOnce=true;

    }

    // Use the hardware transition bit to avoid missed edges
    const SHORT s = GetAsyncKeyState(VK_ESCAPE);
    const bool escPressedEdge = (s & 1) != 0;   // low bit set => key transitioned since last call

    if (escPressedEdge) {
        // if (isTopInGameMenu()) {
        //   //  std::cout << "[mgr] ESC -> resumeFromInGameMenu()\n";
        //     resumeFromInGameMenu();                 // no onEnter()
        // } else {
        //    // std::cout << "[mgr] ESC -> showInGameMenu()\n";
        //     showInGameMenu();                       // switch to _menuScene (IGM)
        // }
        if (!sceneStack_.empty()) {
            if (!isTopInGameMenu())
            {
                showInGameMenu();                       // switch to _menuScene (IGM)
            }
        }
    }

    if (sceneStack_.empty()) return;
    sceneStack_.back()->update(dt);                // update only the top scene
    if (!sceneStack_.empty()) sceneStack_.back()->update(dt);

    // ---- SAFE POINT: run any posted stack mutations after updates ----
    if (!pendingOps_.empty()) {
        auto ops = std::move(pendingOps_);
        pendingOps_.clear();
        for (auto& f : ops) f();
    }
}

void _sceneManager::renderActiveScene()
{
    // render bottom → top so overlays (pause, menus) draw over gameplay
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
   // printf("[mgr] stack=%zu (bottom->top)\n", sceneStack_.size());
    for (auto& s : sceneStack_) s->render();
}


int _sceneManager::forwardWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (sceneStack_.empty()) return 0;
       // --- DEBUG: log key messages briefly ---
//    if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN || message == WM_KEYUP || message == WM_SYSKEYUP) {
//        if (wParam == VK_ESCAPE) {
//            printf("[mgr] ESC msg=%u (%s)\n", message,
//                   (message==WM_KEYDOWN||message==WM_SYSKEYDOWN) ? "down" : "up");
//        }
//    }
// const bool escDown = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) && wParam == VK_ESCAPE;
//    const bool escUp   = (message == WM_KEYUP   || message == WM_SYSKEYUP)   && wParam == VK_ESCAPE;
//
//    if (escDown || escUp) {
//        if (escDown) {
//            const bool wasDown = (lParam & (1 << 30)) != 0;   // bit 30 = previous key state
//            if (wasDown) {
//                // ignore repeats; let the first down or the key-up handle it
//                return 0;
//            }
//        }
//
//        // If top is the in-game menu, resume; otherwise show it.
//        if (!sceneStack_.empty()) {
//            if (auto ms = dynamic_cast<_menuScene*>(sceneStack_.back().get())) {
//                // If you don't have isInGameMenu(), directly check the mode value (see note below)
//                if (ms->isInGameMenu()) {        // accessor lives in your menu scene
//                    resumeFromInGameMenu();
//                    return 0;
//                }
//            }
//        }
//        showInGameMenu();
//        return 0;
//    }
    MSG m{};
    m.hwnd = windowHandle;
    m.message = message;
    m.wParam = wParam;
    m.lParam = lParam;
    sceneStack_.back()->handleEvent(m);
    // Some scenes still prefer direct Win32:
        //std::cout << "[SM] fwd msg=" << std::hex << message << " wParam=" << std::dec << wParam << "\n";

    return sceneStack_.back()->winMsg(windowHandle, message, wParam, lParam);
}

void _sceneManager::clearAllScenes()
{
    while (!sceneStack_.empty()) popScene();
}
void _sceneManager::setCurrentLevel(const std::string& levelId)
{
    auto lvl = LevelRegistry::instance().create(levelId);
    auto loader = [this](const std::string& nextId){
        this->post([this, nextId]{ this->setCurrentLevel(nextId); });
    };
    setCurrentScene(std::make_unique<LoadLevelScene>(std::move(lvl), loader));
}

LevelRegistry& LevelRegistry::instance() {
  static LevelRegistry R; return R;
}
void LevelRegistry::registerLevel(const std::string& id, LevelFactory f) {
  factories[id] = std::move(f);
}
std::unique_ptr<ILevel> LevelRegistry::create(const std::string& id) {
  auto it = factories.find(id);
  if (it == factories.end()) return nullptr;
  return it->second();
}
LoadLevelScene::LoadLevelScene(std::unique_ptr<ILevel> lvl)
    : LoadLevelScene(std::move(lvl), nullptr) {}   // delegate

LoadLevelScene::LoadLevelScene(std::unique_ptr<ILevel> lvl,
                               std::function<void(const std::string&)> fn)
    : level(std::move(lvl)), loadLevelFn(std::move(fn)) {}
void LoadLevelScene::onEnter(){
    std::cout << "[load] onEnter: loadAssets begin\n";

    if (auto* l01 = dynamic_cast<_level01*>(level.get())) {
        if (loadLevelFn) l01->setRequestNextLevel(loadLevelFn);
    }

    level->loadAssets();
    level->reset();
}

void LoadLevelScene::onExit() {
    if (level) {
        level->unloadAssets();
    }
}

void LoadLevelScene::update(double dt) {
    if (level)
        level->update(dt);
}

void LoadLevelScene::render() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    RenderFlags flags;
    if (level) {
        level->applyCamera();
        level->render(flags);
    }
}
int LoadLevelScene::winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            const bool wasDown = (lParam & (1 << 30)) != 0;
            if (!wasDown && level) level->handleKey(uMsg, wParam);
            return 0;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            if (level) level->handleKey(uMsg, wParam);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (!level) return 0;

            // ----- screen -> world ray -----
            GLdouble proj[16], model[16];
            GLint    vp[4];
            glGetDoublev(GL_PROJECTION_MATRIX, proj);
            glGetDoublev(GL_MODELVIEW_MATRIX,  model);
            glGetIntegerv(GL_VIEWPORT,         vp);

            const int mx = GET_X_LPARAM(lParam);
            const int my = GET_Y_LPARAM(lParam);
            const double sx = static_cast<double>(mx);
            const double sy = static_cast<double>(vp[3] - my - 1); // invert Y for OpenGL

            GLdouble nx, ny, nz;  // near
            GLdouble fx, fy, fz;  // far
            gluUnProject(sx, sy, 0.0, model, proj, vp, &nx, &ny, &nz);
            gluUnProject(sx, sy, 1.0, model, proj, vp, &fx, &fy, &fz);

            // Ray origin = near point, direction = far - near
            const float rdx = static_cast<float>(fx - nx);
            const float rdy = static_cast<float>(fy - ny);
            const float rdz = static_cast<float>(fz - nz);

            if (auto* l01 = dynamic_cast<_level01*>(level.get())) {
                l01->throwBallFromRay(
                    { static_cast<float>(nx), static_cast<float>(ny), static_cast<float>(nz) },
                    { static_cast<float>(fx - nx), static_cast<float>(fy - ny), static_cast<float>(fz - nz) }
                );
            }
        }
        default:
            return 0;
    }
}
void _sceneManager::initlGL()
{
    //sanity logic. ensure this function executes once on accidental double calls
    static bool done = false;
    if(done) return;
    done =true;

    glShadeModel(GL_SMOOTH); // to handle GPU shaders
    glClearColor(0.0f,0.0f,0.0f,0.0f); // black background color
    glClearDepth(1.0f);         //depth test for layers
    glEnable(GL_DEPTH_TEST);    //activate depth test
    glDepthFunc(GL_LEQUAL);     // depth function type

    glEnable(GL_TEXTURE_2D);

    globalLight.setLight(GL_LIGHT0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//
    //menuBackground->skyBoxInit();
    //menuBackground->tex[0] = menuBackground->textures->loadTexture("images/front.jpg");//front/posz
    //menuBackground->tex[1] = menuBackground->textures->loadTexture("images/back.jpg"); //back/negz
    //menuBackground->tex[2] = menuBackground->textures->loadTexture("images/top.jpg");  //top/posy
    //menuBackground->tex[3] = menuBackground->textures->loadTexture("images/bottom.jpg"); //bottom/negy
    //menuBackground->tex[4] = menuBackground->textures->loadTexture("images/right.jpg");  //right/posx

}
void _sceneManager::applyPerspective(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<float>(w)/static_cast<float>(h), 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}
void _sceneManager::showInGameMenu()
{
    std::cout << "[mgr] showInGameMenu()\n";

    // Preserve gameplay exactly once
    if (!preservedScene_ && !sceneStack_.empty()) {
        preservedScene_ = std::move(sceneStack_.back());
        sceneStack_.pop_back();
    }

auto menu = std::make_unique<_menuScene>(
    /* onResume   */ [this]{
        this->post([this]{ this->resumeFromInGameMenu(); });
    },
    /* onMainMenu */ [this]{
        this->post([this]{
            preservedScene_.reset();
            clearAllScenes();
            this->setCurrentScene(std::make_unique<_menuScene>(
                /* onStart */ [this](const std::string& lvl){ this->post([this,lvl]{ this->setCurrentLevel(lvl); }); },
                // /* onHelp  */ [this]{
                //     this->pushScene(std::make_unique<HelpScene>(
                //         /* onClose */ [this]{ this->popScene(); }
                //     ));
                /* onHelp     */ [this]{
                    this->showHelpOverlay(); // calls pushScene(<_help>)
                },

                /* onQuit  */ [this]{ PostQuitMessage(0); },
                "level01"
            ));
        });
    },
    /* onHelp     */ [this]{
        this->pushScene(std::make_unique<HelpScene>(
            /* onClose */ [this]{ this->popScene(); }
        ));
    },
    /* onQuit     */ [this]{ this->post([this]{ PostQuitMessage(0); }); }
);
menu->setMode(_menuScene::Mode::InGameMenu);
setCurrentScene(std::move(menu));
}

void _sceneManager::resumeFromInGameMenu()
{
    if (!preservedScene_) return;
    clearAllScenes();  // remove the menu
    setCurrentSceneNoEnter(std::move(preservedScene_));  // restore gameplay, no onEnter()
}
bool _sceneManager::isTopInGameMenu() const
{
    if (sceneStack_.empty()) return false;
    if (auto* ms = dynamic_cast<_menuScene*>(sceneStack_.back().get())) {
        return ms->isInGameMenu();
    }
    return false;
}

//
//void MenuScene::render()
//{
//
//}
//
//int MenuScene::winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//  if (uMsg == WM_LBUTTONDOWN && onStartLevel) {
//    onStartLevel("level01");
//    return 0;
//  }
//  return 0;
//}


void _sceneManager::bootMainMenu(const string& firstLevelId)
{
    auto start = [this](const std::string& levelId) {
        setCurrentLevel(levelId);
    };
    auto helpMenu = [this](){
        showHelpOverlay();
    };
    auto quit  = [&](){
        PostQuitMessage(0);
    };
    setCurrentScene(std::make_unique<_menuScene>(
         start,
         helpMenu,
         quit,
         firstLevelId
         ));
}

void _sceneManager::showHelpOverlay()
{
    pushScene(std::make_unique<HelpScene>(
        /* onClose */ [this]{ this->popScene(); }
    ));
}