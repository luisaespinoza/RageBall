#include "_inputs.h"

_inputs::_inputs()
{
    //ctor
    isRotationActive = false;
    isTranslateActive = false;
}

_inputs::~_inputs()
{
    //dtor
}
void _inputs::keyPressed(_model* mdl)
{

}

void _inputs::keyPressed(_parallax* prlx)
{

     switch(wParam)
     {
     case VK_LEFT:

        break;
     case VK_RIGHT:
          prlx->prlxScrollAuto("left", 0.005);
        break;

     case VK_UP:

        break;
     case VK_DOWN:

        break;

       case VK_ADD:

        break;
     case VK_SUBTRACT:
        break;
     }
}

void _inputs::keyPressed(_3DModelLoader* Ply, _3DModelLoader* W)
{
      switch(wParam)
     {
     case VK_LEFT:
           W->actionTrigger = Ply->actionTrigger = Ply->RUN;
        break;
     case VK_RIGHT:
           W->actionTrigger = Ply->actionTrigger = Ply->ATTACK;
        break;

     case VK_UP:
           W->actionTrigger = Ply->actionTrigger = Ply->JUMP;
        break;
     case VK_DOWN:
          W->actionTrigger = Ply->actionTrigger = Ply->PAIN;
        break;

     default:
        W->actionTrigger = Ply->actionTrigger = Ply->STAND;
        break;
   }

}

void _inputs::keyPressed(_skyBox* sky)
{

     switch(wParam)
     {
     case VK_LEFT:
           sky->rotation.y -=1.0;
        break;
     case VK_RIGHT:
            sky->rotation.y +=1.0;
        break;

     case VK_UP:
        sky->rotation.x +=1.0;
        break;
     case VK_DOWN:
         sky->rotation.x -=1.0;
        break;

       case VK_ADD:

        break;
     case VK_SUBTRACT:

        break;
     }
}

void _inputs::keyPressed(_sprite* mySprite)
{
     switch(wParam)
     {
     case VK_LEFT:
mySprite->actionTrigger = mySprite->WALKLEFT;
        break;
     case VK_RIGHT:
            mySprite->actionTrigger = mySprite->WALKRIGHT;
        break;

     case VK_UP:

        break;
     case VK_DOWN:

        break;

       case VK_ADD:

        break;
     case VK_SUBTRACT:

        break;
     }
}

void _inputs::keyPressed(_camera* cm)
{
   // cout<< wParam << endl;
     switch(wParam)
     {
     case 65:   //a
           cm->camMoveLtRt(-1);
        break;
     case 68:   //d
            cm->camMoveLtRt(1);
        break;

     case 0x57:    //w - 87/16
            cm->camMoveFdBd(-1);
        break;
     case 0x53:    //s   83/16
            cm->camMoveFdBd(1);
        break;

     case 73:    //i
            cm->rotAngle.y +=1.0;
            cm->rotateXY();
        break;

     case 74: //j
            cm->rotAngle.x -=1.0;
              cm->rotateXY();
        break;

      case 75:    //k
             cm->rotAngle.y -=1.0;
               cm->rotateXY();
        break;

     case 76: //l
           cm->rotAngle.x +=1.0;
             cm->rotateXY();
        break;
     case 32:   // space bar
            cm->camReset();
        break;
     }
}




void _inputs::keyUp(_sprite* mySprite)
{
    mySprite->actionTrigger = mySprite->STAND;
}

void _inputs::keyUp()
{
    switch(wParam)
    {
        default: break;
    }
}

void _inputs::mouseEventDown(_model* mdl, double x, double y)
{
    prev_MouseX =x;
    prev_MouseY =y;

    switch(wParam)
    {
    case MK_LBUTTON:
         isRotationActive =true;  //activate rotation flag
        break;

    case MK_RBUTTON:
        isTranslateActive =true;  //activate translation flag
        break;

    case MK_MBUTTON:
        break;
    }
}

void _inputs::mouseEventUp()
{
    isRotationActive = false;   // deactivate the flags
    isTranslateActive= false;
}

void _inputs::mouseWheel(_model* mdl, double delta)
{
}

void _inputs::mouseMove(_model* mdl, double x, double y)
{
                   // reset mouse y
}

