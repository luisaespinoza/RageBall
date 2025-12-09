#include "_menuScene.h"
//
//_menuScene::_menuScene()
//{
//    //ctor
//}
//
//_menuScene::~_menuScene()
//{
//    //dtor
//}
namespace {
    constexpr float kMainMenuCubeEdge  = 2.0f;  // matches menuRender()
    constexpr float kPauseMenuCubeEdge = 1.6f;  // matches pause mode in menuRender()
    constexpr float kHitPadding        = 0.5f;  // inflate hit volume beyond cube edge
}

static bool gSkyLoaded = false;
static void ensureMenuSkyboxLoaded(_skyBox* sb) {
    if (!sb || gSkyLoaded) return;
    sb->skyBoxInit();
    sb->tex[0] = sb->textures->loadTexture("images/front.jpg");   // posZ
    sb->tex[1] = sb->textures->loadTexture("images/back.jpg");    // negZ
    sb->tex[2] = sb->textures->loadTexture("images/top.jpg");     // posY
    sb->tex[3] = sb->textures->loadTexture("images/bottom.jpg");  // negY
    sb->tex[4] = sb->textures->loadTexture("images/right.jpg");   // posX
    sb->tex[5] = sb->textures->loadTexture("images/left.jpg");    // negX
    gSkyLoaded = true;
}
// -------- constructors --------
_menuScene::_menuScene(StartCallback onStart, HelpCallback onHelpCb, QuitCallback onQuitCb, const std::string& levelId)
    : mode(Mode::MainMenu),
      defaultLevelId(levelId),
      onStartLevel(std::move(onStart)),
      onHelp(std::move(onHelpCb)),
      onQuit(std::move(onQuitCb)) {}

_menuScene::_menuScene(ResumeCallback onResumeCb, MainMenuCallback onMainCb, HelpCallback onHelpCb, QuitCallback onQuitCb)
    : mode(Mode::InGameMenu),
      onResume(std::move(onResumeCb)),
      onGoMainMenu(std::move(onMainCb)),
      onHelp(std::move(onHelpCb)),
      onQuit(std::move(onQuitCb)) {}


// -------- lifecycle --------
void _menuScene::onEnter() {
    char* menuModelFile = "models/megaman/tris.md2";
    menuModel->initModel(menuModelFile);

    menuBullet.speed=pauseBullet.speed = 36.0f;
    menuBullet.radius = 1.0f;

  //      std::cout << "[menu] onEnter mode=" << (mode==Mode::InGameMenu?"IGM":"MAIN") << "\n";
    menuBackground->skyBoxInit();
    menuBackground->tex[0] = menuBackground->textures->loadTexture("images/front.jpg");//front/posz


    menuBackground->tex[1] = menuBackground->textures->loadTexture("images/back.jpg"); //back/negz
    menuBackground->tex[2] = menuBackground->textures->loadTexture("images/top.jpg");  //top/posy
    menuBackground->tex[3] = menuBackground->textures->loadTexture("images/bottom.jpg"); //bottom/negy
    menuBackground->tex[4] = menuBackground->textures->loadTexture("images/right.jpg");  //right/posx
    menuBackground->tex[5] = menuBackground->textures->loadTexture("images/left.jpg");  //left/negx

    if (mode == Mode::MainMenu) enterMainMenu();
    else                        enterPause();
}

void _menuScene::onResize(int w, int h) {
    width = (w > 0) ? w : 1;
    height= (h > 0) ? h : 1;
}

// -------- loop --------
void _menuScene::update(double dt) {
    menuUpdate(dt);
}

void _menuScene::render() {
   // std::cout << "[menu] render mode=" << (mode==Mode::InGameMenu?"IGM":"MAIN") << "\n";
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    menuRender();
}
// -------- input --------
int _menuScene::winMsg(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_LBUTTONDOWN:
            spawnBullet();
            return 0;
        case WM_MOUSEMOVE:
            mouseMapping(LOWORD(lParam), HIWORD(lParam));
            return 0;
        default: break;
    }
    return 0;
}

// -------- mouse mapping (camera must be set beforehand if you want true depth) --------
void _menuScene::mouseMapping(int x, int y) {
    GLint viewPort[4];
    GLdouble model[16], proj[16];
    GLfloat winX, winY, winZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, viewPort);

    winX = (GLfloat)x;
    winY = (GLfloat)(viewPort[3] - y); // flip Y for window coords

    glReadPixels(x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    gluUnProject(winX, winY, winZ, model, proj, viewPort, &msX, &msY, &msZ);
}

