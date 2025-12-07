#include "_level02.h"

_level02::_level02() {
    player = new Player();
    roomModel = new _model();
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
    player->scale = {0.08f, 0.08f, 0.08f};
    player->initPlayer();
    player->position = {0.0f, 1.0f, -2.0f};
    //player->radius   = 0.05f;
    playerHitBox->initBoundingBox({0.25f, 0.4f, 0.25f}, {player->position.x, player->position.y + 0.9f, player->position.z}, {1.0f, 1.0f, 1.0f});
    // ROOM MODEL INIT //
    roomModel->initModel("images/back.jpg","models/arena.obj", _model::CUSTOM);
    roomModel->position = {0.0f, -1.0f, 0.0f};
    roomModel->scale    = {1.5f, 1.5f, 1.5f};
    roomModel->addBoundingBox({10.0f, 0.5f, 10.0f},{roomModel->position.x,roomModel->position.y+0.5f,roomModel->position.z},{roomModel->scale}); // floor
    roomModel->addBoundingBox({0.5f,5.5f,10.0f},{-7.75f,3.0f,0},roomModel->scale); // left wall
    roomModel->addBoundingBox({0.5f,5.5f,10.0f},{7.75f,3.0f,0},roomModel->scale);  // right wall
    roomModel->addBoundingBox({10.0f,5.5f,0.5f},{0,3.0f,-7.75f},roomModel->scale); // back wall
    roomModel->addBoundingBox({10.0f,5.5f,0.5f},{0,3.0f,7.75f},roomModel->scale);  // front wall
    // BALLS INIT //
    ballPrototype = new _model();
    ballPrototype->initModel("images/dodgeball.jpg", "models/dodge_ball.obj", _model::CUSTOM);
    ballPrototype->scale = {0.15f, 0.15f, 0.15f};
    ballPrototype->addBoundingBox({1.0f, 1.0f, 1.0f}, ballPrototype->position, ballPrototype->scale);
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
    delete roomModel;
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
    float playerMoveSpeed = 0.01f;
    float ballSpeed = 0.5f;
    bool jumpImpulse = false;
    player->currentAnimation = IDLE;
    // COLLISION CHECKING //
    // we get a vector of ALL collisions, which affects controls
    vector<_boundingBox::collisionType> collisions = roomModel->checkCollisionWith(*playerHitBox);
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
    characterCamera->eye = {player->position.x, player->position.y + 1.25f, player->position.z + 3.0f};
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
        vector<_boundingBox::collisionType> ballCollisions = roomModel->checkCollisionWith(balls[i]->boundingBoxes[0]);
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
        // TODO -- add ball removal
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
    if (showMapModels) {
        player->drawPlayer();      
        roomModel->drawModel();
        for (int i = 0; i < balls.size(); i++) {
            balls[i]->drawModel();
        }
    }
    if (showBoundingBoxes) {
        roomModel->displayBoundingBoxes();
        playerHitBox->displayBoundingBox();
        for (int i = 0; i < balls.size(); i++) {
            balls[i]->displayBoundingBoxes();
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

