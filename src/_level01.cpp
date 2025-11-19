#include "_level01.h"
static GLuint gActiveObstacleTex = 0;


static void DrawTexturedCube()
{
    glEnable(GL_TEXTURE_2D);
    if (gActiveObstacleTex) glBindTexture(GL_TEXTURE_2D, gActiveObstacleTex);

    glBegin(GL_QUADS);
      // +X
      glNormal3f(1,0,0);
      glTexCoord2f(0,0); glVertex3f(+0.5f,-0.5f,-0.5f);
      glTexCoord2f(1,0); glVertex3f(+0.5f,-0.5f,+0.5f);
      glTexCoord2f(1,1); glVertex3f(+0.5f,+0.5f,+0.5f);
      glTexCoord2f(0,1); glVertex3f(+0.5f,+0.5f,-0.5f);
      // -X
      glNormal3f(-1,0,0);
      glTexCoord2f(0,0); glVertex3f(-0.5f,-0.5f,+0.5f);
      glTexCoord2f(1,0); glVertex3f(-0.5f,-0.5f,-0.5f);
      glTexCoord2f(1,1); glVertex3f(-0.5f,+0.5f,-0.5f);
      glTexCoord2f(0,1); glVertex3f(-0.5f,+0.5f,+0.5f);
      // +Y
      glNormal3f(0,1,0);
      glTexCoord2f(0,0); glVertex3f(-0.5f,+0.5f,-0.5f);
      glTexCoord2f(1,0); glVertex3f(+0.5f,+0.5f,-0.5f);
      glTexCoord2f(1,1); glVertex3f(+0.5f,+0.5f,+0.5f);
      glTexCoord2f(0,1); glVertex3f(-0.5f,+0.5f,+0.5f);
      // -Y
      glNormal3f(0,-1,0);
      glTexCoord2f(0,0); glVertex3f(-0.5f,-0.5f,+0.5f);
      glTexCoord2f(1,0); glVertex3f(+0.5f,-0.5f,+0.5f);
      glTexCoord2f(1,1); glVertex3f(+0.5f,-0.5f,-0.5f);
      glTexCoord2f(0,1); glVertex3f(-0.5f,-0.5f,-0.5f);
      // +Z
      glNormal3f(0,0,1);
      glTexCoord2f(0,0); glVertex3f(-0.5f,-0.5f,+0.5f);
      glTexCoord2f(1,0); glVertex3f(+0.5f,-0.5f,+0.5f);
      glTexCoord2f(1,1); glVertex3f(+0.5f,+0.5f,+0.5f);
      glTexCoord2f(0,1); glVertex3f(-0.5f,+0.5f,+0.5f);
      // -Z
      glNormal3f(0,0,-1);
      glTexCoord2f(0,0); glVertex3f(+0.5f,-0.5f,-0.5f);
      glTexCoord2f(1,0); glVertex3f(-0.5f,-0.5f,-0.5f);
      glTexCoord2f(1,1); glVertex3f(-0.5f,+0.5f,-0.5f);
      glTexCoord2f(0,1); glVertex3f(+0.5f,+0.5f,-0.5f);
    glEnd();
}

