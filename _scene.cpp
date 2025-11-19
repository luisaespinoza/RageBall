#include "_scene.h"

_scene::_scene()
{
    //ctor
}

_scene::~_scene()
{
    //dtor
}

void _scene::reSizeScene(int width, int height)
{
    float aspectRatio = (float)width/(float)height;// keep track of the ratio
    glViewport(0,0,width,height); // adjust my viewport

    glMatrixMode(GL_PROJECTION);  // To setup ptrojection
    glLoadIdentity();             // calling identity matrix
    gluPerspective(45, aspectRatio,0.1,1000.0); // setting perspective projection

    this->width = GetSystemMetrics(SM_CXSCREEN);
    this->height= GetSystemMetrics(SM_CYSCREEN);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();             // calling identity matrix

    W = width; H = height;

    pause->layout(W, H);

}

//Draws the texture for the entire window
void _scene::drawFullscreenTex(GLuint tex, int W, int H)
{
    glBindTexture(GL_TEXTURE_2D, tex);

    //Does the full screen quads on image
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);  //bottom left
      glVertex2i(0, 0);
      glTexCoord2f(1.0f, 1.0f); //bottom right
      glVertex2i(W, 0);
      glTexCoord2f(1.0f, 0.0f); //top right
      glVertex2i(W, H);
      glTexCoord2f(0.0f, 0.0f); //top left
      glVertex2i(0, H);
    glEnd();

    //Enables textures and avoids texture bleed.
    glBindTexture(GL_TEXTURE_2D, 0);
}


//helps to switch from 3D to 2D to show title screen

void _scene::orthoStart()
{
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); //saves 3D state
      glLoadIdentity();
      gluOrtho2D(0,W, 0, H); //2D view
      glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); //saves camera state
    glLoadIdentity();
}

