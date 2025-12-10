#include <_objloader.h>

_objLoader::_objLoader()
{
    //ctor
}

_objLoader::~_objLoader()
{
    //dtor
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
}

void _objLoader::buildBuffer() {
    vector<ExpandedVertex3> expandedVertices;    // final expanded vertex buffer
    expandedVertices.reserve(faces3.size() * 3 + faces4.size() * 6); // reserve space for triangles + quads (split into 2 triangles each)
    
    // Process triangle faces
    for (int i = 0; i < faces3.size(); i++) {    // for each face we build our buffer
        ExpandedVertex3 ev1, ev2, ev3;
        // Expand vertex 1 //
        ev1.vx = vertices[faces3[i].v1 - 1].x;
        ev1.vy = vertices[faces3[i].v1 - 1].y;
        ev1.vz = vertices[faces3[i].v1 - 1].z;
        if (faces3[i].n1 > 0) {
            ev1.nx = normals[faces3[i].n1 - 1].nx;
            ev1.ny = normals[faces3[i].n1 - 1].ny;
            ev1.nz = normals[faces3[i].n1 - 1].nz;
        } else {
            ev1.nx = ev1.ny = ev1.nz = 0.0f;
        }
        if (faces3[i].t1 > 0) {
            ev1.tu = texCords[faces3[i].t1 - 1].u;
            ev1.tv = texCords[faces3[i].t1 - 1].v;
        } else {
            ev1.tu = ev1.tv = 0.0f;
        }
        expandedVertices.push_back(ev1);
        // Expand vertex 2 //
        ev2.vx = vertices[faces3[i].v2 - 1].x;
        ev2.vy = vertices[faces3[i].v2 - 1].y;
        ev2.vz = vertices[faces3[i].v2 - 1].z;
        if (faces3[i].n2 > 0) {
            ev2.nx = normals[faces3[i].n2 - 1].nx;
            ev2.ny = normals[faces3[i].n2 - 1].ny;
            ev2.nz = normals[faces3[i].n2 - 1].nz;
        } else {
            ev2.nx = ev2.ny = ev2.nz = 0.0f;
        }
        if (faces3[i].t2 > 0) {
            ev2.tu = texCords[faces3[i].t2 - 1].u;
            ev2.tv = texCords[faces3[i].t2 - 1].v;
        } else {
            ev2.tu = ev2.tv = 0.0f;
        }
        expandedVertices.push_back(ev2);
        // Expand vertex 3 //
        ev3.vx = vertices[faces3[i].v3 - 1].x;
        ev3.vy = vertices[faces3[i].v3 - 1].y;
        ev3.vz = vertices[faces3[i].v3 - 1].z;
        if (faces3[i].n3 > 0) {
            ev3.nx = normals[faces3[i].n3 - 1].nx;
            ev3.ny = normals[faces3[i].n3 - 1].ny;
            ev3.nz = normals[faces3[i].n3 - 1].nz;
        } else {
            ev3.nx = ev3.ny = ev3.nz = 0.0f;
        }
        if (faces3[i].t3 > 0) {
            ev3.tu = texCords[faces3[i].t3 - 1].u;
            ev3.tv = texCords[faces3[i].t3 - 1].v;
        } else {
            ev3.tu = ev3.tv = 0.0f;
        }
        expandedVertices.push_back(ev3);
    }
    
    // Process quad faces - split each quad into 2 triangles (v1,v2,v3) and (v1,v3,v4)
    for (int i = 0; i < faces4.size(); i++) {
        ExpandedVertex3 ev1, ev2, ev3, ev4;
        
        // Expand vertex 1 //
        ev1.vx = vertices[faces4[i].v1 - 1].x;
        ev1.vy = vertices[faces4[i].v1 - 1].y;
        ev1.vz = vertices[faces4[i].v1 - 1].z;
        if (faces4[i].n1 > 0) {
            ev1.nx = normals[faces4[i].n1 - 1].nx;
            ev1.ny = normals[faces4[i].n1 - 1].ny;
            ev1.nz = normals[faces4[i].n1 - 1].nz;
        } else {
            ev1.nx = ev1.ny = ev1.nz = 0.0f;
        }
        if (faces4[i].t1 > 0) {
            ev1.tu = texCords[faces4[i].t1 - 1].u;
            ev1.tv = texCords[faces4[i].t1 - 1].v;
        } else {
            ev1.tu = ev1.tv = 0.0f;
        }
        
        // Expand vertex 2 //
        ev2.vx = vertices[faces4[i].v2 - 1].x;
        ev2.vy = vertices[faces4[i].v2 - 1].y;
        ev2.vz = vertices[faces4[i].v2 - 1].z;
        if (faces4[i].n2 > 0) {
            ev2.nx = normals[faces4[i].n2 - 1].nx;
            ev2.ny = normals[faces4[i].n2 - 1].ny;
            ev2.nz = normals[faces4[i].n2 - 1].nz;
        } else {
            ev2.nx = ev2.ny = ev2.nz = 0.0f;
        }
        if (faces4[i].t2 > 0) {
            ev2.tu = texCords[faces4[i].t2 - 1].u;
            ev2.tv = texCords[faces4[i].t2 - 1].v;
        } else {
            ev2.tu = ev2.tv = 0.0f;
        }
        
        // Expand vertex 3 //
        ev3.vx = vertices[faces4[i].v3 - 1].x;
        ev3.vy = vertices[faces4[i].v3 - 1].y;
        ev3.vz = vertices[faces4[i].v3 - 1].z;
        if (faces4[i].n3 > 0) {
            ev3.nx = normals[faces4[i].n3 - 1].nx;
            ev3.ny = normals[faces4[i].n3 - 1].ny;
            ev3.nz = normals[faces4[i].n3 - 1].nz;
        } else {
            ev3.nx = ev3.ny = ev3.nz = 0.0f;
        }
        if (faces4[i].t3 > 0) {
            ev3.tu = texCords[faces4[i].t3 - 1].u;
            ev3.tv = texCords[faces4[i].t3 - 1].v;
        } else {
            ev3.tu = ev3.tv = 0.0f;
        }
        
        // Expand vertex 4 //
        ev4.vx = vertices[faces4[i].v4 - 1].x;
        ev4.vy = vertices[faces4[i].v4 - 1].y;
        ev4.vz = vertices[faces4[i].v4 - 1].z;
        if (faces4[i].n4 > 0) {
            ev4.nx = normals[faces4[i].n4 - 1].nx;
            ev4.ny = normals[faces4[i].n4 - 1].ny;
            ev4.nz = normals[faces4[i].n4 - 1].nz;
        } else {
            ev4.nx = ev4.ny = ev4.nz = 0.0f;
        }
        if (faces4[i].t4 > 0) {
            ev4.tu = texCords[faces4[i].t4 - 1].u;
            ev4.tv = texCords[faces4[i].t4 - 1].v;
        } else {
            ev4.tu = ev4.tv = 0.0f;
        }
        
        // First triangle: v1, v2, v3
        expandedVertices.push_back(ev1);
        expandedVertices.push_back(ev2);
        expandedVertices.push_back(ev3);
        
        // Second triangle: v1, v3, v4
        expandedVertices.push_back(ev1);
        expandedVertices.push_back(ev3);
        expandedVertices.push_back(ev4);
    }
    
    vertexCount = expandedVertices.size();
    // Buffer completed, time to upload to GPU //
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                    expandedVertices.size() * sizeof(ExpandedVertex3),
                    expandedVertices.data(),
                    GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    cout << "VBO created with " << vertexCount << " vertices (" 
         << faces3.size() << " triangles, " << faces4.size() << " quads)\n";
}