// -------- setup per mode --------
void _menuScene::enterMainMenu() {
    menuCamera.camInit();

    const float halfEdge = kMainMenuCubeEdge * 0.5f;
    const float blockRadius = halfEdge + kHitPadding; // treat boxes as “bigger”

    menuBlocks = {
        { { -3.0f, 0.0f, -20.0f }, blockRadius, MenuBlock::Type::StartGame },
        { {  0.0f, 0.0f, -20.0f }, blockRadius, MenuBlock::Type::Help      },
        { {  3.0f, 0.0f, -20.0f }, blockRadius, MenuBlock::Type::Quit      },
    };

    menuBullet.actionTrigger = menuBullet.READY;
    menuBullet.live = false;
    menuBullet.t = 0.0f;
}


void _menuScene::enterPause() {
    pauseCamera.camInit();

    const float halfEdge = kPauseMenuCubeEdge * 0.5f;
    const float blockRadius = halfEdge + kHitPadding;

    pauseBlocks = {
        { {  0.0f,  3.0f, -12.0f }, blockRadius, MenuBlock::Type::Resume   },
        { {  0.0f,  1.0f, -12.0f }, blockRadius, MenuBlock::Type::Help     },
        { {  0.0f, -1.0f, -12.0f }, blockRadius, MenuBlock::Type::MainMenu },
        { {  0.0f, -3.0f, -12.0f }, blockRadius, MenuBlock::Type::Quit     },
    };

    pauseBullet.actionTrigger = pauseBullet.READY;
    pauseBullet.live = false;
    pauseBullet.t = 0.0f;
}


_menuScene::MenuContext _menuScene::activeMenuContext() {
    switch (mode) {
        case Mode::MainMenu:
            return MenuContext{ &menuCamera, &menuBullet, &menuBlocks, &menuCollision };
        case Mode::InGameMenu:
            return MenuContext{ &pauseCamera, &pauseBullet, &pauseBlocks, &pauseCollision };
        default:
            return MenuContext{ nullptr, nullptr, nullptr, nullptr };
    }
}

// -------- update / hit test --------
void _menuScene::menuUpdate(double dt) {
    MenuContext ctx = activeMenuContext();
    if (!ctx.bullet || !ctx.blocks || !ctx.collision) return;

    ctx.bullet->bulletActions(dt);
    if (!ctx.bullet->live) return;

    constexpr float kBulletRadius = 0.5f;
    for (const auto& block : *ctx.blocks) {
        bool hit = ctx.collision->isSphereCol(
            ctx.bullet->pos, block.centerPosition,
            kBulletRadius, block.collisionRadius, 0.0f
        );
        if (!hit) continue;

        // reset bullet
        ctx.bullet->actionTrigger = ctx.bullet->READY;
        ctx.bullet->live = false;

        // dispatch
        if (mode == Mode::MainMenu) {
            switch (block.kind) {
                case MenuBlock::Type::StartGame:
                    if (onStartLevel) onStartLevel(defaultLevelId);
                    return;
                case MenuBlock::Type::Help:
                    if (onHelp) onHelp();
                    return;
                case MenuBlock::Type::Quit:
                    if (onQuit) onQuit();
                    return;
                default: break;
            }
        } else if (mode == Mode::InGameMenu) {
            switch (block.kind) {
                case MenuBlock::Type::Resume:    if (onResume)     onResume();     return;
                case MenuBlock::Type::MainMenu:  if (onGoMainMenu) onGoMainMenu(); return;
                case MenuBlock::Type::Help:      if (onHelp) onHelp();             return;
                case MenuBlock::Type::Quit:      if (onQuit)       onQuit();       return;
                default: break;
            }
        }
        return; // stop after first hit
    }
}