// obstacle draw proc — used in addObstacleLocal callbacks
// Renders one cube obstacle; picks up the active hall's wall texture via gActiveObstacleTex
static void DrawCubeInstance()
{
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT);

    glEnable(GL_TEXTURE_2D);
    if (gActiveObstacleTex) glBindTexture(GL_TEXTURE_2D, gActiveObstacleTex);

    // Make sure the cube is actually visible even with lighting enabled globally.
    //  draw unlit
    glDisable(GL_LIGHTING);
    glColor4f(1.f, 1.f, 1.f, 1.f);

    // Optional robustness: avoid disappearing faces if winding/cull differs
    // glDisable(GL_CULL_FACE);

    DrawTexturedCube();

    glPopAttrib();
}
static std::string resolveRelativeTo(const std::string& baseFile, const std::string& maybeRel)
{
    if (maybeRel.size() > 1 && (maybeRel[1] == ':' || maybeRel[0] == '/'))
        return maybeRel; // already absolute (Windows or Unix)

    size_t slash = baseFile.find_last_of("/\\");
    std::string dir = (slash == std::string::npos) ? "" : baseFile.substr(0, slash + 1);
    return dir + maybeRel;
}
bool _level01::loadFromTextFile(const std::string& path)
{
    using std::string;
    //halls.clear();

    // --- Parse-time state from file (applies to ALL halls) ---
    string themeFloor, themeWall, themeCeil;
    bool   haveTheme  = false;

    int tileFx=2, tileFy=2, tileWx=2, tileWy=2, tileCx=2, tileCy=2;
    bool haveTiling = false;

    std::vector<_hallway> built;
    _hallway current;
    bool hasCurrent = false;

    std::ifstream in(path.c_str());
    if (!in) return false;

    // Resolve relative texture paths against folder of the level file
 auto resolveRelativeTo = [](const std::string& baseFile, const std::string& maybeRel) -> std::string {
    // Absolute paths (Windows "C:\..." or Unix "/...") — just return as-is
    if (maybeRel.size() > 1 && (maybeRel[1] == ':' || maybeRel[0] == '/'))
        return maybeRel;

    // Find the last directory separator in baseFile
    size_t slash = baseFile.find_last_of("/\\");
    std::string dir = (slash == std::string::npos) ? "" : baseFile.substr(0, slash + 1);

    // Join directory + relative filename
    return dir + maybeRel;
};

    auto commitCurrent = [&]() {
        if (hasCurrent) { built.push_back(current); hasCurrent = false; }
    };

    std::string tok;
    while (in >> tok)
    {
        if (tok.empty()) continue;
        if (tok[0] == '#') { in.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue; }

        if (tok == "THEME") {
            in >> themeFloor >> themeWall >> themeCeil;
            // Single theme applies to all halls in this level file
            themeFloor = resolveRelativeTo(path, themeFloor);
            themeWall  = resolveRelativeTo(path, themeWall);
            themeCeil  = resolveRelativeTo(path, themeCeil);
            haveTheme = true;
            continue;
        }

        if (tok == "TILING") {
            in >> tileFx >> tileFy >> tileWx >> tileWy >> tileCx >> tileCy;
            haveTiling = true;
            continue;
        }

        if (tok == "HALL") {
            float w,h,L; int segs; float yaw,x,y,z;
            in >> w >> h >> L >> segs >> yaw >> x >> y >> z;

            if (!haveTheme) {
                fprintf(stderr, "[level] THEME must appear before HALL (file: %s)\n", path.c_str());
                return false;
            }

            // close previous hall block if any
            commitCurrent();

            current = _hallway{};
            current.attachLoader(&textures);
            current.configure(w,h,L,segs);
            current.setTransform(x,y,z, yaw);

            // Apply single, global tiling (if not provided, defaults stay at 2x)
            if (haveTiling) current.setTiling(tileFx, tileFy, tileWx, tileWy, tileCx, tileCy);
            else            current.setTiling(2,2, 2,2, 2,2);

            // Apply global theme to all halls
            current.loadTheme({ themeFloor.c_str(), themeWall.c_str(), themeCeil.c_str() });

            hasCurrent = true;
            continue;
        }

        if (tok == "ATTACH") {
            float dz = 0.f, yawDelta = 0.f;
            if (!(in >> dz >> yawDelta)) {
                fprintf(stderr, "[level] ATTACH needs: ATTACH <dz> <yawDeg>\n");
                return false;
            }

            // Clamp to your design (±45°)
            if (yawDelta > 45.f)  yawDelta = 45.f;
            if (yawDelta < -45.f) yawDelta = -45.f;

            if (!hasCurrent) {
                fprintf(stderr, "[level] ATTACH before any HALL (file: %s)\n", path.c_str());
                return false;
            }

            // Duplicate current hall (keeps geom/theme/tiling) and attach forward
            _hallway next = current;
            next.attachLoader(&textures);

            current.attachNextExact(next, dz);  // place next relative to current

            // Rotate the newly attached hall
            next.pose_.yawDeg += yawDelta;
            if (next.pose_.yawDeg > 180.f)  next.pose_.yawDeg -= 360.f;
            if (next.pose_.yawDeg < -180.f) next.pose_.yawDeg += 360.f;

            built.push_back(current);
            current = next;
            hasCurrent = true;
            continue;
        }

        if (tok == "OBST") {
            if (!hasCurrent) {
                fprintf(stderr, "[level] OBST before any HALL (file: %s)\n", path.c_str());
                return false;
            }
            float ox, oy, oz, size;
            in >> ox >> oy >> oz >> size;
            // Local-space obstacle (as you intended)
            current.addObstacleLocal({{ox,oy,oz}, 0.f}, size, &DrawCubeInstance);
            continue;
        }

        // Unknown token: skip to end of line to be forgiving
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    commitCurrent();
    halls = std::move(built);
    return !halls.empty();
}

void renderHallSeam(const _hallway& A, const _hallway& B);
static inline vec3 sub(const vec3& a, const vec3& b){ return {a.x-b.x, a.y-b.y, a.z-b.z}; }
static inline vec3 cross3(const vec3& a, const vec3& b){
    return { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
}
static inline vec3 norm(const vec3& v){
    float L = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    return (L>1e-6f)? vec3{v.x/L, v.y/L, v.z/L} : vec3{0,1,0};
}
static _model* levelTeapot = nullptr;   // will point to our level instance
// Draw a world-space wireframe for a hallway using only toWorld(...)
// No glTranslate/glRotate — computes corners directly in world.
void renderHallSeam(const _hallway& A, const _hallway& B)
{
 // corners (local forward = -Z)
    const float wA=A.w_, wB=B.w_;
    const float hA=A.h_, hB=B.h_;
    const float LA=A.L_;

    // floor corners
    vec3 AFL = A.toWorld({-wA*0.5f, 0.0f, -LA}); // A far left
    vec3 AFR = A.toWorld({+wA*0.5f, 0.0f, -LA}); // A far right
    vec3 BNL = B.toWorld({-wB*0.5f, 0.0f,  0.0f}); // B near left
    vec3 BNR = B.toWorld({+wB*0.5f, 0.0f,  0.0f}); // B near right

    // ceiling corners
    vec3 ACL = A.toWorld({-wA*0.5f, hA, -LA});
    vec3 ACR = A.toWorld({+wA*0.5f, hA, -LA});
    vec3 BCL = B.toWorld({-wB*0.5f, hB,  0.0f});
    vec3 BCR = B.toWorld({+wB*0.5f, hB,  0.0f});

    // slight polygon offset to avoid z-fighting against hall caps (optional)
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    // --- FLOOR (y up) ---
    if (A.texFloor_) glBindTexture(GL_TEXTURE_2D, A.texFloor_);
    glBegin(GL_TRIANGLES);
      glNormal3f(0,1,0);
      // AFL -> AFR -> BNR
      glTexCoord2f(0,        0);   glVertex3f(AFL.x, AFL.y, AFL.z);
      glTexCoord2f(A.fU_,    0);   glVertex3f(AFR.x, AFR.y, AFR.z);
      glTexCoord2f(A.fU_, A.fV_);  glVertex3f(BNR.x, BNR.y, BNR.z);
      // AFL -> BNR -> BNL
      glTexCoord2f(0,        0);   glVertex3f(AFL.x, AFL.y, AFL.z);
      glTexCoord2f(A.fU_, A.fV_);  glVertex3f(BNR.x, BNR.y, BNR.z);
      glTexCoord2f(0,    A.fV_);   glVertex3f(BNL.x, BNL.y, BNL.z);
    glEnd();

    // --- CEILING (y down) ---
    if (A.texCeil_) glBindTexture(GL_TEXTURE_2D, A.texCeil_);
    glBegin(GL_TRIANGLES);
      glNormal3f(0,-1,0);
      // ACR -> ACL -> BCL
      glTexCoord2f(A.cU_,   0);    glVertex3f(ACR.x, ACR.y, ACR.z);
      glTexCoord2f(0,       0);    glVertex3f(ACL.x, ACL.y, ACL.z);
      glTexCoord2f(0,    A.cV_);   glVertex3f(BCL.x, BCL.y, BCL.z);
      // ACR -> BCL -> BCR
      glTexCoord2f(A.cU_,   0);    glVertex3f(ACR.x, ACR.y, ACR.z);
      glTexCoord2f(0,    A.cV_);   glVertex3f(BCL.x, BCL.y, BCL.z);
      glTexCoord2f(A.cU_, A.cV_);  glVertex3f(BCR.x, BCR.y, BCR.z);
    glEnd();

    // --- RIGHT WALL (A far-right ↔ B near-right) ---
    if (A.texWall_) glBindTexture(GL_TEXTURE_2D, A.texWall_);
    vec3 ARB = AFR, ART = ACR;  // A right bottom/top
    vec3 BRB = BNR, BRT = BCR;  // B right bottom/top
    vec3 nR = norm(cross3(sub(ART,ARB), sub(BRB,ARB)));
    glBegin(GL_TRIANGLES);
      glNormal3f(nR.x,nR.y,nR.z);
      // ARB -> ART -> BRT
      glTexCoord2f(0,       0);    glVertex3f(ARB.x, ARB.y, ARB.z);
      glTexCoord2f(0,    A.wlV_);  glVertex3f(ART.x, ART.y, ART.z);
      glTexCoord2f(A.wlU_,A.wlV_); glVertex3f(BRT.x, BRT.y, BRT.z);
      // ARB -> BRT -> BRB
      glTexCoord2f(0,       0);    glVertex3f(ARB.x, ARB.y, ARB.z);
      glTexCoord2f(A.wlU_,A.wlV_); glVertex3f(BRT.x, BRT.y, BRT.z);
      glTexCoord2f(A.wlU_,  0);    glVertex3f(BRB.x, BRB.y, BRB.z);
    glEnd();

    // --- LEFT WALL (A far-left ↔ B near-left) ---
    vec3 ALB = AFL, ALT = ACL;  // A left bottom/top
    vec3 BLB = BNL, BLT = BCL;  // B left bottom/top
    vec3 nL = norm(cross3(sub(ALT,ALB), sub(BLB,ALB)));
    glBegin(GL_TRIANGLES);
      glNormal3f(nL.x,nL.y,nL.z);
      // ALB -> BLB -> BLT
      glTexCoord2f(0,       0);    glVertex3f(ALB.x, ALB.y, ALB.z);
      glTexCoord2f(A.wlU_,  0);    glVertex3f(BLB.x, BLB.y, BLB.z);
      glTexCoord2f(A.wlU_,A.wlV_); glVertex3f(BLT.x, BLT.y, BLT.z);
      // ALB -> BLT -> ALT
      glTexCoord2f(0,       0);    glVertex3f(ALB.x, ALB.y, ALB.z);
      glTexCoord2f(A.wlU_,A.wlV_); glVertex3f(BLT.x, BLT.y, BLT.z);
      glTexCoord2f(0,    A.wlV_);  glVertex3f(ALT.x, ALT.y, ALT.z);
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);
}
static void DebugDrawHallWire(const _hallway& H, float r=1, float g=1, float b=0)
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor3f(r,g,b);

    const float half = 0.5f * H.length(); // not used, just to remind dimensions
    const float w2 = 0.5f *  H.length();  // (ignore; length handled per z below)

    const float xL = -H.w_ * 0.5f;
    const float xR =  H.w_ * 0.5f;
    const float yB = 0.0f;
    const float yT = H.h_;

    // Near and far z in *local* coordinates (hall faces toward local -Z)
    const float zN = 0.0f;
    const float zF = -H.L_;

    // Compute the 8 box corners in *world* space
    auto W = [&](float x, float y, float z){ return H.toWorld({x,y,z}); };

    vec3 c[8] = {
        W(xL,yB,zN), W(xR,yB,zN), W(xR,yT,zN), W(xL,yT,zN), // near
        W(xL,yB,zF), W(xR,yB,zF), W(xR,yT,zF), W(xL,yT,zF)  // far
    };

    // Wireframe box
    glBegin(GL_LINES);
      // near rectangle
      glVertex3f(c[0].x,c[0].y,c[0].z); glVertex3f(c[1].x,c[1].y,c[1].z);
      glVertex3f(c[1].x,c[1].y,c[1].z); glVertex3f(c[2].x,c[2].y,c[2].z);
      glVertex3f(c[2].x,c[2].y,c[2].z); glVertex3f(c[3].x,c[3].y,c[3].z);
      glVertex3f(c[3].x,c[3].y,c[3].z); glVertex3f(c[0].x,c[0].y,c[0].z);
      // far rectangle
      glVertex3f(c[4].x,c[4].y,c[4].z); glVertex3f(c[5].x,c[5].y,c[5].z);
      glVertex3f(c[5].x,c[5].y,c[5].z); glVertex3f(c[6].x,c[6].y,c[6].z);
      glVertex3f(c[6].x,c[6].y,c[6].z); glVertex3f(c[7].x,c[7].y,c[7].z);
      glVertex3f(c[7].x,c[7].y,c[7].z); glVertex3f(c[4].x,c[4].y,c[4].z);
      // connect near↔far
      glVertex3f(c[0].x,c[0].y,c[0].z); glVertex3f(c[4].x,c[4].y,c[4].z);
      glVertex3f(c[1].x,c[1].y,c[1].z); glVertex3f(c[5].x,c[5].y,c[5].z);
      glVertex3f(c[2].x,c[2].y,c[2].z); glVertex3f(c[6].x,c[6].y,c[6].z);
      glVertex3f(c[3].x,c[3].y,c[3].z); glVertex3f(c[7].x,c[7].y,c[7].z);
    glEnd();

    // Draw forward ray (local -Z) from near center
    vec3 nearC = W(0, 0.5f*(yB+yT), zN);
    vec3 fwdW  = H.toWorldDir({0,0,-1});  // world forward of hall
    glBegin(GL_LINES);
      glColor3f(0,1,1);
      glVertex3f(nearC.x, nearC.y, nearC.z);
      glVertex3f(nearC.x + 5*fwdW.x, nearC.y + 5*fwdW.y, nearC.z + 5*fwdW.z);
    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}


