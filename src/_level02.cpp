#include "_level02.h"

//construct pointers
_level02::_level02()
{
    textureLoader = new _textureLoader();
    playerHitBox = new _boundingBox();
    ballDeleteTimer = new _timerPlusPlus();
    stunnedTimer = new _timerPlusPlus();
    enemyMoveTimer = new _timerPlusPlus();
    soundEngine = new _sounds();

    //Ball Stuff
    ballTimer = new _timerPlusPlus();
    ballThrowTimer = new _timerPlusPlus();
    ballTimer->reset();
    ballThrowTimer->reset();
}

//Delete once completed
//Stop memory leaks
_level02::~_level02()
{
    unloadAssets();

    delete textureLoader;
    delete playerHitBox;
    delete ballDeleteTimer;
    delete stunnedTimer;
    delete enemyMoveTimer;
    delete ballTimer;
    delete ballThrowTimer;

    textureLoader = nullptr;
    playerHitBox = nullptr;
    ballDeleteTimer = nullptr;
    stunnedTimer = nullptr;
    enemyMoveTimer = nullptr;
    ballTimer = nullptr;
    ballThrowTimer = nullptr;
}

// ==================== OVERRIDE SYSTEMS ===========================
// Main game mechanic loaders
void _level02::loadAssets()
{
    //Load all assets here

    //Pointers
    player = make_unique<Player>();
    characterCam = make_unique<_camera>();

    //PLAYER
    player->init("models/megaman/tris.md2",
                "models/megaman/MegaMan.jpg",
                *textureLoader);
    player->applyScale(0.02f);
    player->position = {0.0f, 0.5f, -2.0f};
    player->radius   = 0.05f;
    player->yawDeg = 90.0f;
    playerHitBox->initBoundingBox(
                                  {0.25f, 0.4f, 0.25f},
                                  {player->position.x,
                                  player->position.y + 0.9f,
                                  player->position.z},
                                  {1.0f, 1.0f, 1.0f});

    //BALLS (Set one ball for vector to copy)
    ballPrototype.initModel("images/dodgeball.jpg",
                             "models/dodge_ball.obj",
                            _model::CUSTOM);
    ballPrototype.scale = {0.15f, 0.15f, 0.15f};
    ballPrototype.addBoundingBox({1.0f, 1.0f, 1.0f},
                                 ballPrototype.position,
                                 ballPrototype.scale);

   //CAMERAS
    characterCam->camInit();

    //CUBES

       //Set cube beginning/end position
    currentState = Cubestate::Waiting;
    cubeModel.initModel("images/target/target.jpg",
                        cubeModel.modelType::CUBE);
    cubeModel.scale = {2.0f, 3.0f, 2.0f};
    cubeModel.color = {1.0f, 0.0f, 0.0f};
       //Set cube positions
    cubeStartPosition = {-6.0f,
                          player->position.y,
                          player->position.z - 2.0f};
    cubeEndPosition = {player->position.x,
                       player->position.y,
                       player->position.z - 5.0f};
    cubeCurrentPosition = cubeStartPosition;

       //CUBE hit box
    cubeBox.initBoundingBox({3.0f, 4.0f, 3.0f},
                             cubeCurrentPosition,
                             {1.0f, 1.0f, 1.0f});
    cubeBox.position = cubeCurrentPosition;

    //ENEMIES
    //temporary model lol
    //EnemyPrototype.initModel("models/ufo.obj",
                             //_model::CUSTOM);
    EnemyPrototype.initModel("images/target/target.jpg",
                             "models/ufo.obj",
                             _model::CUSTOM);
    EnemyPrototype.scale = {0.15f, 0.15f, 0.15f};
    EnemyPrototype.addBoundingBox({1.0f, 1.0f, 1.0f},
                                 EnemyPrototype.position,
                                 EnemyPrototype.scale);
    enemyMoveTimer->reset();
    spawnInitEnemies();

    //ARENA AREA
    const float arenaWidth = 30.0f;
    const float arenaHeight = 30.0f;
    const float arenaDepth = 30.0f;

    //Set dimensions for arena
    arena.attachLoader(textureLoader);
    arena.configure(arenaWidth, arenaHeight, arenaDepth);

    arena.setMidlineEnabled(true);

    arenaCenter = {0.0f, arenaHeight * 0.5f, 0.0f};

    //Initialize bounds
    arenaHitBox.initBoundingBox({10.0f, 10.0f, 10.0f},
                                 {-12.0f, 0.5f, -12},
                                 {1.0f, 1.0f, 1.0f});
    arenaMidlineHitBox.initBoundingBox({10.0f, 10.0f, 0.5f},
                                        {-12.0f, 0.5f, -12},
                                        {1.0f, 1.0f, 1.0f});
    char* floorTexture = "images/arena/main_court.png";
    char* wallTexture = "images/arena/wall.jpg";
    char* ceiingTexture = "images/arena/plaster.jpg";
    arena.loadTheme({floorTexture, wallTexture, ceiingTexture});

     // SOUNDS INIT //
    _sounds::initSoundEngine();
    soundEngine->playMusic("sounds/level00_music.mp3", musicVolume);
    soundEngine->playSounds("sounds/level_transition.mp3", false, transitionLevelVolume, 0.8f);
}

