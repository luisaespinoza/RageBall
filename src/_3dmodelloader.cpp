#include "_3dmodelloader.h"

_3DModelLoader::_3DModelLoader()
{
    //ctor
    pos.x =0;
    pos.y =0;
    pos.z =-10.0;
	animTime        = 0.0;
	animPhaseOffset = 0.0f;
	animSpeed       = 5.0f;
	StartFrame      = 0;
	EndFrame        = 0;
}
_3DModelLoader::_3DModelLoader(vec3 initPos)
{
	pos   = initPos;

	animTime        = 0.0;
	animPhaseOffset = 0.0f;
	animSpeed       = 5.0f;
	StartFrame      = 0;
	EndFrame        = 0;
}

_3DModelLoader::~_3DModelLoader()
{
    //dtor

    FreeModel(&md2file);
}
int _3DModelLoader::ReadMD2Model(const char* filename, struct md2_model_t* mdl)
{
     FILE *fp;

  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
      return 0;
    }

  /* Read header */
  fread (&mdl->header, 1, sizeof (struct md2_header_t), fp);

  if ((mdl->header.ident != 844121161) ||
      (mdl->header.version != 8))
    {
      /* Error! */
      fprintf (stderr, "Error: bad version or identifier\n");
      fclose (fp);
      return 0;
    }

  /* Memory allocations */
  mdl->skins = (struct md2_skin_t *)malloc (sizeof (struct md2_skin_t) * mdl->header.num_skins);
  mdl->texcoords = (struct md2_texCoord_t *)malloc (sizeof (struct md2_texCoord_t) * mdl->header.num_st);
  mdl->triangles = (struct md2_triangle_t *)malloc (sizeof (struct md2_triangle_t) * mdl->header.num_tris);
  mdl->frames = (struct md2_frame_t *)malloc (sizeof (struct md2_frame_t) * mdl->header.num_frames);
  mdl->glcmds = (int *)malloc (sizeof (int) * mdl->header.num_glcmds);

  /* Read model data */
  fseek (fp, mdl->header.offset_skins, SEEK_SET);
  fread (mdl->skins, sizeof (struct md2_skin_t),mdl->header.num_skins, fp);

  fseek (fp, mdl->header.offset_st, SEEK_SET);
  fread (mdl->texcoords, sizeof (struct md2_texCoord_t),mdl->header.num_st, fp);

  fseek (fp, mdl->header.offset_tris, SEEK_SET);
  fread (mdl->triangles, sizeof (struct md2_triangle_t),mdl->header.num_tris, fp);

  fseek (fp, mdl->header.offset_glcmds, SEEK_SET);
  fread (mdl->glcmds, sizeof (int), mdl->header.num_glcmds, fp);

  /* Read frames */
  fseek (fp, mdl->header.offset_frames, SEEK_SET);
  for (int i = 0; i < mdl->header.num_frames; ++i)
    {
      /* Memory allocation for vertices of this frame */
      mdl->frames[i].verts = (struct md2_vertex_t *)malloc (sizeof (struct md2_vertex_t) * mdl->header.num_vertices);

      /* Read frame data */
      fread (mdl->frames[i].scale, sizeof (vec3_t), 1, fp);
      fread (mdl->frames[i].translate, sizeof (vec3_t), 1, fp);
      fread (mdl->frames[i].name, sizeof (char), 16, fp);
      fread (mdl->frames[i].verts, sizeof (struct md2_vertex_t),mdl->header.num_vertices, fp);

      //cout<<mdl->frames[i].name<<endl; // only for Debug
    }

    for(int i =0; i<mdl->header.num_skins; i++){
        cout<<mdl->skins[i].name<<endl;  // only for debug
        //myTex->loadTexture("models/Tekk/blade.jpg");
       // mdl->tex_id = myTex->textID;
    }
     EndFrame = mdl->header.num_frames-1;

  fclose (fp);
  return 1;

}