void _level01::loadAssets() {
    halls.clear();
    // Collision Checker //
    collisionChecker = new _collisionCheck();
    // Pickup item -- swap to vector to add multiple later
    pickupItem = new _pickup();
    pickupItem->pickupInit(_pickup::HEALTH);
    pickupItem->pSize = {0.45f,0.6f,0.6f};
    pickupItem->pos = {0.0f, 0.4f, 2.0f};
    pickupItem->life = 1; // one life point on pickup
    // Effect upon picking up health ring -- change for other pickups later -- can add more sprite images if needed
    healthRingEffect = new _sprite();
    healthRingEffect->spriteInit("images/health_ring_effect.png",1,1);
    // Timer for health ring animation //
    healthRingEffectTimer = _timer();
    healthRingEffectTimer.enabled = false;
    if (!player) player = new Player();
    player->init("models/megaman/tris.md2", "models/megaman/megaman.pcx", textures);
    player->applyScale(0.005f);
    player->position = {0.0f, 0.0f, -2.0f};
    player->radius   = 0.05f;
    currentHallIndex = 0;

    bool loaded = false;
    if (!levelPath_.empty()) {
        loaded = loadFromTextFile(levelPath_);
    }

    if (useArena) {
        player->ball->setTrajectory(Trajectory_Parabola());
        arena_.attachLoader(&textures);
        arena_.configure(/*w*/12.f, /*h*/6.f, /*d*/24.f);
        arena_.setTransform(0,0,0, /*yaw*/0.f);
        arena_.setMidlineEnabled(true);
        arena_.setTeamSide(+1);

        // reuse your textures (adjust paths as needed)
        char* roomTexture = "images/top.jpg";
        arena_.loadTheme({ roomTexture,roomTexture,roomTexture });

        // a couple of obstacles, analogous to your demo cubes
        arena_.addObstacleLocal({{ 0.0f, 0.5f, -6.0f }, 0.f}, 0.6f, &DrawCubeInstance);
        arena_.addObstacleLocal({{ 2.0f, 0.5f,  3.5f }, 0.f}, 0.6f, &DrawCubeInstance);

        skyReady = false; // unchanged
        return;
    }

    // ---- original fallback (kept for now if you toggle useArena_=false) ----
    if (!loaded) {
        _hallway h0; h0.attachLoader(&textures);
        h0.configure(4.f, 3.f, 20.f, 10);
        h0.setTiling(2,2, 2,2, 2,2);
        h0.loadTheme({ "img/floor_a.jpg", "img/wall_a.jpg", "img/ceil_a.jpg" });
        h0.setTransform(0,0,0, 0);
        h0.addObstacleLocal({{0,0.5f,-6}, 0.f}, 0.6f, &DrawCubeInstance);

        _hallway h1 = h0; h0.attachNextExact(h1, +20.f);
        h1.loadTheme({ "img/floor_b.jpg", "img/wall_b.jpg", "img/ceil_b.jpg" });
        h1.addObstacleLocal({{1.0f,0.5f,-4}, 0.f}, 0.6f, &DrawCubeInstance);

        _hallway h2 = h1; h1.attachNextExact(h2, -20.f);

        halls = { std::move(h0), std::move(h1), std::move(h2) };
    }
}


