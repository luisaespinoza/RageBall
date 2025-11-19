#ifndef _COMMON_H
#define _COMMON_H

#include<iostream>
#include<windows.h>
#include<string>

#include<gl/gl.h>
#include<GL/glut.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<memory>
#include<vector>
#include<fstream>
#include<sstream>
#include<cassert>
#include<functional>
#include<unordered_map>
#include<cmath>
#include<limits>
#include<algorithm>
#include<cstring>
#include <cstdio>

#define PI 3.14159265
// 32-bit golden-ratio constant (used for RNG seeding / hashing)
constexpr uint32_t GOLDEN_RATIO = 0x9E3779B9u;
using namespace std;

typedef struct{
          float x;
          float y;
          } vec2;

typedef struct{
          float x;
          float y;
          float z;
          } vec3;

typedef struct{
          float r;
          float g;
          float b;
          float a;
          } col4;

struct _SceneInterface{

    virtual ~_SceneInterface() = default;
    virtual void onEnter(){};
    virtual void onExit() {};
    virtual handleEvent(const MSG&/* e*/){};
    virtual void onResize(int /*w*/, int /*h*/) {};
    virtual void update(double /*dt*/) {};
    virtual void render() = 0;
    virtual int  winMsg(HWND, UINT, WPARAM, LPARAM) { return 0; };
    virtual const char* name() const {return "Scene";}
};
struct RenderFlags {
  bool showSky     = true;
  bool showTerrain = true;
  bool showActors  = true;
  bool showUI      = false;
};

struct ILevel {
  virtual ~ILevel() = default;
  virtual const char* id() const = 0;

  virtual void loadAssets() = 0;      // textures/models/skybox
  virtual void unloadAssets() = 0;
  virtual void reset() = 0;           // place player, reset state
  virtual void update(double dt) = 0;
  virtual void render(const RenderFlags& flags) = 0;
  virtual void handleKey(UINT /*uMsg*/,WPARAM /*wParam*/){};
  virtual void applyCamera(){};
};



/*KeyPress Constants*/
#define KEY_W           0x57
#define KEY_A           0x41
#define KEY_S           0x53
#define KEY_D           0x44
#define KEY_SPACE       0x20

#endif // _COMMON_H
