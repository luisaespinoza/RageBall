#include "_level02.h"

_level02::_level02() {
    bleachers = new _model();
    front_back_walls = new _model();
    left_wall = new _model();
    right_wall = new _model();
    outer_court = new _model();
    main_court = new _model();

    player = new Player();
    textureLoader = new _textureLoader();
    characterCamera = new _camera();
    debugTimer = new _timerPlusPlus();
    physicsTimer = new _timerPlusPlus();
    keyTimer = new _timerPlusPlus();
    noclipCamera = new _camera();
    playerHitBox = new _boundingBox();
    ballTimer = new _timerPlusPlus();
    ballDeleteTimer = new _timerPlusPlus();
    directorTimer = new _timerPlusPlus();
    ballThrowTimer = new _timerPlusPlus();
}

_level02::~_level02() {
    unloadAssets();
}

void _level02::loadAssets() {
    currentStage = LEVEL02_INIT;
    // PLAYER INIT //
    player->vec_scale = {0.15f, 0.15f, 0.15f};
    player->initPlayer();
    player->position = {0.0f, 4.0f, 25.0f};
    playerHitBox->initBoundingBox({0.25f, 0.4f, 0.25f}, {player->position.x, player->position.y + 0.9f, player->position.z}, {1.0f, 1.0f, 1.0f});
    // ARENA MODELS INTI //
    // models
    main_court->initModel("images/arena/main_court.png","models/arena/main_court.obj", _model::CUSTOM);
    outer_court->initModel("images/arena/wood.jpg","models/arena/outer_court.obj", _model::CUSTOM);
    bleachers->initModel("images/arena/metal.jpg","models/arena/bleachers.obj", _model::CUSTOM);
    front_back_walls->initModel("images/arena/plaster.jpg","models/arena/front_back_walls.obj", _model::CUSTOM);
    left_wall->initModel("images/arena/plaster.jpg","models/arena/left_wall.obj", _model::CUSTOM);
    right_wall->initModel("images/arena/plaster.jpg","models/arena/right_wall.obj", _model::CUSTOM);
    // bounding boxes
    main_court->addBoundingBox({47.0f,1.0f,88.0f});
    outer_court->addBoundingBox({25.0f,12.0f,1.0f}, {0.0f, 6.0f, 0.0f}, outer_court->scale);     // mid-court divider
    outer_court->addBoundingBox({25.0f,12.0f,1.0f}, {0.0f, 6.0f, -25.0f}, outer_court->scale);   // back wall
    outer_court->addBoundingBox({25.0f,12.0f,1.0f}, {0.0f, 6.0f, 25.0f}, outer_court->scale);    // front wall
    outer_court->addBoundingBox({1.0f,12.0f,50.0f}, {-12.5f, 6.0f, 0.0f}, outer_court->scale);   // left wall
    outer_court->addBoundingBox({1.0f,12.0f,50.0f}, {12.5f, 6.0f, 0.0f}, outer_court->scale);    // right wall
    // BALLS INIT //
    ballPrototype = new _balls();
    ballPrototype->initModel("images/dodgeball.jpg", "models/dodge_ball.obj", _model::CUSTOM);
    ballPrototype->scale = {0.15f, 0.15f, 0.15f};
    ballPrototype->addBoundingBox({1.0f, 1.0f, 1.0f}, ballPrototype->position, ballPrototype->scale);
    // TARGETS INIT //
    targetPrototype = new _targets();
    targetPrototype->initModel("images/target/target.jpg", "models/target/target_main.obj", _model::CUSTOM);
    // CAMERA INIT//
    characterCamera->camInit();
    noclipCamera->camInit();
    noclipCamera->step = 0.07f;
    noclipCamera->rotAngle = {0.0f, 0.0f};
    noclipCamera->updateFPSCamera();
    // TIMER INIT //
    debugTimer->reset();
    physicsTimer->reset();
    keyTimer->reset();
    ballTimer->reset();
    ballDeleteTimer->reset();
    directorTimer->reset();
    ballThrowTimer->reset();
    // start game 
    currentStage = LEVEL02_PLAYING_0;
    updateDirector(true); // force update to spawn initial targets
}

