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
}

_level02::~_level02() {
    unloadAssets();
}

void _level02::loadAssets() {
    // PLAYER INIT //
    //player->init("models/megaman/tris.md2", "models/megaman/MegaMan.pcx", *textureLoader);
    //player->applyScale(0.005f);
    player->vec_scale = {0.15f, 0.15f, 0.15f};
    player->initPlayer();
    player->position = {0.0f, 1.0f, -2.0f};
    //player->radius   = 0.05f;
    playerHitBox->initBoundingBox({0.25f, 0.4f, 0.25f}, {player->position.x, player->position.y + 0.9f, player->position.z}, {1.0f, 1.0f, 1.0f});
    // ARENA MODELS INTI //
    // models
    main_court->initModel("images/arena/main_court.png","models/arena/main_court.obj", _model::CUSTOM);
    outer_court->initModel("images/arena/wall.jpg","models/arena/outer_court.obj", _model::CUSTOM);
    bleachers->initModel("images/arena/wall.jpg","models/arena/bleachers.obj", _model::CUSTOM);
    front_back_walls->initModel("images/arena/wall.jpg","models/arena/front_back_walls.obj", _model::CUSTOM);
    left_wall->initModel("images/arena/wall.jpg","models/arena/left_wall.obj", _model::CUSTOM);
    right_wall->initModel("images/arena/wall.jpg","models/arena/right_wall.obj", _model::CUSTOM);
    // bounding boxes
    main_court->addBoundingBox({47.0f,1.0f,88.0f});
    outer_court->addBoundingBox({25.0f,12.0f,1.0f}, {0.0f, 6.0f, -25.0f}, outer_court->scale);  // back wall
    outer_court->addBoundingBox({25.0f,12.0f,1.0f}, {0.0f, 6.0f, 25.0f}, outer_court->scale);   // front wall
    outer_court->addBoundingBox({1.0f,12.0f,50.0f}, {-12.5f, 6.0f, 0.0f}, outer_court->scale);   // left wall
    outer_court->addBoundingBox({1.0f,12.0f,50.0f}, {12.5f, 6.0f, 0.0f}, outer_court->scale);   // right wall
    // BALLS INIT //
    ballPrototype = new _model();
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
}

void _level02::unloadAssets()
{
    for (int i = 0; i < balls.size(); i++) {
        delete balls[i];
    }
    balls.clear();
    delete ballPrototype;
    delete characterCamera;
    delete noclipCamera;

    delete bleachers;
    delete front_back_walls;
    delete left_wall;
    delete right_wall;
    delete outer_court;
    delete main_court;

    delete player;
    delete playerHitBox;
    delete debugTimer;
    delete physicsTimer;
    delete keyTimer;
    delete ballTimer;
    delete ballDeleteTimer;
    delete textureLoader;
}