//Delete and unload the assets after closing
void _level02::unloadAssets()
{
    player.reset();
    characterCam.reset();
    balls.clear();
    enemies.clear();
}

void _level02::reset()
{
    player->playerResetLife(5);
    lastThrownTime = 0.0f;

    //Ball reset
    ballPower = 0.0f;
    if(ballTimer) ballTimer->reset();
    if(ballThrowTimer) ballThrowTimer->reset();

    //Reset cube
    cubeTimer = 0.0;
    cubeDelay = 2.0;
    currentState = Cubestate::Waiting;
    cubeBox.position = cubeCurrentPosition;
    cubeModel.position = cubeCurrentPosition;

    //clear vector arrays
    balls.clear();
    enemies.clear();

    //Reset timers
    ballDeleteTimer->reset();
    stunnedTimer->reset();
    enemyMoveTimer->reset();
    spawnInitEnemies();

    //Reset camera at the right spot
    characterCam->des = player->position;
    characterCam->eye = { player->position.x,
                          player->position.y + 3.5f,
                          player->position.z + 10.0f };

    // reuse your clamp logic
    vec3 eyeLocal = arena.toLocal({
        characterCam->eye.x,
        characterCam->eye.y,
        characterCam->eye.z
    });
    eyeLocal = arena.clampLocal(eyeLocal, 0.0f);
    vec3 eyeWorld = arena.toWorld(eyeLocal);
    characterCam->eye.x = eyeWorld.x;
    characterCam->eye.y = eyeWorld.y;
    characterCam->eye.z = eyeWorld.z;
}

//Update every time in loop as needed
void _level02::update(double dt)
{
    //Update handlers
    updateCubeBlockage(dt);
    updatePlayer(dt);
    updateBalls(dt);
    updateEnemies(dt);

    //Collision with objects and when ball hits an object/model
    ballHitsEnemies();
    playerHitCube();
    ballHitsCube();
    enemyBallHitPlayer();
    playerHitEnemy();

}

//Render the items for the game
void _level02::render(const RenderFlags& flags)
{
    //Helps pivot the arena
    float arenaYaw = 90.0f;
    characterCam->setUpCamera();
    glEnable(GL_LIGHTING);

    //Render the arena
    glPushMatrix();
      arena.render();
    glPopMatrix();
      arenaMidlineHitBox.displayBoundingBox();
      arenaHitBox.displayBoundingBox();

    //Render the player and cube
    glPushMatrix();
      player->render();
    glPopMatrix();
     // playerHitBox->displayBoundingBox();

    glPushMatrix();
      cubeModel.enabled  = true;
      glDisable(GL_LIGHTING);      // ignore lighting for now
      glDisable(GL_TEXTURE_2D);    // ignore textures for now
        cubeModel.drawModel();
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_LIGHTING);
    glPopMatrix();
     // cubeBox.displayBoundingBox();

    //spawn balls and enemies
    for(auto& b : balls)
    {
        if(!b.active) continue;
        glPushMatrix();
          glTranslatef(b.position.x, b.position.y, b.position.z);
          b.model->drawModel();
        glPopMatrix();
      //    b.hitBox.displayBoundingBox();

    }

    for(auto& e : enemies)
    {
        if(!e.alive) continue;
        glPushMatrix();
          glTranslatef(e.position.x, e.position.y, e.position.z);
          e.model->drawModel();
        glPopMatrix();
       //   e.enemyBox.displayBoundingBox();

    }
}