bool    skyReady = false; // unchanged
//}


void _level01::unloadAssets()
{
    // Tear down halls & their GPU state
//    for (auto& h : halls) {
//
//    }
    halls.clear();

    // Reset the shared teapot pointer used by obstacle draw
   // levelTeapot = nullptr;
    //teapotModel = _model{};        // or provide a proper .destroy() if you have one

    // Drop the player safely
    if (player) {
        //player->destroy();         // if you have it; otherwise:
        delete player;
        player = nullptr;
    }

    // Drop textures owned by the level
   // textures.unloadAll();          // or equivalent in your loader

    skyReady = false;
}


void _level01::reset()
{
    player->playerResetLife(5);

    if (useArena) {
        // spawn in local space near “near wall”, centerline on X
        const float y0 = 0.5f;
        const float insetZ = -2.0f;
        vec3 pL { 0.0f, y0, insetZ };

        // clamp once, convert to world, face forward (local yaw 0 = -Z)
        pL = arena_.clampLocal(pL, player ? player->radius * 0.95f : 0.4f);
        player->position = arena_.toWorld(pL);
        player->yawDeg   = /*arena yaw*/ 0.0f; // worldYawFromLocal(0) since arena yaw is 0 here
        return;
    }
    if (halls.empty()) {
        _hallway h;
        h.attachLoader(&textures);
        h.configure(4.0f, 3.0f, 20.0f, 10);
        h.setTiling(2,2,  2,2,  2,2);
        h.setTransform(0.0f, 0.0f, 0.0f, 0.0f);
        halls.push_back(h);
    }

    currentHallIndex = 0;
    if (currentHallIndex >= halls.size()) currentHallIndex = 0;

    // Spawn at centerline, slightly inside the hall, on the floor (local space)
    const float y0 = 0.5f;
    const float insetZ = -2.0f;
    vec3 localStart{ 0.0f, y0, insetZ };

    _hallway& H = halls[currentHallIndex];

    // Clamp inside once to avoid spawning exactly on boundaries
    vec3 pL = H.clampLocal(localStart, player ? player->radius * 0.95f : 0.4f);

    // Hallway is authoritative for position + ABSOLUTE orientation (face forward = local yaw 0°)
    H.placePlayerLocal(*player, pL, 0.0f);
}