// -------- render --------
void _menuScene::menuRender() {
    //cout<<"menu rendered"<<endl;
    MenuContext ctx = activeMenuContext();
    if (!ctx.camera || !ctx.blocks || !ctx.bullet) return;

    // ---- ensure a sane projection every frame ----
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const double aspect = (height > 0) ? (double)width / (double)height : 1.0;
    gluPerspective(60.0, aspect, 0.1, 1000.0);

    // ---- view ----
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    ctx.camera->setUpCamera();

    // keep depth test on by default
    glEnable(GL_DEPTH_TEST);
    // Optional: draw skybox in main menu
    if (mode == Mode::MainMenu && menuBackground && !gSkyLoaded) {
        menuBackground->skyBoxInit();
        menuBackground->tex[0] = menuBackground->textures->loadTexture("images/front.jpg");
        menuBackground->tex[1] = menuBackground->textures->loadTexture("images/back.jpg");
        menuBackground->tex[2] = menuBackground->textures->loadTexture("images/top.jpg");
        menuBackground->tex[3] = menuBackground->textures->loadTexture("images/bottom.jpg");
        menuBackground->tex[4] = menuBackground->textures->loadTexture("images/right.jpg");
        menuBackground->tex[5] = menuBackground->textures->loadTexture("images/left.jpg");
        gSkyLoaded = true;
        }
    // Draw skybox in BOTH Main Menu and In-Game Menu
    if ((mode == Mode::MainMenu || mode == Mode::InGameMenu) && menuBackground) {
        ensureMenuSkyboxLoaded(menuBackground);
        glDisable(GL_DEPTH_TEST);
        menuBackground->drawSkyBox();
        glEnable(GL_DEPTH_TEST);
    }

    // Blocks

    float cubeEdge = (mode == Mode::InGameMenu) ? kPauseMenuCubeEdge : kMainMenuCubeEdge;


    for (const auto& block : *ctx.blocks) {
        glPushMatrix();
        glTranslatef(block.centerPosition.x, block.centerPosition.y, block.centerPosition.z);

        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        switch (block.kind) {
            case MenuBlock::Type::StartGame: glColor3f(1.0f, 0.8f, 0.2f); break;
            case MenuBlock::Type::Help:      glColor3f(0.2f, 0.6f, 1.0f); break;
            case MenuBlock::Type::Quit:      glColor3f(0.9f, 0.2f, 0.2f); break;
            case MenuBlock::Type::Resume:    glColor3f(1.0f, 0.9f, 0.3f); break;
            case MenuBlock::Type::MainMenu:  glColor3f(0.8f, 0.8f, 0.8f); break;
        }
        glutSolidCube(cubeEdge);
        glPopAttrib();

        // label
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        drawBitmapText3D(menuBlockLabel(block.kind),
                         block.centerPosition.x - 0.7f,
                         block.centerPosition.y + 1.2f,
                         block.centerPosition.z + 0.1f);
        glPopAttrib();

        glPopMatrix();
    }

    // Bullet
    glPushMatrix();
    glScalef(0.02f,0.02f,0.02f);
    glRotatef(-90.0f, 1, 0 ,0 );
    glRotatef(90.0f, 0,0,1);
    glTranslatef(0.0f,0.0f,-200.0f);
    menuModel->Draw();
    glPopMatrix();
    ctx.bullet->drawBullet();


}


// -------- actions --------
void _menuScene::spawnBullet() {
    MenuContext ctx = activeMenuContext();
    if (!ctx.camera || !ctx.bullet) return;

    // Aim from camera eye -> world point under cursor (msX, msY, msZ)
    //vec3 eye { ctx.camera->eye.x, ctx.camera->eye.y, ctx.camera->eye.z };
    //from the model. like he is throwing it at it.
    vec3 eye { 0.0f,-10.0f,-10.0f };
    vec3 hit { static_cast<float>(msX), static_cast<float>(msY), static_cast<float>(msZ) };

    vec3 dir { hit.x - eye.x, hit.y - eye.y, hit.z - eye.z };
    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
    if (len < 1e-5f) return;
    dir.x/=len; dir.y/=len; dir.z/=len;

    ctx.bullet->src  = eye;
    ctx.bullet->des  = { hit.x, hit.y, hit.z }; // straight to unprojected point
    ctx.bullet->pos  = ctx.bullet->src;
    ctx.bullet->t    = 0.0f;
    ctx.bullet->live = true;
    ctx.bullet->actionTrigger = ctx.bullet->SHOOT;
}

// -------- helpers --------
const char* _menuScene::menuBlockLabel(MenuBlock::Type t) const {
    switch (t) {
        case MenuBlock::Type::StartGame: return "Start";
        case MenuBlock::Type::Help:      return "Help";
        case MenuBlock::Type::Quit:      return "Quit";
        case MenuBlock::Type::Resume:    return "Resume";
        case MenuBlock::Type::MainMenu:  return "Main Menu";
        default: return "";
    }
}

void _menuScene::drawBitmapText3D(const char* text, float x, float y, float z) {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glRasterPos3f(x, y, z);
    for (const char* p = text; *p; ++p)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
    glPopAttrib();
}

void _menuScene::handleBulletHits()
{
    pauseCamera = menuCamera; // inherit aim
    pauseBlocks = {
        { {  0.0f,  3.0f, -12.0f }, 1.0f, MenuBlock::Type::Resume   },
        { {  0.0f,  1.0f, -12.0f }, 1.0f, MenuBlock::Type::Help     }, // added Help
        { {  0.0f, -1.0f, -12.0f }, 1.0f, MenuBlock::Type::MainMenu },
        { {  0.0f, -3.0f, -12.0f }, 1.0f, MenuBlock::Type::Quit     },
    };

    pauseBullet.actionTrigger = pauseBullet.READY;
    pauseBullet.live = false;
    pauseBullet.t = 0.0f;
}

