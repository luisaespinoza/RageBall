#include "_textureloader.h"
// #include <_textureloader.h>
_textureLoader::_textureLoader()
{
    //ctor
    width = 0;
    height = 0;
}

_textureLoader::~_textureLoader()
{
    //dtor
}

GLuint _textureLoader::loadTexture(char* fileName)
{
    glGenTextures(1,&textID);
    glBindTexture(GL_TEXTURE_2D,textID);

    image = SOIL_load_image(fileName,&width,&height,0,SOIL_LOAD_RGBA);
//    if(!image)cout<<"Error : *******file did not load *****"<<endl;
    if (!image) {
        std::cout << "Error: couldn't load " << fileName << std::endl;
        glBindTexture(GL_TEXTURE_2D, 0);
        textID = 0;                      // mark invalid
        return 0;
    }
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    return textID;
}

void _textureLoader::bindTexture()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,textID);
}