//Handles
void _level02::applyCamera()
{
    //Set camera positions whenever moved by the player
    characterCam->des = player->position;
    characterCam->eye = {player->position.x,
                         player->position.y + 3.5f,
                         player->position.z + 10.0f};

    //CLAMPING SECTION (keep camera inside)
    //Use Arena functions to clamp it
    vec3 eyeLocal = arena.toLocal({characterCam->eye.x,
                                    characterCam->eye.y,
                                    characterCam->eye.z});
    eyeLocal = arena.clampLocal(eyeLocal, 0.0f);

    vec3 eyeWorld = arena.toWorld(eyeLocal);
    characterCam->eye.x = eyeWorld.x;
    characterCam->eye.y = eyeWorld.y;
    characterCam->eye.z = eyeWorld.z;

}

void _level02::handleKey(UINT uMsg, WPARAM wParam)
{

}

// ==================== HELPER FUNCTIONS ===========================

//Creates the balls for the vector
void _level02::createBall()
{
    Ball newBall;

    newBall.model = &ballPrototype;
    newBall.position = {player->position.x,
                        player->position.y - 0.5f,
                        player->position.z};

    vec3f newDim = {1.0f, 1.0f, 1.0f};
    vec3f newScale = {1.0f, 1.0f, 1.0f};
    vec3f newPos = newBall.position;

    newBall.hitBox.initBoundingBox(newDim, newPos, newScale);

    newBall.velocity = {0.0f, 1.0f, -9.0f};
    newBall.spawnTime = ballDeleteTimer->getTicks();
    newBall.active = true;
    newBall.fromEnemy = false;

    balls.push_back(newBall);

}

//spawns the enemies and incorporates movement and balls for it
void _level02::spawnInitEnemies()
{
    float speed = 0.5f;

    //Set Enemies positions, hit boxes, etc.
    vector<vec3f> spawnPositions = {{5.0f, player->position.y, -6.0f},
                                    {15.0f, player->position.y, -14.0f},
                                    {2.0f, player->position.y, -10.0f},

                                    {8.0f, player->position.y, -6.0f},
                                    {10.0f, player->position.y, -1.0f},
                                    {7.0f, player->position.y, -5.0f},

                                    {10.0f, player->position.y, -6.0f},
                                    {6.0f, player->position.y, -12.0f},
                                    {4.0f, player->position.y, -14.0f}};

    enemies.clear();
    for(const auto& pos : spawnPositions)
    {

        Enemy spawnedEnemies;
        spawnedEnemies.model = &EnemyPrototype;
        spawnedEnemies.position = pos;
        spawnedEnemies.enemyLife = 3;

        //Hit box
        vec3f newDim = {1.0f, 1.0f, 1.0f};
        vec3f newScale = {1.0f, 1.0f, 1.0f};
        spawnedEnemies.enemyBox.initBoundingBox(newDim, pos, newScale);

        float angle = static_cast<float>(rand()) / RAND_MAX * 2.0 * PI;
        spawnedEnemies.faceForward = {cos(angle),
                                      0.0f,
                                      sin(angle)};

        spawnedEnemies.velocity = { spawnedEnemies.faceForward.x * speed,
                                    0.0f,
                                    spawnedEnemies.faceForward.z * speed};

        spawnedEnemies.yaw = atan2(spawnedEnemies.faceForward.x,
                                   spawnedEnemies.faceForward.z);

        //Movement for stunned/move
        spawnedEnemies.stunned = false;
        spawnedEnemies.stunTimer = 0;
        spawnedEnemies.alive = true;

        // Get the next moved change
        double nowMove = enemyMoveTimer->getTicks();
        double intervalsMove = 500.0 + (rand()/ (double)RAND_MAX * 1500.0);
        spawnedEnemies.nextDirChange = nowMove + intervalsMove;

        double shootInterval = 1000.0 + (rand() / (double)RAND_MAX * 2000.0);
        spawnedEnemies.nextShootTime = nowMove + shootInterval;
        enemies.push_back(spawnedEnemies);
    }
}

