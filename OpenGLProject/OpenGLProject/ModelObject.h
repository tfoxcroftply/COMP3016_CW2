#pragma once

#include <GL/glew.h>
#include "Textures.h"
#include "Log.h"
#include "ObjectData.h"

#include <string>

using namespace std;


class ModelObject {
private:
    unsigned int texture = -1;
    unsigned int textureMix = -1;

public:
    ObjectData data;
    mat4 projection = mat4(1.0f);

    ModelObject(){};

    void ApplyTexture(string Input) {
        texture = GenerateTexture(Input);
    }

    void ApplySecondaryTexture(string Input) {
        textureMix = GenerateTexture(Input);
    }

    unsigned int GetTexture() {
        return texture;
    }

    unsigned int GetSecondaryTexture() {
        return textureMix;
    }
};