void _level02::handleKey(UINT uMsg, WPARAM wParam) {
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

void _level02::debugPrint() {
    cout << "Player Position: (" << player->position.x << ", " << player->position.y << ", " << player->position.z << ")" << endl;
    cout << "Mouse Position: (" << lastMouseX << ", " << lastMouseY << ")" << endl;
    cout << "Window Size: (" << windowWidth << " x " << windowHeight << ")" << endl;
}

void _level02::update(double dt) {
    // INPUT HANDLING //
    float physicsDt = physicsTimer->getTicks()/1000.0f; // convert ms to s

    const bool w = (GetAsyncKeyState(KEY_W) & 0x8000) != 0;
    const bool s = (GetAsyncKeyState(KEY_S) & 0x8000) != 0;
    const bool a = (GetAsyncKeyState(KEY_A) & 0x8000) != 0;
    const bool d = (GetAsyncKeyState(KEY_D) & 0x8000) != 0;
    const bool space = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

    float gravity = -9.81f; // gravity acceleration
    float moveSpeedCamera = 0.1f;
    float playerMoveSpeed = 0.03f;
    float ballSpeed = 0.5f;
    bool jumpImpulse = false;
    player->currentAnimation = IDLE;
    // COLLISION CHECKING //
    // we get a vector of ALL collisions, which affects controls
    vector<_boundingBox::collisionType> collisions = main_court->checkCollisionWith(*playerHitBox);
    vector<_boundingBox::collisionType> outerCollisions = outer_court->checkCollisionWith(*playerHitBox);
    collisions.insert(collisions.end(), outerCollisions.begin(), outerCollisions.end());
    if (noclipEnabled) {
        if (w) {noclipCamera->moveFPSForward(moveSpeedCamera);}
        if (a) {noclipCamera->moveFPSStrafe(moveSpeedCamera);}
        if (s) {noclipCamera->moveFPSForward(-moveSpeedCamera);}
        if (d) {noclipCamera->moveFPSStrafe(-moveSpeedCamera);}
        if (space) { noclipCamera->moveFPSUp(moveSpeedCamera); }
        if (shift) { noclipCamera->moveFPSUp(-moveSpeedCamera); }
    } else {
        if (w && std::find(collisions.begin(), collisions.end(), _boundingBox::POS_Z) == collisions.end()) { 
            player->position.z += -playerMoveSpeed;
            player->rotation.y = 180.0f;
            player->currentAnimation = WALK;
        }
        if (s && std::find(collisions.begin(), collisions.end(), _boundingBox::NEG_Z) == collisions.end()) { 
            player->position.z += playerMoveSpeed;
            player->rotation.y = 0.0f;
            player->currentAnimation = WALK;
        }
        if (a && std::find(collisions.begin(), collisions.end(), _boundingBox::POS_X) == collisions.end()) { 
            player->position.x += -playerMoveSpeed;
            player->rotation.y = 270.0f;
            player->currentAnimation = WALK;
        }
        if (d && std::find(collisions.begin(), collisions.end(), _boundingBox::NEG_X) == collisions.end()) { 
            player->position.x += playerMoveSpeed;
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
        if (shift) {
            if (ballTimer->getTicks() >= 500) { 
                createBall();
                ballTimer->reset();
            }    
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
        balls[i]->velocity.y += balls[i]->acceleration.y * physicsDt; 
        balls[i]->position.x += balls[i]->velocity.x * physicsDt; 
        balls[i]->position.y += balls[i]->velocity.y * physicsDt;
        balls[i]->position.z += balls[i]->velocity.z * physicsDt;
        balls[i]->boundingBoxes[0].position = {balls[i]->position.x, balls[i]->position.y, balls[i]->position.z};
        if (balls[i]->position.y < -15.0f) {
            delete balls[i];
            balls.erase(balls.begin() + i);
            i--; // adjust index after removal
        } else if (ballDeleteTimer->getTicks() - balls[i]->spawnTime >= ballDeleteTime) {
            delete balls[i];
            balls.erase(balls.begin() + i);
            i--; // adjust index after removal          
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
    float ballVelocity = 10.0f; // temp
    // normalize mouse coords between -1 and 1
    // -1 = LEFT or BOTTOM, 1 = RIGHT or TOP
    float mouseXNorm = (2.0f * lastMouseX) / windowWidth - 1.0f;
    float mouseYNorm = 1.0f - (2.0f * lastMouseY) / windowHeight;
    
    _model* newBall = new _model(*ballPrototype); // copy prototype
    newBall->modelId = modelIdList++;
    newBall->spawnTime = ballDeleteTimer->getTicks();
    newBall->position = {player->position.x, player->position.y + 1.0f, player->position.z};
    newBall->velocity.x = ballVelocity * mouseXNorm;
    newBall->velocity.y = ballVelocity * mouseYNorm;
    newBall->velocity.z = -ballVelocity; // forward
    newBall->boundingBoxes[0].position = {newBall->position.x, newBall->position.y, newBall->position.z};
    balls.push_back(newBall);
}

void _level02::createTarget(vec3f position, float speed) {
    _targets* newTarget = new _targets(*targetPrototype); // copy prototype
    newTarget->scale = {0.3f, 0.3f, 0.3f};
    newTarget->position = position;
    newTarget->rotation.x = 90.0f;

    newTarget->speed = speed;
    newTarget->direction = 1; // start moving right
    newTarget->leftBound = -12.5f;
    newTarget->rightBound = 12.5f;

    newTarget->addBoundingBox({4.5f, 4.5f, 0.3f});
    targets.push_back(newTarget);
}