//Creates new balls for enemies to use
void _level02::createEnemyBall(const Enemy& e)
{
    Ball newBall;
    newBall.model = &ballPrototype;

    //Create the positions for the enemeies balls
    newBall.position = {e.position.x,
                        e.position.y,
                        e.position.z};

    vec3f newDim = {1.0f, 1.0f, 1.0f};
    vec3f newScale = {1.0f, 1.0f, 1.0f};
    vec3f newPos = newBall.position;

    //Create the bounding box for the new balls.
    newBall.hitBox.initBoundingBox(newDim, newPos, newScale);

    //Get the direction from the enemies to the player
    vec3f direction;
    direction.x = player->position.x - e.position.x;
    direction.y = player->position.y - e.position.y;
    direction.z = player->position.z - e.position.z;

    float lenSq = direction.x*direction.x + direction.y*direction.y + direction.z*direction.z;
    if(lenSq < 1e-6f)
    {
        direction = {0.0f, 0.0f, 0.0f};
    }
    else
    {
        float invLen = 1.0f / sqrtf(lenSq);
        direction.x *= invLen;
        direction.y *= invLen;
        direction.z *= invLen;
    }

    float speed = 5.0f;
    newBall.velocity = {direction.x * speed,
                        direction.y * speed,
                        direction.z * speed};
    newBall.spawnTime = ballDeleteTimer->getTicks();
    newBall.active = true;
    newBall.fromEnemy = true;

    balls.push_back(newBall);
}

void _level02::updateEnemies(double dt)
{
    float speed = 0.5f;
    double moveNow = enemyMoveTimer->getTicks();

    for(size_t i = 0; i < enemies.size(); i++)
    {
        if(!enemies[i].alive) continue;

        if(enemies[i].stunned)
        {
            enemies[i].stunTimer += dt;
            if(enemies[i].stunTimer > 1.0f)
            {
                enemies[i].stunned = false;
                enemies[i].stunTimer = 0.0;
                continue;
            }
        }

        //Use timer to get movement directions
        if(moveNow >= enemies[i].nextDirChange)
        {
            float angle = static_cast<float>(rand()) / RAND_MAX * 2.0 * PI;
            enemies[i].faceForward = {cos(angle),
                                      0.0f,
                                      sin(angle)};

            enemies[i].velocity = { enemies[i].faceForward.x * speed,
                                    0.0f,
                                    enemies[i].faceForward.z * speed};

            enemies[i].yaw = atan2(enemies[i].faceForward.x,
                                   enemies[i].faceForward.z);

            //Update the movement/time change
            double intervalMs = 500.0 + (rand() / (double)RAND_MAX) * 1500.0;
            enemies[i].nextDirChange = moveNow + intervalMs;
        }

        if(moveNow >= enemies[i].nextShootTime)
        {
            createEnemyBall(enemies[i]);
            double shootIntervals = 1000.0 + (rand() / (double)RAND_MAX * 2000.0);
            enemies[i].nextShootTime = moveNow + shootIntervals;
        }

        // Move directly toward the player
        vec3f prevPos = enemies[i].position;

        vec3f dir;
        dir.x = player->position.x - enemies[i].position.x;
        dir.y = 0.0f;
        dir.z = player->position.z - enemies[i].position.z;

        // Normalize
        float len = sqrtf(dir.x * dir.x + dir.z * dir.z);
        if(len > 0.0001f)
        {
            dir.x /= len;
            dir.z /= len;
        }

        float chaseSpeed = 1.5f;

        enemies[i].position.x += dir.x * chaseSpeed * static_cast<float>(dt);
        enemies[i].position.z += dir.z * chaseSpeed * static_cast<float>(dt);

        // update hitbox
        enemies[i].enemyBox.position = enemies[i].position;

        // keep enemy behind midline
        if(enemies[i].enemyBox.isColliding(arenaMidlineHitBox))
        {
            enemies[i].position = prevPos;
            enemies[i].enemyBox.position = prevPos;
        }
    }
}
void _level02::updateBalls(double dt)
{
    float gravity = -9.8f;
    for(size_t i = 0; i < balls.size(); i++)
    {
        //Ball new positions
        if(!balls[i].active) continue;
        balls[i].velocity.y += gravity * dt;
        balls[i].position.x += balls[i].velocity.x * dt;
        balls[i].position.y += balls[i].velocity.y * dt;
        balls[i].position.z += balls[i].velocity.z * dt;
        balls[i].hitBox.position = balls[i].position;

        //Make the ball disappear
        currentBallTime = ballDeleteTimer->getTicks();
        if(currentBallTime - balls[i].spawnTime > 2000)
        {
            balls[i].active = false;
        }
    }
}