void _level02::unloadAssets()
{
    // clean up dynamically allocated memory -- setting all pointers to nullptr is a bit excessive, but safe
    delete ballPrototype; ballPrototype = nullptr;
    delete targetPrototype; targetPrototype = nullptr;
    for (int i = 0; i < balls.size(); i++) {
        delete balls[i];
        balls[i] = nullptr;
    }
    balls.clear();
    for (int i = 0; i < targets.size(); i++) {
        delete targets[i];
        targets[i] = nullptr;
    }
    targets.clear();
    delete characterCamera; characterCamera = nullptr;
    delete noclipCamera; noclipCamera = nullptr;

    delete bleachers; bleachers = nullptr;
    delete front_back_walls; front_back_walls = nullptr;
    delete left_wall; left_wall = nullptr;
    delete right_wall; right_wall = nullptr;
    delete outer_court; outer_court = nullptr;
    delete main_court; main_court = nullptr;

    delete player; player = nullptr;
    delete playerHitBox; playerHitBox = nullptr;
    delete debugTimer; debugTimer = nullptr;
    delete physicsTimer; physicsTimer = nullptr;
    delete keyTimer; keyTimer = nullptr;
    delete ballTimer; ballTimer = nullptr;
    delete ballDeleteTimer; ballDeleteTimer = nullptr;
    delete textureLoader; textureLoader = nullptr;
    delete directorTimer; directorTimer = nullptr;
    delete ballThrowTimer; ballThrowTimer = nullptr;
}

void _level02::handleKey(UINT uMsg, WPARAM wParam) {
    if (currentStage == LEVEL02_INIT) {
        // Loading stage - skip input handling
        return;
    }
    switch (wParam) {
        case VK_OEM_4:
            if (keyTimer->getTicks() > 300) {
                cout << "Toggled Bounding Box Display: " << (showBoundingBoxes ? "OFF" : "ON") << endl;
                showBoundingBoxes = !showBoundingBoxes;
                keyTimer->reset();
            }
            break;
        case VK_OEM_6:
            if (keyTimer->getTicks() > 300) {
                cout << "Toggled Map Models Display: " << (showMapModels ? "OFF" : "ON") << endl;
                showMapModels = !showMapModels;
                keyTimer->reset();
            }
            break;
        case VK_OEM_5:
            if (keyTimer->getTicks() > 300) {
                cout << "Toggled Noclip Mode: " << (noclipEnabled ? "OFF" : "ON") << endl;
                noclipEnabled = !noclipEnabled;
                firstMouse = true; // reset for noclip mouse movement
                keyTimer->reset();
            }
            break;
        case VK_ADD:
            if (keyTimer->getTicks() > 300) {
                cout << "Spawning a target!" << endl;
                createTarget({0.0f, 2.0f, 0.0f}, 1.3f);
                keyTimer->reset();
            }
            break;
    }
}

int _level02::winMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (currentStage == LEVEL02_INIT) {
        // Loading stage - skip input handling
        return 0;
    }
    switch (uMsg) {
        case WM_MOUSEMOVE: {
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            windowWidth = clientRect.right - clientRect.left;
            windowHeight = clientRect.bottom - clientRect.top;

            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            
            if (firstMouse) {
                lastMouseX = x;
                lastMouseY = y;
                firstMouse = false; 
                return 0;
            }
            
            int deltaX = x - lastMouseX;
            int deltaY = y - lastMouseY;
            lastMouseX = x;
            lastMouseY = y;
            if (noclipEnabled) {
                const float sensitivity = 0.15f;
                noclipCamera->rotAngle.x -= deltaX * sensitivity;
                noclipCamera->rotAngle.y -= deltaY * sensitivity;
                
                if (noclipCamera->rotAngle.y > 89.0f)  noclipCamera->rotAngle.y = 89.0f;
                if (noclipCamera->rotAngle.y < -89.0f) noclipCamera->rotAngle.y = -89.0f;
                
                noclipCamera->updateFPSCamera();
            }
            return 0;
        }
    }
    return 0;
}

void _level02::applyCamera() {

}

void _level02::reset() {

}

