#include "_camera.h"

_camera::_camera()
{
    //ctor
}

_camera::~_camera()
{
    //dtor
}
void _camera::camInit()
{
    eye.x =0; eye.y =0; eye.z =10;
    des.x =0; des.y =0; des.z =-10;
    up.x  =0; up.y  =1; up.z  =0;

    step =0.5;

    distance = sqrt(pow(eye.x-des.x,2)+pow(eye.y-des.y,2)+pow(eye.z-des.z,2));

    rotAngle.x =0;
    rotAngle.y =0;
}

void _camera::camReset()
{
    eye.x =0; eye.y =0; eye.z =10;
    des.x =0; des.y =0; des.z =0;
    up.x  =0; up.y  =1; up.z  =0;

    step =0.5;

    distance = sqrt(pow(eye.x-des.x,2)+pow(eye.y-des.y,2)+pow(eye.z-des.z,2));

    rotAngle.x =0;
    rotAngle.y =0;
}

void _camera::rotateXY()
{
    eye.x = des.x + distance*cos(rotAngle.y*PI/180.0)*sin(rotAngle.x*PI/180.0);
    eye.y = des.y + distance*sin(rotAngle.y*PI/180.0);
    eye.z = des.z + distance*cos(rotAngle.y*PI/180.0)*cos(rotAngle.x*PI/180.0);
}

void _camera::rotateUP()
{

}

void _camera::camMoveFdBd(int dir)
{
       eye.z += step*dir; // if forward dir =1 else dir =-1
       des.z += step*dir;
}

void _camera::camMoveLtRt(int dir)
{
    eye.x += step*dir;
    des.x += step*dir;
}

void _camera::setUpCamera()
{
    gluLookAt(eye.x,eye.y,eye.z,
              des.x,des.y,des.z,
              up.x, up.y, up.z);
}