void _level01::update(double dt)
{

    if (!player) return;

    // --- INPUT ---
    const bool w = (GetAsyncKeyState(KEY_W) & 0x8000) != 0;
    const bool s = (GetAsyncKeyState(KEY_S) & 0x8000) != 0;
    const bool a = (GetAsyncKeyState(KEY_A) & 0x8000) != 0;
    const bool d = (GetAsyncKeyState(KEY_D) & 0x8000) != 0;

    float fwd = (w ? 1.f : 0.f) - (s ? 1.f : 0.f);   // forward = local -Z
    float str = (d ? 1.f : 0.f) - (a ? 1.f : 0.f);   // strafe  = local +X
    float mag = sqrtf(fwd*fwd + str*str);
    if (mag > 1e-6f) { fwd /= mag; str /= mag; }

    if (useArena) {
        // --- LOCAL velocity ---
        const float step = player->speed * (float)dt;
        vec3 vL { str * step, 0.0f, -fwd * step };

        // recompute local pos in arena, convert delta via two points (like halls)
        vec3 pL = arena_.toLocal(player->position);
        vec3 w0 = arena_.toWorld(pL);
        vec3 w1 = arena_.toWorld({ pL.x + vL.x, pL.y + vL.y, pL.z + vL.z });
        vec3 vW { w1.x - w0.x, w1.y - w0.y, w1.z - w0.z };

        // move & clamp inside arena
        player->moveAndClamp(dt, vW, arena_);  // same template you used with halls

        // ------------- Obstacle collisions (same sphere test) -------------
        auto sqr = [](float v){ return v*v; };
        auto hitSphere = [&](float px, float py, float pz, float pr,
                             float ox, float oy, float oz, float orad) {
            const float dx = px - ox, dy = py - oy, dz = pz - oz;
            const float r  = pr + orad;
            return (sqr(dx) + sqr(dy) + sqr(dz)) <= (r*r);
        };

        if (player->hurtCooldown > 0.0f)
            player->hurtCooldown = std::max(0.0f, player->hurtCooldown - static_cast<float>(dt));

        if (player->hurtCooldown <= 0.f) {
            bool tookHit = false;
            arena_.forEachObstacleWorld([&](float ox, float oy, float oz, float radius){
                if (!tookHit && hitSphere(player->position.x, player->position.y, player->position.z, player->radius,
                                          ox, oy, oz, radius)) {
                    tookHit = true;
                }
            });

            if (tookHit) {
                player->life -= 1;
                player->hurtCooldown = 1.0f;
                std::cout << "[player] hit! life=" << player->life << "\n";
                if (player->life <= 0) { this->reset(); return; }
            }
        }

        if (collisionChecker->isSphereCol(pickupItem->pos,player->position,pickupItem->pSize.x,player->radius,0.0f) && !pickupItem->isCollected) {
            pickupItem->applyEffect(player);
            healthRingEffectTimer.enabled = true;
            healthRingEffectTimer.reset();
            healthRingEffect->pos = pickupItem->pos;
            std::cout << "[player] picked up health! life=" << player->life << "\n";
        }

        // --- Facing from local motion → world yaw via arena orientation ---
        if (vL.x*vL.x + vL.z*vL.z > 1e-6f) {
            const float yawLocalDeg = atan2f(/*x=*/vL.x, /*-z*/-vL.z) * 180.0f / PI;
            // arena world yaw = arena_.pose_.yawDeg + local yaw
            player->yawDeg = arena_.pose_.yawDeg + yawLocalDeg;
        }

        player->setAnimForVelocity(vW);
        player->updateBall(dt);
        return;
    }

    // ---- original hallway path (kept behind the flag) ----
 if (!player || halls.empty()) return;

    fwd = (w ? 1.f : 0.f) - (s ? 1.f : 0.f);   // forward = local -Z
    str = (d ? 1.f : 0.f) - (a ? 1.f : 0.f);   // strafe  = local +X
    mag = sqrtf(fwd*fwd + str*str);
    if (mag > 1e-6f) { fwd /= mag; str /= mag; }

    // --- 1) Decide which hallway is active BEFORE computing velocity ---
    _hallway& Hcur = halls[currentHallIndex];
    vec3 pLcur = Hcur.toLocal(player->position);
     float edge = 0.05f;

    // Advance/backtrack hall based on where we are in the *current* hall
    if (fwd > 0.0f && pLcur.z < -Hcur.length() + edge && currentHallIndex + 1 < (int)halls.size()) {
        ++currentHallIndex;
    } else if (fwd < 0.0f && pLcur.z > -edge && currentHallIndex > 0) {
        --currentHallIndex;
    }

    // H is the authoritative hallway for THIS frame
    _hallway& H = halls[currentHallIndex];

    // IMPORTANT: recompute player local position in the *new* hallway
    vec3 pL = H.toLocal(player->position);

    // --- 2) Compute LOCAL velocity then convert to WORLD as a delta of transformed points ---
    const float step = player->speed * (float)dt;

    // local forward is -Z, strafe is +X
    vec3 vL = { str * step, 0.0f, -fwd * step };

    // Convert local delta to world delta at the player's local position
    vec3 w0 = H.toWorld(pL);
    vec3 w1 = H.toWorld({ pL.x + vL.x, pL.y + vL.y, pL.z + vL.z });
    vec3 vW = { w1.x - w0.x, w1.y - w0.y, w1.z - w0.z };


    // --- 3) Move & clamp inside THIS hallway ---
    player->moveAndClamp(dt, vW, H);


        // ------------- Collision with obstacles -------------
    auto sqr = [](float v){ return v*v; };
    auto hitSphere = [&](float px, float py, float pz, float pr,
                         float ox, float oy, float oz, float orad) {
        const float dx = px - ox, dy = py - oy, dz = pz - oz;
        const float r  = pr + orad;
        return (sqr(dx) + sqr(dy) + sqr(dz)) <= (r*r);
    };

    // tick down hurt cooldown
if (player->hurtCooldown > 0.0f)
    player->hurtCooldown = std::max(0.0f, player->hurtCooldown - static_cast<float>(dt));

    // Only check for damage if not invincible
    if (player->hurtCooldown <= 0.f) {
        bool tookHit = false;

        // Iterate all halls and their obstacles in the current level
        for (const auto& hall : halls) {
            hall.forEachObstacleWorld([&](float ox, float oy, float oz, float radius){
                if (!tookHit && hitSphere(player->position.x, player->position.y, player->position.z, player->radius,
                                          ox, oy, oz, radius)) {
                    tookHit = true;
                }
            });
            if (tookHit) break;
        }

        if (tookHit) {
            player->life -= 1;
            player->hurtCooldown = 1.0f;   // 1 second invulnerability to avoid rapid multi-hit

            // optional: print or flash UI
            std::cout << "[player] hit! life=" << player->life << "\n";

            if (player->life <= 0) {
                // ---- RESET THE LEVEL ----
               this->reset();            // see #4 below
                return;                   // bail out of this frame's update
            }
        }
    }
// --- 4) Absolute facing: steer toward movement direction (world space) ---
    float faceYawWorld = player->yawDeg;
    const float mv2 = vL.x*vL.x + vL.z*vL.z;
    if (mv2 > 1e-6f) {
        // Local yaw: 0° means local -Z (forward), +X is to the right
        const float yawLocalDeg = atan2f(/*x=*/vL.x, /*-z forward=*/-vL.z) * 180.0f / PI;
        // Convert to world yaw using the hallway’s orientation
        faceYawWorld = H.worldYawFromLocal(yawLocalDeg);
    } else {
        // Not moving: either keep current yaw (do nothing), or:
        // faceYawWorld = H.worldYawFromLocal(0.0f);  // align to hall forward when idle
    }
    // If we're on the last hallway and pushing out the far end, request next level once.
    if (!firedNext_
        && currentHallIndex == static_cast<int>(halls.size()) - 1
        && fwd > 0.0f)
    {
        // Re-evaluate local position in the authoritative hall after movement:
        vec3 pL_after = H.toLocal(player->position);
        const float edge = 0.02f;
        if (pL_after.z <= -H.length() + edge) {
            firedNext_ = true;                         // prevent double-fire
            if (requestNextLevel_ && !nextLevelId_.empty()) {
                requestNextLevel_(nextLevelId_);       // hand off to scene manager
            }
        }
    }
// TODO: (Not currently working) Turn toward the target yaw with a capped angular speed
//auto shortestDeltaDeg = [](float from, float to) {
//    float d = fmodf(to - from + 540.0f, 360.0f) - 180.0f; // [-180, 180)
//    return d;
//};
//
//const float turnSpeedDegPerSec = 720.0f;              // tweak to taste
//const float maxStep = turnSpeedDegPerSec * (float)dt;
//float delta = shortestDeltaDeg(player->yawDeg, faceYawWorld);
//if (fabsf(delta) > maxStep) delta = (delta > 0.f ? maxStep : -maxStep);
//player->yawDeg += delta;

// (Optional) animation from motion
    player->setAnimForVelocity(vW);
    player->updateBall(dt);

}