void _level02::updateDirector(bool forceUpdate) {
    if (forceUpdate) {
        // force update -- dont check for level change conditions just change stage
        switch (currentStage) {
            case LEVEL02_INIT:
                break;
            case LEVEL02_PLAYING_0:
                // Spawn 5 static targets at random positions
                directorTimer->reset();
                for (int i = 0; i < 5; i++) {
                    vec3f pos = {RNG::getFloat(-10.0f, 10.0f), 2.0f, RNG::getFloat(-20.0f, 0.0f)};
                    createTarget(pos, 0.0f);
                }
                break;
            case LEVEL02_PLAYING_1:
                directorTimer->reset();
                for (int i = 0; i < 5; i++) {
                    vec3f pos = {RNG::getFloat(-10.0f, 10.0f), RNG::getFloat(1.0f,3.0f), RNG::getFloat(-20.0f, 0.0f)};
                    float speed = RNG::getFloat(0.5f, 1.5f);
                    int direction = RNG::getInt(0,1);
                    if (direction == 0) {
                        createTarget(pos, speed,-1);
                    } else {
                        createTarget(pos, speed,1);
                    }
                }
                break;
            case LEVEL02_COMPLETE:
                break;
        }
    } else {
        // passive director update -- check for level change conditions
        switch (currentStage) {
            case LEVEL02_INIT:
                break;
            case LEVEL02_PLAYING_0:
                if (targets.size() == 0) {
                    cout << "All targets cleared! Advancing to next stage." << endl;
                    currentStage = LEVEL02_PLAYING_1;
                    updateDirector(true);
                }
                break;
            case LEVEL02_PLAYING_1:
                break;
            case LEVEL02_COMPLETE:
                break;
        }
    }
}

void _level02::debugPrint() {
    cout << "Player Position: (" << player->position.x << ", " << player->position.y << ", " << player->position.z << ")" << endl;
    cout << "Mouse Position: (" << lastMouseX << ", " << lastMouseY << ")" << endl;
    cout << "Window Size: (" << windowWidth << " x " << windowHeight << ")" << endl;
    cout << "Ball Power: " << ballPower << endl;
}