void _scene::orthoEnd()
{
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

void _scene::initGL()
{
    glShadeModel(GL_SMOOTH); // to handle GPU shaders
    glClearColor(0.0f,0.0f,0.0f,0.0f); // black background color
    glClearDepth(1.0f);         //depth test for layers
    glEnable(GL_DEPTH_TEST);    //activate depth test
    glDepthFunc(GL_LEQUAL);     // depth function type

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    myLight->setLight(GL_LIGHT0);    //Light
    fallingMdl->modelColor("images/tex.jpg");

    //skybox
    mySkyBox->skyBoxInit();
    mySkyBox->tex[0] = mySkyBox->textures->loadTexture("images/front.jpg");
    mySkyBox->tex[1] = mySkyBox->textures->loadTexture("images/back.jpg");
    mySkyBox->tex[2] = mySkyBox->textures->loadTexture("images/top.jpg");
    mySkyBox->tex[3] = mySkyBox->textures->loadTexture("images/bottom.jpg");
    mySkyBox->tex[4] = mySkyBox->textures->loadTexture("images/right.jpg");
    mySkyBox->tex[5] = mySkyBox->textures->loadTexture("images/left.jpg");

    texLandingPage = myTexture->loadTexture("images/landing.jpg");
    texMenu = myTexture->loadTexture("images/menu.jpg");
    texHelp = myTexture->loadTexture("images/help.jpg");

    //Connects the timer and the model.
    static _timer simTimer;
    fallingObj->myTime = &simTimer;
    fallingObj->myFallingMdl = fallingMdl;
    randomNum->model = fallingMdl;
    randomNum->generateRandom();
    fallingMdl->draw = true;
    //camera
    myCam->camInit();
    myCam->step = 0.25f;

}

void _scene::drawScene()
{
    //LANDING
    if(currentStage == LANDING)
    {
        orthoStart();
        drawFullscreenTex(texLandingPage, W, H);
        landing->draw();
        orthoEnd();
        return;
    }

    //MENU
    if(currentStage == MENU)
    {
        orthoStart();
        drawFullscreenTex(texMenu, W, H);
        menuScreen->draw();
        orthoEnd();
        return;
    }

    if(currentStage == HELP_MENU)
    {
        orthoStart();
        drawFullscreenTex(texHelp, W, H);
        help->draw();
        orthoEnd();
        return;
    }

    if(currentStage == PAUSE_GAME)
    {
        orthoStart();
        pause->draw();
        orthoEnd();
        return;
    }

    if(currentStage == START_GAME)
    {
        ShowCursor(false);  //Turn off cursor specifically here

    //============ GAME STAGE ==============
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);//clear bits in each iteration
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();             // calling identity matrix

    //get delta time per frame
    static _timer frameTimer;
    static bool firstFrame = true;

    if(firstFrame)
    {
        frameTimer.reset();
        firstFrame = false;
    }

    double dt = (double)frameTimer.getTicks()/CLOCKS_PER_SEC;
    frameTimer.reset();
    if(dt > 0.05) dt = 0.05;

    //Eyes/Inputs/Collision checks
    myCam->lastSafePosition = myCam->eye;

    myInputs->continuous(myCam, dt); //input for WASD

    //player collision check
    vec3 playerPos = myCam->eye;
    vec3 skyboxCenter = mySkyBox->boxSize;
    bool inside = (playerPos.x > -skyboxCenter.x && playerPos.x < skyboxCenter.x) &&
                  (playerPos.y > -skyboxCenter.y && playerPos.y < skyboxCenter.y) &&
                  (playerPos.z > -skyboxCenter.z && playerPos.z < skyboxCenter.z);

    //If the player is outside of box go back to the last position inside.
    if(!inside)
    {
        myCam->eye = myCam->lastSafePosition;
        myCam->camUpdateFPS();
    }

    myCam->setUpCamera();  //camera Mouse movements
    mySkyBox->drawSkyBox(); //Skybox drawn
    glDisable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Draw floor
    glPushMatrix();
    glScalef(500, 300, 1);
    ground->drawParallax(mySkyBox->boxSize.x, mySkyBox->boxSize.y);
    glPopMatrix();

    //bullets
    glPushMatrix();
        for(int i = 0; i <10; i+=1){
            if(bullet[i].live){
                bullet[i].bulletActions();

                if(fallingMdl->checkCollison(&bullet[i])){
                        cout << "Check True" << endl;
                    bullet[i].live = false;
                    bullet[i].actionTrigger = _bullets::READY; //_bullets::READY Makes it clear it belongs to this class.
                    fallingObj->bulletHitObject = true;
                    continue;
                }

            }
            bullet[i].drawBullet();

        }
    glPopMatrix();

    fallingObj->updatePhysics();

    if(!fallingMdl->draw)
    {
        //reset the state of the objects
        fallingObj->velocityY = 0.0f;
        fallingObj->bulletHitObject = false;
        fallingObj->objectHitGround = false;

        randomNum->generateRandom();
    }

    fallingMdl->drawModel();

    orthoStart();
    glColor3f(1, 1, 1);
    glRasterPos2i(W/2 - 5, H/2 - 8);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '+');
    orthoEnd();

    }

}


void _scene::mouseMapping(int x, int y)
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




