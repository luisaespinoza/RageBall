#include "_scene.h"

_Scene::_Scene()
{
    //ctor

}

_Scene::~_Scene()
{
    //dtor
}
_Scene::_Scene(std::unique_ptr<ILevel> lvl)
: level(std::move(lvl)) {}

void _Scene::setLevel(std::unique_ptr<ILevel> lvl) {
    // optional: unload old one first
    if (level) level->unloadAssets();
    level = std::move(lvl);
}

void _Scene::onEnter() {
    if (!level) return;
    level->loadAssets();
    level->reset();
}

void _Scene::onExit() {
    if (level) level->unloadAssets();
}

//void _Scene::onResize(int w, int h) {
//    applyProjection(w, h);
//}

void _Scene::update(double dt) {
    if (level) level->update(dt);
}

void _Scene::render() {
    // Clear + camera + (optional) light
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    applyFrameCameraAndLight();

    RenderFlags flags;  // Toggle here if you later add pause/overlay conditions
    if (level) level->render(flags);
}

int _Scene::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg)
        {

        case WM_LBUTTONDOWN:
        {
            // Only throw if we're in gameplay and the current level is _level01
            if (level) {
                if (auto* lvl = dynamic_cast<_level01*>(level.get())) {
                    mouseMapping(LOWORD(lParam), HIWORD(lParam));   // fills msX/msY/msZ
                    lvl->throwBallAtWorld(msX, msY, msZ);
                    return 0; // consumed
                }
                // else: not a gameplay level (e.g., a menu scene or no level) → ignore
            }
        }
            break;

        case WM_RBUTTONDOWN:

            break;

         case WM_MBUTTONDOWN:


            break;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:

            break;

        case WM_MOUSEMOVE:

            break;
        case WM_MOUSEWHEEL:

            break;

        default:
            break;

    }
    return 0;
}

// ---------- helpers ----------
void _Scene::applyProjection(int w, int h) {
    if (h == 0) h = 1;
    const float aspect = static_cast<float>(w)/static_cast<float>(h);
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void _Scene::applyFrameCameraAndLight() {
    glLoadIdentity();
    camera.setUpCamera();                 // your existing camera

    if (lightingEnabled) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
        glEnable(GL_COLOR_MATERIAL);
        GLfloat Lpos[] = { 0.f, 6.f, 10.f, 1.f };
        glLightfv(GL_LIGHT0, GL_POSITION, Lpos);  // set each frame after camera
    }
}

//void _Scene::initGL()
//{
//    //sanity logic. ensure this function executes once on accidental double calls
//    static bool done = false;
//    if(done) return;
//    done =true;
//
//    glShadeModel(GL_SMOOTH); // to handle GPU shaders
//    glClearColor(0.0f,0.0f,0.0f,0.0f); // black background color
//    glClearDepth(1.0f);         //depth test for layers
//    glEnable(GL_DEPTH_TEST);    //activate depth test
//    glDepthFunc(GL_LEQUAL);     // depth function type
//
//    glEnable(GL_TEXTURE_2D);
//
//    globalLight.setLight(GL_LIGHT0);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
////
//    //menuBackground->skyBoxInit();
//    //menuBackground->tex[0] = menuBackground->textures->loadTexture("images/front.jpg");//front/posz
//    //menuBackground->tex[1] = menuBackground->textures->loadTexture("images/back.jpg"); //back/negz
//    //menuBackground->tex[2] = menuBackground->textures->loadTexture("images/top.jpg");  //top/posy
//    //menuBackground->tex[3] = menuBackground->textures->loadTexture("images/bottom.jpg"); //bottom/negy
//    //menuBackground->tex[4] = menuBackground->textures->loadTexture("images/right.jpg");  //right/posx
//    //menuBackground->tex[5] = menuBackground->textures->loadTexture("images/left.jpg");  //left/negx
//}

void _Scene::mouseMapping(int x, int y)
{
    GLint viewPort[4];
    GLdouble ModelViewM[16];
    GLdouble projectionM[16];
    GLfloat winX,winY,winZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, ModelViewM);
    glGetDoublev(GL_PROJECTION_MATRIX,projectionM);
    glGetIntegerv(GL_VIEWPORT,viewPort);

    winX =(GLfloat)x;
    winY = (GLfloat)y;

    glReadPixels(x,(int)winY,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&winZ);
    gluUnProject(winX,winY,winZ,ModelViewM,projectionM,viewPort,&msX,&msY,&msZ);
}