void _3DModelLoader::RenderFrame(int n, const struct md2_model_t* mdl)
{
       /* Check if n is in a valid range */
  if ((n < 0) || (n > mdl->header.num_frames - 1))
    return;

  /* Enable model's texture */
  glBindTexture (GL_TEXTURE_2D, mdl->tex_id);

  /* Draw the model */
  glBegin (GL_TRIANGLES);
    /* Draw each triangle */
    for (int i = 0; i < mdl->header.num_tris; ++i)
      {
	/* Draw each vertex */
	for (int j = 0; j < 3; ++j)
	  {
	    pframe = &mdl->frames[n];
	    pvert = &pframe->verts[mdl->triangles[i].vertex[j]];

	    /* Compute texture coordinates */
	    s = (GLfloat)mdl->texcoords[mdl->triangles[i].st[j]].s / mdl->header.skinwidth;
	    t = (GLfloat)mdl->texcoords[mdl->triangles[i].st[j]].t / mdl->header.skinheight;

	    /* Pass texture coordinates to OpenGL */
	    glTexCoord2f (s, t);

	    /* Normal vector */
	    glNormal3fv (anorms_table[pvert->normalIndex]);

	    /* Calculate vertex real position */
	    v[0] = (pframe->scale[0] * pvert->v[0]) + pframe->translate[0];
	    v[1] = (pframe->scale[1] * pvert->v[1]) + pframe->translate[1];
	    v[2] = (pframe->scale[2] * pvert->v[2]) + pframe->translate[2];

	    glVertex3fv (v);
	  }
      }
  glEnd ();

}

void _3DModelLoader::RenderFrameItpWithGLCmds(int n, float interp, const struct md2_model_t* mdl)
{

   /* Check if n is in a valid range */
  if ((n < 0) || (n > mdl->header.num_frames - 1))
    return;

  /* Enable model's texture */
//  glBindTexture (GL_TEXTURE_2D, mdl->tex_id);
if (mdl->tex_id) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mdl->tex_id);
} else {
    glDisable(GL_TEXTURE_2D);
}
  /* pglcmds points at the start of the command list */
  pglcmds = mdl->glcmds;

  /* Draw the model */
  while ((i = *(pglcmds++)) != 0)
    {
      if (i < 0)
	{
	  glBegin (GL_TRIANGLE_FAN);
	  i = -i;
	}
      else
	{
	  glBegin (GL_TRIANGLE_STRIP);
	}

      /* Draw each vertex of this group */
      for (/* Nothing */; i > 0; --i, pglcmds += 3)
	{
	  packet = (struct md2_glcmd_t *)pglcmds;
	  pframe1 = &mdl->frames[n];
	  pframe2 = &mdl->frames[n + 1];
	  pvert1 = &pframe1->verts[packet->index];
	  pvert2 = &pframe2->verts[packet->index];

	  /* Pass texture coordinates to OpenGL */
	  glTexCoord2f (packet->s, packet->t);

	  /* Interpolate normals */
	  n_curr = anorms_table[pvert1->normalIndex];
	  n_next = anorms_table[pvert2->normalIndex];

	  norm[0] = n_curr[0] + interp * (n_next[0] - n_curr[0]);
	  norm[1] = n_curr[1] + interp * (n_next[1] - n_curr[1]);
	  norm[2] = n_curr[2] + interp * (n_next[2] - n_curr[2]);

	  glNormal3fv (norm);

	  /* Interpolate vertices */
	  v_curr[0] = pframe1->scale[0] * pvert1->v[0] + pframe1->translate[0];
	  v_curr[1] = pframe1->scale[1] * pvert1->v[1] + pframe1->translate[1];
	  v_curr[2] = pframe1->scale[2] * pvert1->v[2] + pframe1->translate[2];

	  v_next[0] = pframe2->scale[0] * pvert2->v[0] + pframe2->translate[0];
	  v_next[1] = pframe2->scale[1] * pvert2->v[1] + pframe2->translate[1];
	  v_next[2] = pframe2->scale[2] * pvert2->v[2] + pframe2->translate[2];

	  v[0] = v_curr[0] + interp * (v_next[0] - v_curr[0]);
	  v[1] = v_curr[1] + interp * (v_next[1] - v_curr[1]);
	  v[2] = v_curr[2] + interp * (v_next[2] - v_curr[2]);

	  glVertex3fv (v);
	}
      glEnd ();
    }
}

