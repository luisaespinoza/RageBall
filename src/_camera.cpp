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

void _camera::updateFPSCamera() {
    // Calculate forward direction from yaw/pitch
    float yawRad = rotAngle.x * PI / 180.0f;
    float pitchRad = rotAngle.y * PI / 180.0f;

    // Forward vector (OpenGL convention: -Z is forward)
    vec3 forward;
    forward.x = -sin(yawRad) * cos(pitchRad);  
    forward.y = sin(pitchRad);
    forward.z = -cos(yawRad) * cos(pitchRad);  

    // Destination = eye + forward
    des.x = eye.x + forward.x;
    des.y = eye.y + forward.y;
    des.z = eye.z + forward.z;
}

void _camera::moveFPSForward(float amount) {
    // Calculate forward vector (ignore pitch for horizontal movement)
    float yawRad = rotAngle.x * PI / 180.0f;
    
    vec3 forward;
    forward.x = -sin(yawRad);  
    forward.y = 0.0f;
    forward.z = -cos(yawRad);  
    
    // Normalize
    float len = sqrt(forward.x * forward.x + forward.z * forward.z);
    if (len > 0.0001f) {
        forward.x /= len;
        forward.z /= len;
    }
    
    // Move eye position
    eye.x += forward.x * amount;
    eye.z += forward.z * amount;
    
    // Update destination
    updateFPSCamera();
}

void _camera::moveFPSStrafe(float amount) {
    float yawRad = rotAngle.x * PI / 180.0f;
    
    vec3 right;
    right.x = -cos(yawRad);  
    right.y = 0.0f;
    right.z = sin(yawRad);   
    
    eye.x += right.x * amount;
    eye.z += right.z * amount;
    
    updateFPSCamera();
}

void _camera::moveFPSUp(float amount) {
    // Move in world Y axis
    eye.y += amount;
    
    // Update destination
    updateFPSCamera();
}