bool _objLoader::initObj(const char* filename)
{
    try {                     
        float v1,v2,v3;
        FILE* file = fopen(filename, "r");
        if (!file) {
            cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        fseek(file, 0, SEEK_END);                   // set tape to end of file                        
        size_t fileSize = ftell(file);              // get current position of tape (end of file)
        fseek(file, 0, SEEK_SET);                   // set tape back to beginning of file
        char* fileData = new char[fileSize + 1];    // allocate memory for file data (+1 for null terminator)
        fread(fileData, 1, fileSize, file);         // read file data into memory
        cout << "Loading OBJ file: " << filename << " (" << fileSize << "B)" << endl;
        // Step 1. Check tag -- tag can either be "#", "v", "vt", "vn", "f"
        char* tape = fileData;                      // use pointer movement through the file data (fast) call it "tape" as a tapeHead (where we are at!)
        char* end = fileData + fileSize;            // end of file data
        int where = 0;
        while (tape < end) {
            if (*tape == '#') { // comment line, skip to next line
                while (*tape != '\n' && tape < end) {  
                    tape++;
                }
            } else if (*tape == ' ' || *tape == '\n' && tape < end) { // skip spaces and new lines
                tape++;
            } else if (*tape == 'v') { // data found, could be v, vt, or vn
                tape++;
                if (*tape == 't') {
                    /* VERTEX TEXTURE */
                    tape++;
                    while (*tape == ' ' || *tape == '\t') {  // skip whitespace
                        tape++;
                    }
                    v1 = parseEntry(&tape);
                    tape++; // skip space
                    v2 = parseEntry(&tape);
                    TexCoord texCord = {v1, v2};
                    texCords.push_back(texCord);  
                } else if (*tape == 'n') {
                    /* VERTEX NORMAL */
                    tape++;
                    while (*tape == ' ' || *tape == '\t') {  // skip whitespace
                        tape++;
                    }
                    v1 = parseEntry(&tape);
                    tape++; // skip space
                    v2 = parseEntry(&tape);
                    tape++; // skip space
                    v3 = parseEntry(&tape);
                    Normal normal = {v1, v2, v3};
                    normals.push_back(normal);  
                } else {
                    /* VERTEX */
                    while (*tape == ' ' || *tape == '\t') {  // skip whitespace
                        tape++;
                    }
                    v1 = parseEntry(&tape);
                    tape++; // skip space
                    v2 = parseEntry(&tape);
                    tape++; // skip space
                    v3 = parseEntry(&tape);
                    Vertex vertex = {v1, v2, v3};
                    vertices.push_back(vertex);  
                }
            } else if (*tape == 'f') {
                bool isQuad = false;
                Face3 temp_face;
                /*
                The faces are a bit trickier, they do not have whitespace separated values, but instead us '/', but use spaces to separate each entry (9 values total!)
                In addition: there are different combinations,
                    f 1 2 3                     (vertex)
                    f 3/1 4/2 5/3               (vertex, texture coordinate)
                    f 6/4/1 3/5/3 7/6/5         (vertex, texture coordinate, vertex normal)
                    f 7//1 8//2 9//3            (vertex, vertex normal)
                    - https://en.wikipedia.org/wiki/Wavefront_.obj_file -
                Finally, a Face can either be a Quad or a Triangle, we only support Triangles here.
                */
                tape+=2;
                parseFace(temp_face.v1, temp_face.t1, temp_face.n1, &tape);   
                tape++;  
                parseFace(temp_face.v2, temp_face.t2, temp_face.n2, &tape);   
                tape++;  
                parseFace(temp_face.v3, temp_face.t3, temp_face.n3, &tape);
                while (*tape == ' ' || *tape == '\t') {
                    tape++;
                }
                if (*tape >= '0' && *tape <= '9') {
                    // QUAD //
                    Face4 temp_faceq;
                    temp_faceq.v1 = temp_face.v1; temp_faceq.v2 = temp_face.v2; temp_faceq.v3 = temp_face.v3;
                    temp_faceq.t1 = temp_face.t1; temp_faceq.t2 = temp_face.t2; temp_faceq.t3 = temp_face.t3;
                    temp_faceq.n1 = temp_face.n1; temp_faceq.n2 = temp_face.n2; temp_faceq.n3 = temp_face.n3;
                    parseFace(temp_faceq.v4, temp_faceq.t4, temp_faceq.n4, &tape);    
                    faces4.push_back(temp_faceq);
                } else {
                    faces3.push_back(temp_face);
                }
            } else {
                tape++;
            }
        }
        fclose(file);
        vertexCount = vertices.size();
        normalCount = normals.size();
        texcoordCount = texCords.size();
        faceCount = faces3.size() + faces4.size();
        modelSize = vertexCount * sizeof(Vertex) +
                    normalCount * sizeof(Normal) +
                    texcoordCount * sizeof(TexCoord) +
                    faces3.size() * sizeof(Face3) +
                    faces4.size() * sizeof(Face4);
        return true;
    } catch (exception &ex) {
        cerr << "ERROR: Cannot load model ... reason: " << ex.what() << "\n";
        return false;
    }
}

void _objLoader::drawObj()
{
    if (vbo == 0 || vertexCount == 0) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(ExpandedVertex3), (void*)offsetof(ExpandedVertex3, vx));
    glNormalPointer(GL_FLOAT, sizeof(ExpandedVertex3), (void*)offsetof(ExpandedVertex3, nx));
    glTexCoordPointer(2, GL_FLOAT, sizeof(ExpandedVertex3), (void*)offsetof(ExpandedVertex3, tu));

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Parses a char* entry from the tape and returns its parsed float value (also moves the tape forward)
float _objLoader::parseEntry(char** tape)           // we use a double pointer so that the original tape is modified
{
    char* checkpoint = *tape;                       // mark the start of the entry              
    while (**tape != ' ' && **tape != '\n' && **tape != '/' && **tape != '\0') {       // segment entry by space or new line
        (*tape)++;
    }
    size_t temp_size = *tape - checkpoint;          // calculate size of entry
    char* buffer = new char[temp_size + 1];         // allocate buffer to hold entry + null terminator
    memcpy(buffer,checkpoint,temp_size);            // copy entry into buffer
    buffer[temp_size] = '\0';                       // null terminate the buffer
    float value = atof(buffer);                     // atof converts char* to float
    delete[] buffer;                                // free buffer memory
    buffer = nullptr;
    return value;                                   // return the parsed float value
}

// Parses a char* entry from the tape for a face returning an int
void _objLoader::parseFace(int &x, int &y, int &z, char** tape) {
    x = (int)parseEntry(tape);
    if (**tape == ' ') {
        // VERTEX ONLY //
        y = 0;
        z = 0;
    } else {
        // '/' found
        (*tape)++;
        if (**tape == '/') {
            // VERTEX + NORMAL //
            y = 0;  // no vertex texture
            (*tape)++;
            z = (int)parseEntry(tape);  
        } else {
            y = (int)parseEntry(tape);
            if (**tape == '/') {
                (*tape)++;
                z = (int)parseEntry(tape);
            } else {
                z = 0; // no vertex normal
            }
        }
    }
}

void _objLoader::debug()
{
    cout << "Vertices: " << vertices.size() << "\n";
    cout << "Normals: " << normals.size() << "\n";
    cout << "TexCoords: " << texCords.size() << "\n";
    cout << "Triangles: " << faces3.size() << "\n";
    cout << "Quads: " << faces4.size() << "\n";
    cout << "Model Size: " << modelSize << "B\n";
    if (megaDebug) {
        cout << "----- FULL MODEL DATA DUMP -----\n";
        for (int i = 0; i < vertices.size(); i++) {
            cout << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << endl;
        }
        for (int i = 0; i < texCords.size(); i++) {
            cout << "vt " << texCords[i].u << " " << texCords[i].v << endl;
        }
        for (int i = 0; i < normals.size(); i++) {
            cout << "vn " << normals[i].nx << " " << normals[i].ny << " " << normals[i].nz << endl;
        }
        for (int i = 0; i < faces3.size(); i++) {
            cout << faces3[i].v1 << "/" << faces3[i].t1 << "/" << faces3[i].n1 << " "
                 << faces3[i].v2 << "/" << faces3[i].t2 << "/" << faces3[i].n2 << " "
                 << faces3[i].v3 << "/" << faces3[i].t3 << "/" << faces3[i].n3 << "\n";
        }
        for (int i = 0; i < faces4.size(); i++) {
            cout << faces4[i].v1 << "/" << faces4[i].t1 << "/" << faces4[i].n1 << " "
                 << faces4[i].v2 << "/" << faces4[i].t2 << "/" << faces4[i].n2 << " "
                 << faces4[i].v3 << "/" << faces4[i].t3 << "/" << faces4[i].n3 << " "
                 << faces4[i].v4 << "/" << faces4[i].t4 << "/" << faces4[i].n4 << "\n";
        }
        cout << "----- FULL MODEL DATA DUMP FINISHED -----\n";
    }
}

void _objLoader::logDebug(ObjDebugType type)
{
    ofstream logFile("logs/obj_debug_log.txt");
    if (!logFile.is_open()) {
        cerr << "Failed to open log file for writing.\n";
        return;
    }
    logFile << "# OBJ Debug Log\n";
    logFile << "# Generated by _objLoader\n\n";
    logFile << "# File size: " << modelSize << "B\n";
    logFile << "# Vertices: " << vertices.size() << "\n";
    logFile << "# Normals: " << normals.size() << "\n";
    logFile << "# TexCoords: " << texCords.size() << "\n";
    logFile << "# Triangles: " << faces3.size() << "\n";
    logFile << "# Quads: " << faces4.size() << "\n\n";
    switch (type) {
        case OBJ_VERTEX:
            for (const auto& vertex : vertices) {
                logFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
            }
            break;
        case OBJ_NORMAL:
            for (const auto& normal : normals) {
                logFile << "vn " << normal.nx << " " << normal.ny << " " << normal.nz << "\n";
            }
            break;
        case OBJ_TEXCOORD:
            for (const auto& texCoord : texCords) {
                logFile << "vt " << texCoord.u << " " << texCoord.v << "\n";
            }
            break;
        case OBJ_FACE3:
            for (const auto& face : faces3) {
                logFile << face.v1 << "/" << face.t1 << "/" << face.n1 << " "
                        << face.v2 << "/" << face.t2 << "/" << face.n2 << " "
                        << face.v3 << "/" << face.t3 << "/" << face.n3 << "\n";
            }
            break;
        case OBJ_FACE4:
            for (const auto& face : faces4) {
                logFile << face.v1 << "/" << face.t1 << "/" << face.n1 << " "
                        << face.v2 << "/" << face.t2 << "/" << face.n2 << " "
                        << face.v3 << "/" << face.t3 << "/" << face.n3 << " "
                        << face.v4 << "/" << face.t4 << "/" << face.n4 << "\n";
            }
            break;
        case OBJ_ALL:
            for (const auto& vertex : vertices) {
                logFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
            }
            for (const auto& texCoord : texCords) {
                logFile << "vt " << texCoord.u << " " << texCoord.v << "\n";
            }
            for (const auto& normal : normals) {
                logFile << "vn " << normal.nx << " " << normal.ny << " " << normal.nz << "\n";
            }
            for (const auto& face : faces3) {
                logFile << face.v1 << "/" << face.t1 << "/" << face.n1 << " "
                        << face.v2 << "/" << face.t2 << "/" << face.n2 << " "
                        << face.v3 << "/" << face.t3 << "/" << face.n3 << "\n";
            }
            for (const auto& face : faces4) {
                logFile << face.v1 << "/" << face.t1 << "/" << face.n1 << " "
                        << face.v2 << "/" << face.t2 << "/" << face.n2 << " "
                        << face.v3 << "/" << face.t3 << "/" << face.n3 << " "
                        << face.v4 << "/" << face.t4 << "/" << face.n4 << "\n";
            }
            break;
        default:
            break;
    }
}