void _level01::render(const RenderFlags& flags)
{
   // printf("[level01] render\n");
    if (flags.showSky && skyReady) {
        glDisable(GL_LIGHTING);
        glDepthMask(GL_FALSE);
        sky.drawSkyBox();
        glDepthMask(GL_TRUE);
        glEnable(GL_LIGHTING);
    }

    glEnable(GL_LIGHTING);

    if (useArena) {
        glPushMatrix();
        arena_.render();
        glPopMatrix();

        player->render();
        player->ball->drawBullet();
        //pickupItem->pos = player->position; // for testing
        pickupItem->drawSprite();
        if (healthRingEffectTimer.getTicks() < 2000 && healthRingEffectTimer.enabled) { // Effect lasts for two seconds, long but good for testing
            healthRingEffect->pSize.x += 0.006f; // Animation scale effect
            healthRingEffect->pSize.y += 0.006f; 
            healthRingEffect->pSize.z += 0.006f; 
            healthRingEffect->drawSprite();
        }
        glDisable(GL_LIGHTING);
        return;
    }
    glEnable(GL_LIGHTING);
    for (auto& h : halls) {
        glPushMatrix();
        gActiveObstacleTex = h.texWall_;
        h.render();                     // now self-contained for texture state
        glPopMatrix();
    }
    gActiveObstacleTex = 0;

    for (int i = 0; i+1 < (int)halls.size(); ++i) {
        renderHallSeam(halls[i], halls[i+1]);
    }
    player->render();
    player->ball->drawBullet();
    glDisable(GL_LIGHTING);

}