void _3DModelLoader::Animate(int start, int end, int* frame, float* interp)
{
         if ((*frame < start) || (*frame > end))
    *frame = start;

    if (*interp >= 1.0f)
    {
      /* Move to next frame */
      *interp = 0.0f;
      (*frame)++;

      if (*frame >= end)
	*frame = start;
    }

}
int _3DModelLoader::initModel(const char* filename)
{
     /* Load MD2 model file */
//    if (!ReadMD2Model (filename, &md2file))
//    exit (EXIT_FAILURE);
    int ok = ReadMD2Model(filename, &md2file);
    if (!ok) {
        std::cerr << "[MD2] initModel: failed to load " << filename << "\n";
        return 0;                     // <-- propagate failure, do NOT exit
    }
    return 1;
}

void _3DModelLoader::Draw() {
	// fallback dt if somebody calls the no-arg version
	Draw(1.0 / 60.0);
}

void _3DModelLoader::Draw(double dt)
{
	glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
	// Bind this instance's texture (per-instance tex_id)
	if (md2file.tex_id) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, md2file.tex_id);
	} else {
		glDisable(GL_TEXTURE_2D);
	}

	// --- animation clock (per instance) ---
	// dt from your main loop is already in seconds.
	if (dt < 0.0)
		dt = 0.0;
	animTime += dt;

	// This matches the old "globalTime + animPhaseOffset" * globalSpeed.
	float t = static_cast<float>((animTime + animPhaseOffset) * animSpeed);

	int frameCount = EndFrame - StartFrame; // number of blend segments
	if (frameCount <= 0) {
		// Degenerate range: just draw StartFrame
		RenderFrame(StartFrame, &md2file);
		return;
	}

	// base frame in [StartFrame, EndFrame-1]
	int base = StartFrame + (static_cast<int>(t) % frameCount);

	// fractional part [0, 1) between base and base+1
	float interp = t - floorf(t);

	RenderFrameItpWithGLCmds(base, interp, &md2file);
	RenderFrameItpWithGLCmds(base, interp, &md2file);
	glPopAttrib();
}

//
//void _3DModelLoader::Draw(double dt)
//{
//  static int n = 0;
//  static float interp = 0.0f;
//
//  if (md2file.tex_id) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, md2file.tex_id); }
//  else { glDisable(GL_TEXTURE_2D); }
//
//  float advance = animSpeed * (dt > 0 ? (float)dt : 0.0f);
//  interp += advance;
//  Animate(StartFrame, EndFrame, &n, &interp);
//
//  RenderFrameItpWithGLCmds(n, interp, &md2file);
//}
void _3DModelLoader::Actions()
{
    switch(actionTrigger)
   {
       case STAND: StartFrame=0; EndFrame =39;break;
       case RUN: StartFrame=40; EndFrame =45;break;
       case ATTACK: StartFrame=46; EndFrame =53;break;
       case PAIN: StartFrame=47; EndFrame =65;break;
       case JUMP: StartFrame=66; EndFrame =71;break;
   }
}

void _3DModelLoader::FreeModel(struct md2_model_t* mdl)
{
     if (mdl->skins)
    {
      free (mdl->skins);
      mdl->skins = NULL;
    }

  if (mdl->texcoords)
    {
      free (mdl->texcoords);
      mdl->texcoords = NULL;
    }

  if (mdl->triangles)
    {
      free (mdl->triangles);
      mdl->triangles = NULL;
    }

  if (mdl->glcmds)
    {
      free (mdl->glcmds);
      mdl->glcmds = NULL;
    }

  if (mdl->frames)
    {

      for (int i = 0; i < mdl->header.num_frames; ++i)
	{
	  free (mdl->frames[i].verts);
	  mdl->frames[i].verts = NULL;
	}

      free (mdl->frames);
      mdl->frames = NULL;
    }
}