//void _level01::throwBallFromRay(float nx, float ny, float nz, float fx, float fy, float fz) {
//    if (!player) return;
//
//    // 1) Ray direction (near→far), normalized
//    vec3 n{nx,ny,nz}, d{fx - nx, fy - ny, fz - nz};
//    float L = std::sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
//    if (L < 1e-6f) return;
//    d.x/=L; d.y/=L; d.z/=L;
//
//    // 2) Build a far-away target along the ray from the PLAYER (direction matters)
//    const float aimDist = 50.0f;
//    vec3 target{
//        player->position.x + d.x * aimDist,
//        player->position.y + d.y * aimDist,
//        player->position.z + d.z * aimDist
//    };
//
//    // 3) Fire toward that target (sets baseDir + initializes projectile state)
//    player->throwAt(target, /*speed*/18.0f, /*spreadDeg*/5.0f);
//
//    // 4) Scale-aware muzzle: offset from player using hallway forward
//    const _hallway& H = halls[currentHallIndex];                 // authoritative hall this frame
//    vec3 fwd = H.toWorldDir({0,0,-1});                           // world forward of hall
//    const float r = player->radius;                               // already matches visual scale
//
//    vec3 muzzle{
//        player->position.x + fwd.x * (0.30f * r),
//        player->position.y + 1.20f * r,
//        player->position.z + fwd.z * (0.30f * r)
//    };
//
//    // 5) Override projectile spawn to this muzzle and set bullet radius from player scale
//    player->ball->src = muzzle;
//    player->ball->pos = muzzle;
//    player->ball->radius = std::max(0.02f, 0.40f * r);           // one truth
//}
//

