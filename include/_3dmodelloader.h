#ifndef _3DMODELLOADER_H
#define _3DMODELLOADER_H

#include<_common.h>
#include<_textureloader.h>


/* Vector */
typedef float vec3_t[3];

/* MD2 header */
struct md2_header_t
{
  int ident;
  int version;

  int skinwidth;
  int skinheight;

  int framesize;

  int num_skins;
  int num_vertices;
  int num_st;
  int num_tris;
  int num_glcmds;
  int num_frames;

  int offset_skins;
  int offset_st;
  int offset_tris;
  int offset_frames;
  int offset_glcmds;
  int offset_end;
};

/* Texture name */
struct md2_skin_t
{
  char name[64];
};

/* Texture coords */
struct md2_texCoord_t
{
  short s;
  short t;
};

/* Triangle info */
struct md2_triangle_t
{
  unsigned short vertex[3];
  unsigned short st[3];
};

/* Compressed vertex */
struct md2_vertex_t
{
  unsigned char v[3];
  unsigned char normalIndex;
};

/* Model frame */
struct md2_frame_t
{
  vec3_t scale;
  vec3_t translate;
  char name[16];
  struct md2_vertex_t *verts;
};

/* GL command packet */
struct md2_glcmd_t
{
  float s;
  float t;
  int index;
};

/* MD2 model structure */
struct md2_model_t
{
  struct md2_header_t header;

  struct md2_skin_t *skins;
  struct md2_texCoord_t *texcoords;
  struct md2_triangle_t *triangles;
  struct md2_frame_t *frames;
  int *glcmds;

  GLuint tex_id;
};

/* Table of precalculated normals */



class _3DModelLoader
{
    public:
        _3DModelLoader();
        _3DModelLoader(vec3 initPos);
        virtual ~_3DModelLoader();
        GLfloat s, t;
          int i, *pglcmds;
          vec3_t anorms_table[162] = {
            #include "Anorms.h"
            };

          _textureLoader *myTex = new _textureLoader();
          vec3_t v_curr, v_next, v, norm;
          float *n_curr, *n_next;
          struct md2_frame_t *pframe, *pframe1, *pframe2;
          struct md2_vertex_t *pvert, *pvert1, *pvert2;
          struct md2_glcmd_t *packet;
          struct md2_model_t md2file;

          int ReadMD2Model (const char *filename, struct md2_model_t *mdl);
          void RenderFrame (int n, const struct md2_model_t *mdl);
          void RenderFrameItpWithGLCmds (int n, float interp, const struct md2_model_t *mdl);
          void Animate (int start, int end, int *frame, float *interp);
          int initModel(const char *filename);
          void setTexture(GLuint tex) { md2file.tex_id = tex; }//modular. Load any texture
          void Draw();
          void Draw(double dt);
          float animSpeed = 5.0f;  // frames per second, dt-scaled
          void FreeModel (struct md2_model_t *mdl);
          void Actions();


          enum {STAND, WALKLEFT,WALKRIGHT,RUN,JUMP,ATTACK,PAIN};   // model actions
          int actionTrigger =0;                                //Action Trigger
          int StartFrame =0;
          int EndFrame;

          vec3 pos;
          float dirAngleZ=180;
    protected:

    private:
};

#endif // _3DMODELLOADER_H
