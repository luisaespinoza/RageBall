#ifndef _COMMON_H
#define _COMMON_H

#include<iostream>
#include<windows.h>
#include<string>

#define GL_GLEXT_PROTOTYPES
#include<gl/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>
#include<GL/glext.h>
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
#include<chrono>
#include<random>

// OBJ LOADER BUFFERS //
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif

typedef void (APIENTRY * PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (APIENTRY * PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLBUFFERDATAPROC)(GLenum target, ptrdiff_t size, const GLvoid* data, GLenum usage);
typedef void (APIENTRY * PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);

extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
/////////////////////////
#define PI 3.14159265
// 32-bit golden-ratio constant (used for RNG seeding / hashing)
constexpr uint32_t GOLDEN_RATIO = 0x9E3779B9u;
using namespace std;
/* CLASSES */
// Random Number Generator Utility Class -- To use just call RNG::getInt(min,max) or RNG::getFloat(min,max). Setup in main.cpp @ WinMain. Static function so only one instance exists and doesnt require instantiation.
class RNG {
    private:
        static mt19937 generator;
        static bool initialized;
    public:
        static void init() {
            if (!initialized) {
                random_device rd;
                generator.seed(rd());
                initialized = true;
            }
        }
        static int getInt(int min, int max) {
            uniform_int_distribution<int> distribution(min, max);
            return distribution(generator);
        }
        static float getFloat(float min, float max) {
            uniform_real_distribution<float> distribution(min, max);
            return distribution(generator);
        }
};

/* STRUCTS */
struct vec2f {
    float x;
    float y;
};

struct vec3f {
    float x;
    float y;
    float z;
};
struct col4f {
    float r;
    float g;
    float b;
    float a;
};

struct col3f{
    float r;
    float g;
    float b;
};

// OTHER STRUCTS //

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
    virtual void handleEvent(const MSG&/* e*/){};
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
  virtual int winMsg(HWND, UINT, WPARAM, LPARAM) { return 0; }; // for mouse!
  virtual void applyCamera(){};
};



/*KeyPress Constants*/
#define KEY_W           0x57
#define KEY_A           0x41
#define KEY_S           0x53
#define KEY_D           0x44
#define KEY_SPACE       0x20

#endif // _COMMON_H