//legacy resize logic. No longer used. See onResize() and applyProjection()
//void _Scene::reSizeScene(int width, int height)
//{
//    float aspectRatio = (float)width/(float)height;// keep track of the ratio
//    glViewport(0,0,width,height); // adjust my viewport
//
//    glMatrixMode(GL_PROJECTION);  // To setup ptrojection
//    glLoadIdentity();             // calling identity matrix
//    gluPerspective(45, aspectRatio,0.1,1000.0); // setting perspective projection
//
//    //this->width = GetSystemMetrics(SM_CXSCREEN);
//    //this->height= GetSystemMetrics(SM_CYSCREEN);
//    this->width = width;
//    this->height = height;
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();             // calling identity matrix
//}
//
//void _Scene::initGL()
//{
//    glShadeModel(GL_SMOOTH); // to handle GPU shaders
//    glClearColor(0.0f,0.0f,0.0f,0.0f); // black background color
//    glClearDepth(1.0f);         //depth test for layers
//    glEnable(GL_DEPTH_TEST);    //activate depth test
//    glDepthFunc(GL_LEQUAL);     // depth function type
//
//    glEnable(GL_TEXTURE_2D);
//
//    globalLight.setLight(GL_LIGHT0);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//
//    menuBackground->skyBoxInit();
//    menuBackground->tex[0] = menuBackground->textures->loadTexture("images/front.jpg");//front/posz
//    menuBackground->tex[1] = menuBackground->textures->loadTexture("images/back.jpg"); //back/negz
//    menuBackground->tex[2] = menuBackground->textures->loadTexture("images/top.jpg");  //top/posy
//    menuBackground->tex[3] = menuBackground->textures->loadTexture("images/bottom.jpg"); //bottom/negy
//    menuBackground->tex[4] = menuBackground->textures->loadTexture("images/right.jpg");  //right/posx
//    menuBackground->tex[5] = menuBackground->textures->loadTexture("images/left.jpg");  //left/negx
//}
//bool _Scene::isAnyMenu() const {
//    return currentMode == SceneMode::MainMenu || currentMode == SceneMode::PauseOverlay;
//}
//bool _Scene::shouldDrawDimOverlay() const {
//    return currentMode == SceneMode::PauseOverlay;
//}
//void _Scene::drawScene()
//{
//    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);//clear bits in each itteration
//    glLoadIdentity();             // calling identity matrix
//
//
//    switch (currentMode) {
//        case SceneMode::MainMenu:
//        //menuBackground->drawSkyBox();
//        menuUpdate();
//        menuRender();
//        break;
//
//        case SceneMode::Gameplay:
//    // gameplay update/render
//        break;
//
//        case SceneMode::PauseOverlay:
//    // draw gameplay world first (no update), then:
//        menuUpdate();   // advances the pause bullet + resolves hits
//        menuRender();   // draws dim overlay + pause blocks + bullet
//        break;
//    }
//}
//
//
//void _Scene::mouseMapping(int x, int y)
//{
//    GLint viewPort[4];
//    GLdouble ModelViewM[16];
//    GLdouble projectionM[16];
//    GLfloat winX,winY,winZ;
//
//    glGetDoublev(GL_MODELVIEW_MATRIX, ModelViewM);
//    glGetDoublev(GL_PROJECTION_MATRIX,projectionM);
//    glGetIntegerv(GL_VIEWPORT,viewPort);
//
//    winX =(GLfloat)x;
//    winY = (GLfloat)y;
//
//    glReadPixels(x,(int)winY,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&winZ);
//    gluUnProject(winX,winY,winZ,ModelViewM,projectionM,viewPort,&msX,&msY,&msZ);
//}
//
//
//
//
//int _Scene::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    switch(uMsg)
//    {
//        case WM_KEYDOWN:
//
//        break;
//
//        case WM_KEYUP:
//
//        break;
//
//        case WM_LBUTTONDOWN:
//            if (_Scene::isAnyMenu()) {
//                mouseMapping(LOWORD(lParam), HIWORD(lParam));
//                spawnBullet();
//                return 0;
//            }
//
//            break;
//
//        case WM_RBUTTONDOWN:
//
//            break;
//
//         case WM_MBUTTONDOWN:
//
//
//            break;
//
//        case WM_LBUTTONUP:
//        case WM_RBUTTONUP:
//        case WM_MBUTTONUP:
//
//            break;
//
//        case WM_MOUSEMOVE:
//
//            break;
//        case WM_MOUSEWHEEL:
//
//            break;
//
//        default:
//            break;
//
//    }
//}
//
//void _Scene::enterMainMenu()
//{
//    menuCamera.camInit();
//    menuBlocks = {
//        { { -3.0f, 0.0f, -20.0f }, 1.2f, MenuBlock::Type::StartGame },
//        { {  0.0f, 0.0f, -20.0f }, 1.2f, MenuBlock::Type::Help   },
//        { {  3.0f, 0.0f, -20.0f }, 1.2f, MenuBlock::Type::Quit      },
//    };
//
//    menuBullet.actionTrigger = menuBullet.READY;  // your enum
//    menuBullet.live = false;
//    menuBullet.t = 0.0f;
//}
//void _Scene::enterPause()
//{
//    pauseCamera = menuCamera; // inherit aim
//    pauseBlocks = {
//        { {  0.0f,  2.0f, -12.0f }, 1.0f, MenuBlock::Type::Resume  },
//        { {  0.0f,  0.0f, -12.0f }, 1.0f, MenuBlock::Type::Help },
//        { {  0.0f, -2.0f, -12.0f }, 1.0f, MenuBlock::Type::Quit    },
//    };
//    pauseBullet.actionTrigger = pauseBullet.READY;
//    pauseBullet.live = false;
//    pauseBullet.t = 0.0f;
//}
//
//_Scene::MenuContext _Scene::activeMenuContext()
//{
//    switch (currentMode) {
//        case SceneMode::MainMenu:
//            return MenuContext{ &menuCamera, &menuBullet, &menuBlocks, &menuCollision };
//        case SceneMode::PauseOverlay:
//            return MenuContext{ &pauseCamera, &pauseBullet, &pauseBlocks, &pauseCollision };
//        default:
//            return MenuContext{ nullptr, nullptr, nullptr, nullptr };
//    }
//}
//
//
//void _Scene::setMode(SceneMode newMode)
//{
//    currentMode = newMode;
//    switch (currentMode) {
//        case SceneMode::MainMenu:     enterMainMenu();    break;
//        case SceneMode::PauseOverlay: enterPause();       break;
//        case SceneMode::Gameplay:   break;
//    }
//}
//
//
//
//
//void _Scene::menuUpdate() {
//    MenuContext ctx = activeMenuContext();
//    if (!ctx.bullet || !ctx.blocks || !ctx.collision) return;
//
//    ctx.bullet->bulletActions();
//    if (!ctx.bullet->live) return;
//    constexpr float kBulletRadius = 0.5f;
//    for (const auto& block : *ctx.blocks) {
//        bool hit = ctx.collision->isSphereCol(
//            ctx.bullet->pos, block.centerPosition,
//            kBulletRadius, block.collisionRadius, 0.0f
//        );
//        if (!hit) continue;
//
//        // reset bullet
//        ctx.bullet->actionTrigger = ctx.bullet->READY;
//        ctx.bullet->live = false;
//
//        if (currentMode == SceneMode::MainMenu) {
//            switch (block.kind) {
//                case MenuBlock::Type::StartGame: setMode(SceneMode::Gameplay); break;
//                case MenuBlock::Type::Help:   /* TODO: show options */       break;
//                case MenuBlock::Type::Quit:      PostQuitMessage(0);            break;
//                default: break;
//            }
//        } else if (currentMode == SceneMode::PauseOverlay) {
//            switch (block.kind) {
//                case MenuBlock::Type::Resume:  setMode(SceneMode::Gameplay);    break;
//                case MenuBlock::Type::MainMenu: setMode(SceneMode::MainMenu);   break;
//                case MenuBlock::Type::Help: /* TODO: show options overlay */  break;
//                case MenuBlock::Type::Quit:    PostQuitMessage(0);              break;
//                default: break;
//            }
//        }
//        return; // stop after first hit
//    }
//}
//
//void _Scene::menuRender() {
//    MenuContext ctx = activeMenuContext();
//    if (!ctx.camera || !ctx.blocks || !ctx.bullet) return;
//
//    // Dim overlay only when paused
//    if (shouldDrawDimOverlay()) {
//        glDisable(GL_DEPTH_TEST);
//        glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();
//        glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D(0,1,0,1);
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glColor4f(0.0f, 0.0f, 0.0f, 0.35f);
//        glBegin(GL_QUADS);
//            glVertex2f(0,0); glVertex2f(1,0); glVertex2f(1,1); glVertex2f(0,1);
//        glEnd();
//        glMatrixMode(GL_PROJECTION); glPopMatrix();
//        glMatrixMode(GL_MODELVIEW);  glPopMatrix();
//        glEnable(GL_DEPTH_TEST);
//    }
//
//    // View then blocks
//    ctx.camera->setUpCamera();
//    if(currentMode==SceneMode::MainMenu)
//        {
//            glDisable(GL_DEPTH_TEST);
//            menuBackground->drawSkyBox();
//            glEnable(GL_DEPTH_TEST);
//        };
//    float cubeEdge = (currentMode == SceneMode::PauseOverlay) ? 1.6f : 2.0f;
//    for (const auto& block : *ctx.blocks) {
//        glPushMatrix();
//        glTranslatef(block.centerPosition.x, block.centerPosition.y, block.centerPosition.z);
//        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT);
//        glDisable(GL_LIGHTING);
//        glDisable(GL_TEXTURE_2D);
//        switch (block.kind) {
//            case MenuBlock::Type::StartGame: glColor3f(1.0f, 0.8f, 0.2f); break;
//            case MenuBlock::Type::Help:      glColor3f(0.2f, 0.6f, 1.0f); break;
//            case MenuBlock::Type::Quit:      glColor3f(0.9f, 0.2f, 0.2f); break;
//            case MenuBlock::Type::Resume:    glColor3f(1.0f, 0.9f, 0.3f); break;
//            //default:                         glColor3f(1.0f, 1.0f, 1.0f); break;
//        }
//
//        glutSolidCube(cubeEdge);
//
//
//        glPopAttrib();   // restores color/lighting/texture to previous state
//        glPopMatrix();
//    }
//        glColor3f(1,1,1);
//        glEnable(GL_TEXTURE_2D);
//        glEnable(GL_LIGHTING);
//    // Bullet
//    ctx.bullet->drawBullet();
//for (const auto& block : *ctx.blocks) {
//    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT);
//    glDisable(GL_LIGHTING);
//    glDisable(GL_TEXTURE_2D);
//
//    // place label slightly in front to avoid z-fight
//    drawBitmapText3D(
//        menuBlockLabel(block.kind),
//        block.centerPosition.x - 0.7f,
//        block.centerPosition.y + 1.2f,
//        block.centerPosition.z + 0.1f
//    );
//
//    glPopAttrib();
//}}
//
//void _Scene::spawnBullet()
//{
//    _Scene::MenuContext ctx = activeMenuContext();
//    if (!ctx.camera || !ctx.bullet) return;
//
//    // aim from camera eye -> world point under cursor (msX, msY, msZ)
//    vec3 eye { ctx.camera->eye.x, ctx.camera->eye.y, ctx.camera->eye.z };
//    vec3 hit { static_cast<float>(msX), static_cast<float>(msY), static_cast<float>(msZ) };
//
//    vec3 dir { hit.x - eye.x, hit.y - eye.y, hit.z - eye.z };
//    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
//    if (len < 1e-5f) return;
//    dir.x /= len; dir.y /= len; dir.z /= len;
//
//    ctx.bullet->src  = eye;
//    ctx.bullet->des  = {msX,-msY,msZ};
//    ctx.bullet->pos  = ctx.bullet->src;
//    ctx.bullet->t    = 0.0f;
//    ctx.bullet->live = true;
//    ctx.bullet->actionTrigger = ctx.bullet->SHOOT;
//}
//
//
//void _Scene::drawBitmapText3D(const char* text, float x, float y, float z) {
//    glDisable(GL_LIGHTING);
//    glRasterPos3f(x, y, z);
//    for (const char* p = text; *p; ++p) {
//        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
//    }
//    glEnable(GL_LIGHTING);
//}
