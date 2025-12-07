/**
 * 
 */

#ifndef _OBJLOADER_H
#define _OBJLOADER_H

#include<_common.h>

struct Vertex {
    float x, y, z;
};

struct Normal {
    float nx, ny, nz;
};

struct TexCoord {
    float u, v;
};

// Triangle face (whole row!)
struct Face3 {
    int v1, v2, v3;
    int n1, n2, n3;
    int t1, t2, t3;
};

// Quad face
struct Face4 {
    int v1, v2, v3, v4;
    int n1, n2, n3, n4;
    int t1, t2, t3, t4;
};

struct ExpandedVertex3 {
    float vx, vy, vz;      // position
    float nx, ny, nz;   // normal
    float tu, tv;         // texture coordinate
};

enum ObjDebugType {
    OBJ_VERTEX,
    OBJ_NORMAL,
    OBJ_TEXCOORD,
    OBJ_FACE3,
    OBJ_FACE4,
    OBJ_ALL
};

// Loads and renders a 3D model from an OBJ file
class _objLoader
{
    public:
        _objLoader();
        virtual ~_objLoader();

        vector<Vertex> vertices;
        vector<Normal> normals;
        vector<TexCoord> texCords;
        vector<Face3> faces3;
        vector<Face4> faces4;

        size_t vertexCount = 0;
        size_t normalCount = 0;
        size_t texcoordCount = 0;

        size_t faceCount = 0;

        size_t modelSize = 0;
        GLuint vbo = 0; // Vertex Buffer Object
        // WARNING WILL CRASH ON LARGE FILES -- YOU HAVE BEEN WARNED. Prints all Vertices, Normals, and TexCoords to console
        bool megaDebug = false; 

        // Sets up the OBJ model from file, returns true on success
        bool initObj(const char* filename);
        // Renders the loaded OBJ model -- make sure to call initObj() -> buildBuffer() first
        void drawObj();
        // Debug info to console
        void debug();
        // Debug specfic data to log file as OBJ_VERTEX, OBJ_NORMAL, OBJ_TEXCOORD,OBJ_FACE3, OBJ_FACE4, OBJ_ALL
        void logDebug(ObjDebugType type);
        // Builds the VBO from loaded data (must be called after initObj)
        void buildBuffer();

        void parseFace(int &x, int &y, int &z, char** tape);
        float parseEntry(char** tape);

    protected:

    private:
        
        

};

#endif // _OBJLOADER_H