void _level01::handleKey(UINT uMsg, WPARAM wParam) {
    //    vec3 vel = player->inputDelta(dt, player->playerInput);
//    player->setAnimForVelocity(vel);
//    vec3 proposed = { player->position.x + vel.x,
//                      player->position.y + vel.y,
//                      player->position.z + vel.z };
//
//std::cout /*<< "hall yaw=" << H.hallwayWorldYawDeg()*/
//          << " player yaw=" << player->yawDeg
//          << " baseYawMD2=" << player->baseYawMD2 << "\n";

    switch(wParam)
    {
        /*
    case KEY_A:
        player->position.x -= player->speed;
        break;
    case KEY_S:
        player->position.z += player->speed;
        break;
    case KEY_W:
        player->position.z -= player->speed;
        break;
    case KEY_D:
        player->position.x += player->speed;
        break; */

        //debugging yaw
    case VK_OEM_4:
        player->baseYawMD2 -= 5.0f;
        break;
    case VK_OEM_6:
        player->baseYawMD2 += 5.0f;
        break;
    }

//    const bool down = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
//    const bool up   = (uMsg == WM_KEYUP   || uMsg == WM_SYSKEYUP);
//
//    auto& in = *player->playerInput;
//
//    auto setFlag = [&](WPARAM k, bool v) {
//        switch (k) {
//            case KEY_W: in.moveFwd   = v; break;
//            case KEY_S: in.moveBack  = v; break;
//            case KEY_A: in.moveLeft  = v; break;
//            case KEY_D: in.moveRight = v; break;
//        }
//    };
//
//    if (down) setFlag(wParam, true);
//    if (up)   setFlag(wParam, false);
}
void _level01::applyCamera()
{
    static bool birdEye = false;   // for debugging toggle this flag to switch view
    const float eyeH = 1.6f;
    const float dist = 3.0f;

    if (!player||halls.empty()) return;

    if (birdEye)
    {
        // --- Bird's-eye camera ---
        //for debugging
        vec3 center = player->position;
        float height = 180.0f;   // raise this to see the full chain
        vec3 eye = { center.x, height, center.z };
        vec3 at  = { center.x, 0.0f, center.z };
        gluLookAt(eye.x+20, eye.y, eye.z-20,  at.x+20, at.y, at.z-20,  0,0,-1);
        return;
    }
    if (useArena) {
        const float eyeH = 1.6f;
        const float dist = 3.0f;
        vec3 fwd = arena_.toWorldDir({0,0,-1});
        vec3 eye = { player->position.x - fwd.x * dist,
                     player->position.y + eyeH,
                     player->position.z - fwd.z * dist };
        vec3 at  = { player->position.x + fwd.x,
                     player->position.y + 1.0f,
                     player->position.z + fwd.z };
        glMatrixMode(GL_MODELVIEW);
        gluLookAt(eye.x, eye.y, eye.z, at.x, at.y, at.z, 0,1,0);
        return;
        }
    else {
        // --- Normal 3rd-person follow camera ---
    size_t idx = currentHallIndex;
    if (idx >= halls.size()) idx = 0;
    vec3 pL = halls[idx].toLocal(player->position);
    if (pL.z < -halls[idx].length() + 0.05f && idx + 1 < halls.size()) ++idx;
    else if (pL.z > -0.05f && idx > 0) --idx;
    currentHallIndex = idx;

    const _hallway& H = halls[currentHallIndex];

    // --- build camera from the hallway's world forward (+ up) ---
    const float eyeH = 1.6f;   // camera height above player
    const float dist = 3.0f;   // follow distance

    vec3 fwd = H.toWorldDir({0,0,-1});   // unit forward of this hall
    // (optional) vec3 right = H.toWorldDir({1,0,0}); // if you want strafe-based offsets later

    vec3 eye = { player->position.x - fwd.x * dist,
                 player->position.y + eyeH,
                 player->position.z - fwd.z * dist };

    vec3 at  = { player->position.x + fwd.x,
                 player->position.y + 1.0f,
                 player->position.z + fwd.z };

    // make sure we’re on the right matrix
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(eye.x, eye.y, eye.z, at.x, at.y, at.z, 0, 1, 0);
    }
}
// _level01.cpp
#include <cmath>

