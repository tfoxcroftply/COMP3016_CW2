#include <string>

#include <GL/glew.h>
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr

#include "stb_image.h"

#include "Log.h"

using namespace std;


int GenerateTexture(string FileName) {
    unsigned int texture; //Index
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    int width, height, colourChannels;
    unsigned char* data = stbi_load(FileName.c_str(), &width, &height, &colourChannels, 0);
    if (data) {
        log("Texture loaded successfully. Path '" + FileName + "'");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        return -1;
        log("Texture failed to load. Path: '" + FileName + "'", LogType::Error);
    }
    return texture;

    stbi_image_free(data);
}