int _scene::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_KEYDOWN:
            myInputs->wParam = wParam;
            if (wParam == VK_ESCAPE && currentStage == HELP_MENU)
            {
                currentStage = MENU;
            }
            else if(wParam == VK_ESCAPE && currentStage == START_GAME)
            {
                currentStage = PAUSE_GAME;
            }
            else if(wParam == VK_RETURN && currentStage == PAUSE_GAME)
            {
                ::exit(0);
                return 0;
            }
            else if(wParam == VK_ESCAPE && currentStage == PAUSE_GAME)
            {
                currentStage = START_GAME;
            }
        break;

        case WM_KEYUP:
        break;

        case WM_LBUTTONDOWN:
            {
                int mx = LOWORD(lParam), my = HIWORD(lParam);
                int myInv = H - my; //Helps to match the windows mouse Y to openGl's Y

                if(currentStage == LANDING)
                {
                    if(landing->hit(landing->btnEnter, mx, myInv))
                    {
                        currentStage = MENU;
                        return 0;
                    }
                    return 0; //if the play doesn't click the button and on somewhere else.
                }
                if(currentStage == MENU)
                {
                    if(menuScreen->hit(menuScreen->btnStart, mx, myInv))
                    {
                        currentStage = START_GAME;
                        return 0;
                    }
                    if(menuScreen->hit(menuScreen->btnHelp, mx, myInv)){
                        currentStage = HELP_MENU;
                        return 0;
                    }
                    if(menuScreen->hit(menuScreen->btnExit, mx, myInv))
                    {
                        ::exit(0);
                        return 0;
                    }
                    return 0;
                }
                if(currentStage == HELP_MENU)
                {
                    if(help->hit(help->btnBack, mx, myInv))
                    {
                        currentStage = MENU;
                        return 0;
                    }
                    return 0;
                }

                if(currentStage == START_GAME)
                {
                mouseMapping(LOWORD(lParam), HIWORD(lParam));
                clickCnt = (clickCnt +1) %10 ;
                 //bullet[clickCnt].src.x = mdl3D->pos.x;
                 //bullet[clickCnt].src.y = mdl3D->pos.y;
                 //bullet[clickCnt].src.z = mdl3D->pos.z;

                //Muzzle bullet shots uses the same Ideas from camera
                vec3 bulletForward = {myCam->des.x - myCam->eye.x,
                                      myCam->des.y - myCam->eye.y,
                                      myCam->des.z - myCam->eye.z};
                myCam->normalize(bulletForward);

                vec3 bulletRight = {bulletForward.y*myCam->up.z - bulletForward.z*myCam->up.y,
                                    bulletForward.z*myCam->up.x - bulletForward.x*myCam->up.z,
                                    bulletForward.x*myCam->up.y - bulletForward.y*myCam->up.x};
                myCam->normalize(bulletRight);

                //vec3 barrel is where the barrel is (E.I. to the bottom right) and updates that using the camera
                //coordinates like how camera does.
                float offFront = 0.6f; //Front camera
                float offRight = 0.8f; //Goes right
                float offUp = -0.5f; //goes slightly down

                //Each part axis has to move to the given offsets.
                //Example when camera moves the barrel follows with the two vec3s above.
                //Offset is added to make it feel like shooting.
                //Offset can be tweaked.
                vec3 barrel = {
                    myCam->eye.x + bulletForward.x*offFront + bulletRight.x*offRight + myCam->up.x * offUp,
                    myCam->eye.y + bulletForward.y*offFront + bulletRight.y*offRight + myCam->up.y * offUp,
                    myCam->eye.z + bulletForward.z*offFront + bulletRight.z*offRight + myCam->up.z * offUp
                };

                bullet[clickCnt].src = barrel;

                bullet[clickCnt].des.x = msX;
                bullet[clickCnt].des.y = msY;
                bullet[clickCnt].des.z = msZ;

                bullet[clickCnt].t=0;
                bullet[clickCnt].actionTrigger= bullet[clickCnt].SHOOT;
                bullet[clickCnt].live = true;
                }
                return 0;
            }

        break;

        case WM_RBUTTONDOWN:
        break;

        case WM_MBUTTONDOWN:
        break;

        case WM_LBUTTONUP:
        break;

        case WM_RBUTTONUP:
        break;

        case WM_MBUTTONUP:
        break;

        case WM_MOUSEMOVE:
            {
                if(currentStage == START_GAME)
                {
                    int mx = LOWORD(lParam);
                    int my = HIWORD(lParam);
                    myInputs->mouseMove(myCam, mx, my);
                }

            }

        break;

        case WM_MOUSEWHEEL:
        break;

        default:
        break;

    }
}