void _level02::updateCubeBlockage(double dt)
{
    cubeTimer += dt;

    switch(currentState)
    {
    case Cubestate::Waiting:
        if(cubeTimer >= cubeDelay)
        {
            currentState = Cubestate::Moving;

            vec3f getCurrentPlayerPos;
            getCurrentPlayerPos.x = player->position.x;
            getCurrentPlayerPos.y = player->position.y;
            getCurrentPlayerPos.z = player->position.z - 5.0f;
            cubeEndPosition = getCurrentPlayerPos;

            //Get the velocity
            float speed = 5.0f;
            vec3f direction;
            direction.x = cubeEndPosition.x - cubeCurrentPosition.x;
            direction.y = cubeEndPosition.y - cubeCurrentPosition.y;
            direction.z = cubeEndPosition.z - cubeCurrentPosition.z;

            //normalize sections
            float normDirection = sqrtf(direction.x*direction.x +
                                        direction.y*direction.y +
                                        direction.z * direction.z);
            if(normDirection > 1e-6)
            {
                direction.x /= normDirection;
                direction.y /= normDirection;
                direction.z /= normDirection;
            }

            //Set velocity
            cubeVelocity.x = direction.x * speed;
            cubeVelocity.y = direction.y * speed;
            cubeVelocity.z = direction.z * speed;
        }
        break;

    case Cubestate::Moving:

        //Set velocity
        cubeCurrentPosition.x += cubeVelocity.x * dt;
        cubeCurrentPosition.y += cubeVelocity.y * dt;
        cubeCurrentPosition.z += cubeVelocity.z * dt;

        cubeBox.position = cubeCurrentPosition;

        //Stop when the cube reaches its target in front of the player
        float dx = cubeEndPosition.x - cubeCurrentPosition.x;
        float dy = cubeEndPosition.y - cubeCurrentPosition.y;
        float dz = cubeEndPosition.z - cubeCurrentPosition.z;
        float distSq = dx*dx + dy*dy + dz*dz;

        if(distSq <= 0.4f*0.4f)
        {
            cubeCurrentPosition = cubeEndPosition;
            cubeBox.position = cubeEndPosition;
            currentState = Cubestate::Waiting;
            cubeDelay = 2.0;
            cubeTimer = 0.0;
        }

        break;
    }

    cubeBox.position = cubeCurrentPosition;
    cubeModel.position = cubeCurrentPosition;
}

