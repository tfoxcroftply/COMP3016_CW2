#pragma once

#include <GL/glew.h>
#include "Textures.h"
#include "Log.h"
#include "ObjectData.h"

#include <string>

using namespace std;


class ModelObject {
private:
    unsigned int texture;

public:
    ObjectData data;
    mat4 projection = mat4(1.0f);

    ModelObject(){};

    void ApplyTexture(string Input) {
        texture = GenerateTexture(Input);
        if (texture == -1) {
            log("Texture failed to load. Path: '" + Input + "'", LogType::Error);
        }
    }

    unsigned int GetTexture() {
        return texture;
    }
};