void _level02::update(double dt) {
    if (currentStage == LEVEL02_INIT) {
        // Loading stage - skip updates
        return;
    }
    // INPUT HANDLING //
    float physicsDt = physicsTimer->getTicks()/1000.0f; // convert ms to s

    const bool w = (GetAsyncKeyState(KEY_W) & 0x8000) != 0;
    const bool s = (GetAsyncKeyState(KEY_S) & 0x8000) != 0;
    const bool a = (GetAsyncKeyState(KEY_A) & 0x8000) != 0;
    const bool d = (GetAsyncKeyState(KEY_D) & 0x8000) != 0;
    const bool space = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    const bool leftMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    float gravity = -9.81f; // gravity acceleration
    float moveSpeedCamera = 8.0f;
    float playerMoveSpeed = 8.0f;
    float ballSpeed = 0.5f;
    bool jumpImpulse = false;
    player->currentAnimation = IDLE;
    // COLLISION CHECKING //
    // we get a vector of ALL collisions, which affects controls
    vector<_boundingBox::collisionType> collisions = main_court->checkCollisionWith(*playerHitBox);
    vector<_boundingBox::collisionType> outerCollisions = outer_court->checkCollisionWith(*playerHitBox);
    collisions.insert(collisions.end(), outerCollisions.begin(), outerCollisions.end());
    if (noclipEnabled) {
        if (w) {noclipCamera->moveFPSForward(moveSpeedCamera*physicsDt);}
        if (a) {noclipCamera->moveFPSStrafe(moveSpeedCamera*physicsDt);}
        if (s) {noclipCamera->moveFPSForward(-moveSpeedCamera*physicsDt);}
        if (d) {noclipCamera->moveFPSStrafe(-moveSpeedCamera*physicsDt);}
        if (space) { noclipCamera->moveFPSUp(moveSpeedCamera*physicsDt); }
        if (shift) { noclipCamera->moveFPSUp(-moveSpeedCamera*physicsDt); }
    } else {
        if (w && std::find(collisions.begin(), collisions.end(), _boundingBox::POS_Z) == collisions.end()) { 
            player->position.z += -playerMoveSpeed * physicsDt;
            player->rotation.y = 180.0f;
            player->currentAnimation = WALK;
        }
        if (s && std::find(collisions.begin(), collisions.end(), _boundingBox::NEG_Z) == collisions.end()) { 
            player->position.z += playerMoveSpeed * physicsDt;
            player->rotation.y = 0.0f;
            player->currentAnimation = WALK;
        }
        if (a && std::find(collisions.begin(), collisions.end(), _boundingBox::POS_X) == collisions.end()) { 
            player->position.x += -playerMoveSpeed * physicsDt;
            player->rotation.y = 270.0f;
            player->currentAnimation = WALK;
        }
        if (d && std::find(collisions.begin(), collisions.end(), _boundingBox::NEG_X) == collisions.end()) { 
            player->position.x += playerMoveSpeed * physicsDt;
            player->rotation.y = 90.0f;
            player->currentAnimation = WALK;
        }
        if (space && std::find(collisions.begin(), collisions.end(), _boundingBox::NONE) == collisions.end()) {
            playerVelocity.y = 10.0f; // jump impulse
            jumpImpulse = true;
        }
        // Diagonal Movement Adjustments    
        if (w && a) {
            player->rotation.y = 225.0f;
        }
        if (w && d) {
            player->rotation.y = 135.0f;
        }
        if (s && a) {
            player->rotation.y = 315.0f;
        }
        if (s && d) {
            player->rotation.y = 45.0f;
        }
        if (leftMouse) {
            ballPower = ballThrowTimer->getTicks() / 1000.0f; // seconds held
        } else {
            if (ballPower > 0.0f) {
                if (ballTimer->getTicks() >= 500) {
                    createBall();
                    ballTimer->reset();
                } 
            }
            ballPower = 0.0f;
            ballThrowTimer->reset();
        }
    }
    // HANDLE GRAVITY //
    if (std::find(collisions.begin(), collisions.end(), _boundingBox::POS_Y) == collisions.end()) {
        playerAccel.y = gravity;
    } else {
        if (!jumpImpulse) {
            playerAccel.y = 0.0f;
            playerVelocity.y = 0.0f;
        }
    }
    // CAMERA HANDLING //
    characterCamera->des = player->position;
    characterCamera->eye = {player->position.x, player->position.y + 1.20f, player->position.z + 4.0f};
    // PHYSICS UPDATE //
    // Update velocity by acceleration
    playerVelocity.x += playerAccel.x * physicsDt; 
    playerVelocity.y += playerAccel.y * physicsDt; 
    playerVelocity.z += playerAccel.z * physicsDt;
    // Update position by velocity
    player->position.x += playerVelocity.x * physicsDt; 
    player->position.y += playerVelocity.y * physicsDt; 
    player->position.z += playerVelocity.z * physicsDt;
    // update player hitbox
    playerHitBox->position = {player->position.x, player->position.y, player->position.z};
    // update balls hitboxes
    // PHYSICS UPDATE - BALLS //
    for (int i = 0; i < balls.size(); i++) {
        vector<_boundingBox::collisionType> ballCollisions = main_court->checkCollisionWith(balls[i]->boundingBoxes[0]);
        bool movementImpulse = false; // if true prevents collision to allow for movement to adjust
        if (std::count(ballCollisions.begin(), ballCollisions.end(), _boundingBox::POS_Y) == 0) {
            // ball in air
            if (!movementImpulse) balls[i]->acceleration.y = gravity;
        } else {
            // bounce
            movementImpulse = true;
            balls[i]->velocity.y = 3.0f;
        }
        if (std::count(ballCollisions.begin(), ballCollisions.end(), _boundingBox::POS_X) > 0) {
            // ball on wall X+
            movementImpulse = true;
            balls[i]->velocity.x = ballSpeed;
        } 
        if (std::count(ballCollisions.begin(), ballCollisions.end(), _boundingBox::NEG_X) > 0) {
            // ball on wall X-
            movementImpulse = true;
            balls[i]->velocity.x = -ballSpeed;
        }
        if (std::count(ballCollisions.begin(), ballCollisions.end(), _boundingBox::POS_Z) > 0) {
            // ball on wall Z+
            movementImpulse = true;
            balls[i]->velocity.z = ballSpeed;
        } 
        if (std::count(ballCollisions.begin(), ballCollisions.end(), _boundingBox::NEG_Z) > 0) {
            // ball on wall Z-
            movementImpulse = true;
            balls[i]->velocity.z = -ballSpeed;
        }
        balls[i]->updateModel(physicsDt);
        // move bounding box
        balls[i]->boundingBoxes[0].position = {balls[i]->position.x, balls[i]->position.y, balls[i]->position.z};  
    }
    // DELETION - BALLS //
    for (int i = balls.size()-1; i >=0 ; i--) {   
        if (balls[i]->position.y < -15.0f) {
            delete balls[i];
            balls.erase(balls.begin() + i);
        } else if (ballDeleteTimer->getTicks() - balls[i]->spawnTime >= balls[i]->lifeLength * 1000.0f) {
            delete balls[i];
            balls.erase(balls.begin() + i);      
        } 
    }
    // TARGET UPDATES //
    for (int i = 0; i < targets.size(); i++) {
        // movement
        targets[i]->updateModel(physicsDt);
        // collison check
        for (int j = 0; j < balls.size(); j++) {
            if (targets[i]->isCollidingWith(balls[j]->boundingBoxes[0])) {
                targets[i]->targetHit();
                break;
            }
        }
        // move collison box
        targets[i]->boundingBoxes[0].position = targets[i]->position;
    }
    // TARGET DELETION //
    for (int i = targets.size()-1; i >=0 ; i--) {   
        if (targets[i]->state == DEAD) {
            delete targets[i];
            targets.erase(targets.begin() + i);
        }
    }
    physicsTimer->reset();
    // DIRECTOR HANDLING //
    if (directorTimer->getTicks() >= 5000) { // every 5 seconds
        updateDirector();
        directorTimer->reset();
    }
    // DEBUG PRINTING //
    if (debugTimer->getTicks() >= debugPrintInterval) {
        debugPrint();
        for (int i = 0; i < collisions.size(); ++i) {
            debugPrintCollisionInfo(collisions[i]);
        }
        debugTimer->reset();
    }
}