void _level02::updatePlayer(double dt)
{
    float speed = 3.0f;
    float distance = speed * (float) dt;

    vec3 prevPos = player->position;

    //USER input
    if(GetAsyncKeyState(65) & 0x8000) player->position.x -= distance; //A
    if(GetAsyncKeyState(68) & 0x8000) player->position.x += distance; //D
    if(GetAsyncKeyState(87) & 0x8000) player->position.z -= distance; //W
    if(GetAsyncKeyState(83) & 0x8000) player->position.z += distance; //S

    //Check for collisions
    playerHitBox->position.x = player->position.x;
    playerHitBox->position.y = player->position.y;
    playerHitBox->position.z = player->position.z;

    //Mid line collision
    if(playerHitBox->checkCollision(arenaMidlineHitBox))
    {
        cout << "Hit midline" << endl;
        player->position = prevPos;
        playerHitBox->position.x = prevPos.x;
        playerHitBox->position.y = prevPos.y;
        playerHitBox->position.z = prevPos.z;
    }


    //Throw mechanics
    const bool leftMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    if(leftMouse)
    {
        if(ballPower > 0.0f && ballTimer->getTicks() >= 500)
        {
            createBall();
            ballTimer->reset();
        }

        ballPower = 0.0f;
        ballThrowTimer->reset();
    }

    //Clamp inside of the box
    vec3 playerLocal = arena.toLocal({player->position.x,
                                       player->position.y,
                                       player->position.z});
    playerLocal = arena.clampLocal(playerLocal, player->radius);
    vec3 playerWorld = arena.toWorld(playerLocal);

    //Player position clamp
    player->position.x = playerWorld.x;
    player->position.y = playerWorld.y;
    player->position.z = playerWorld.z;

    //player hit box clamp
    playerHitBox->position.x = player->position.x;
    playerHitBox->position.y = player->position.y;
    playerHitBox->position.z = player->position.z;

}

void _level02::ballHitsEnemies()
{
    for(size_t i = 0; i < balls.size(); i++)
    {
        //Skip inactive balls
        if(!balls[i].active) continue;
        if(balls[i].fromEnemy) continue; //<- makes sure it effects player not enemies
        for(size_t j = 0; j < enemies.size(); j++)
        {
            if(enemies[j].alive && !enemies[j].stunned)
            {
                bool hasCollided = balls[i].hitBox.isColliding(enemies[j].enemyBox);
                if(hasCollided)
                {
                    cout << "Ball hit Enemy" << endl;

                    enemies[j].stunned = true;
                    enemies[j].stunTimer = 0;
                    balls[i].active = false;
                    if(enemies[j].enemyLife > 0)
                    {
                        enemies[j].enemyLife--;
                    }

                    if(enemies[j].enemyLife == 0)
                    {
                        enemies.erase(enemies.begin() + 1);
                        j--;
                        continue;
                    }
                }


            }
        }
    }
}

void _level02::ballHitsCube()
{
    for(size_t i = 0; i < balls.size(); i++)
    {
        bool hasCollided = balls[i].hitBox.isColliding(cubeBox);
        if(!balls[i].fromEnemy && hasCollided)
        {
            cout << "Ball touched Cube" << endl;
            balls[i].active = false;
        }
    }
}

void _level02::playerHitCube()
{
    bool playerCollided = playerHitBox->isColliding(cubeBox);
    if(playerCollided)
    {
        cout << "Player touched Cube" << endl;
        if(player->life > 0)
        {
            player->life--;
            cout << "Player life #: " << player->life << endl;
        }

        if(player->life == 0)
        {
            cout << "Game Over!" << endl;
            if(requestGameOver_)
            {
                requestGameOver_();
            }
            return;
        }

    }
}

void _level02:: enemyBallHitPlayer()
{
    for(size_t i = 0; i < balls.size(); i++)
    {
        if(!balls[i].active) continue;    //Skip inactive balls
        if(!balls[i].fromEnemy) continue; //<- makes sure it effects player not enemies

        //Check the hitboxs
        if(balls[i].hitBox.isColliding(*playerHitBox))
        {
            cout << "Ball hit player" << endl;
            balls[i].active = false;
            player->life--;
        }

        if(player->life == 0)
        {
            cout << "Game Over!" << endl;
            if(requestGameOver_)
            {
                requestGameOver_();
            }
            return;
        }
    }
}
void _level02::playerHitEnemy()
{
     for (auto& e : enemies)
    {
        if(!e.alive) continue;

        if(playerHitBox->isColliding(e.enemyBox))
        {
            cout << "Player touched Enemy!" << endl;

            if(player->life > 0)
                player->life--;

            if(player->life == 0)
            {
                cout << "Game Over!" << endl;
                if(requestGameOver_)
                    requestGameOver_();
            }
            return;
        }
    }
}