void _level01::throwBallFromRay(const vec3& rayOrigin, const vec3& rayDir) {
    if (!player) return;

    const float yPlane = player->position.y + 1.0f;
    vec3 targetW{};
    const float denom = rayDir.y;

    if (std::fabs(denom) > 1e-5f) {
        const float t = (yPlane - rayOrigin.y) / denom;
        if (t > 0.0f) {
            targetW = { rayOrigin.x + t*rayDir.x,
                        rayOrigin.y + t*rayDir.y,
                        rayOrigin.z + t*rayDir.z };
        }
    }
    if (targetW.x==0 && targetW.y==0 && targetW.z==0) {
        targetW = { player->position.x + 8.0f * std::cos(player->yawDeg * 3.14159265f/180.f),
                    yPlane,
                    player->position.z - 8.0f * std::sin(player->yawDeg * 3.14159265f/180.f) };
    }

    // fire
    player->throwAt(targetW, /*speed*/18.0f, /*spreadDeg*/2.0f);

    // muzzle from arena forward
    vec3 fwd = arena_.toWorldDir({0,0,-1});
    const float r = player->radius;
    vec3 muzzle{
        player->position.x + fwd.x * (0.30f * r),
        player->position.y + 1.20f * r,
        player->position.z + fwd.z * (0.30f * r)
    };
    player->ball->src = muzzle;
    player->ball->pos = muzzle;

    player->ball->radius = std::max(0.2f, 0.40f * r);
}

void _level01::throwBallAtWorld(double wx, double wy, double wz) {
    if (!player) return;

    // world target
    vec3 target{ static_cast<float>(wx),
                 static_cast<float>(wy),
                 static_cast<float>(wz) };

    // fire toward that target (fills des, speed, spread, etc.)
    player->throwAt(target, /*speed*/ 18.0f, /*spreadDeg*/ 5.0f);

    // --- spawn from a "muzzle" anchored to the scaled player ---
    // use current hallway’s forward to push the spawn a bit in front
    size_t idx = currentHallIndex;
    if (idx >= halls.size()) idx = 0;
    const _hallway& H = halls[idx];
    vec3 fwd = H.toWorldDir({0,0,-1});           // unit forward in world

    const float r = player->radius;               // already scale-aware (applyScale sets it, :contentReference[oaicite:3]{index=3})
    vec3 muzzle{
        player->position.x + fwd.x * (0.30f * r), // slight forward offset
        player->position.y + 1.20f * r,           // chest-ish height
        player->position.z + fwd.z * (0.30f * r)
    };

    // override the source/pos to the muzzle so the shot starts at the chest
    player->ball->src = muzzle;
    player->ball->pos = muzzle;

    // scale the visual size of the bullet to the character scale
    player->ball->radius = std::max(0.02f, 0.40f * r);
}