void _level02::render(const RenderFlags& flags) {
    if (currentStage == LEVEL02_INIT) {
        // Loading stage - skip input handling
        return;
    }
    if (noclipEnabled) {
        noclipCamera->setUpCamera(); // camera MUST be first!
    }
    else {
        characterCamera->setUpCamera();
    }
    if (showBoundingBoxes) {
        outer_court->displayBoundingBoxes();
        main_court->displayBoundingBoxes();
        playerHitBox->displayBoundingBox();
        for (int i = 0; i < balls.size(); i++) {
            balls[i]->displayBoundingBoxes();
        }
        for (int i = 0; i < targets.size(); i++) {
            targets[i]->displayBoundingBoxes();
        }
    }
    if (showMapModels) {
        player->drawPlayer();      
        main_court->drawModel();
        outer_court->drawModel();
        bleachers->drawModel();
        front_back_walls->drawModel();
        left_wall->drawModel();
        right_wall->drawModel();
        for (int i = 0; i < balls.size(); i++) {
            balls[i]->drawModel();
        }
        for (int i = 0; i < targets.size(); i++) {
            targets[i]->drawModel();
        }
    }
}

void _level02::debugPrintCollisionInfo(const _boundingBox::collisionType& collision) {
    switch (collision) {
        case _boundingBox::NONE:
            cout << "No Collision Detected." << endl;
            break;
        case _boundingBox::POS_X:
            cout << "Collision on Positive X Axis." << endl;
            break;
        case _boundingBox::NEG_X:
            cout << "Collision on Negative X Axis." << endl;
            break;
        case _boundingBox::POS_Y:
            cout << "Collision on Positive Y Axis." << endl;
            break;
        case _boundingBox::NEG_Y:
            cout << "Collision on Negative Y Axis." << endl;
            break;
        case _boundingBox::POS_Z:
            cout << "Collision on Positive Z Axis." << endl;
            break;
        case _boundingBox::NEG_Z:
            cout << "Collision on Negative Z Axis." << endl;
            break;
    }
}

void _level02::createBall() {
    float ballVelocity = 10.0f * ballPower; // temp
    // normalize mouse coords between -1 and 1
    // -1 = LEFT or BOTTOM, 1 = RIGHT or TOP
    float mouseXNorm = (2.0f * lastMouseX) / windowWidth - 1.0f;
    float mouseYNorm = 1.0f - (2.0f * lastMouseY) / windowHeight;
    
    _balls* newBall = new _balls(*ballPrototype); // copy prototype
    newBall->ballType = BALL_FRIENDLY;
    newBall->spawnTime = ballDeleteTimer->getTicks();
    newBall->position = {player->position.x, player->position.y + 1.0f, player->position.z};
    newBall->velocity.x = ballVelocity * mouseXNorm;
    //newBall->velocity.y = ballVelocity * mouseYNorm;
    newBall->velocity.y = ballVelocity * 0.25f; // slight arc
    newBall->velocity.z = -ballVelocity; // forward
    newBall->boundingBoxes[0].position = {newBall->position.x, newBall->position.y, newBall->position.z};
    balls.push_back(newBall);
}

void _level02::createTarget(vec3f position, float speed, int direction) {
    _targets* newTarget = new _targets(*targetPrototype); // copy prototype
    newTarget->scale = {0.3f, 0.3f, 0.3f};
    newTarget->position = position;
    newTarget->rotation.x = 90.0f;

    newTarget->speed = speed;
    newTarget->direction = 1; // start moving right
    newTarget->leftBound = -12.5f;
    newTarget->rightBound = 12.5f;
    newTarget->direction = direction;
    newTarget->addBoundingBox({4.5f, 4.5f, 0.3f});
    targets.push_back(newTarget);
}



