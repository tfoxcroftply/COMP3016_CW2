#pragma once

#include <vector>
#include <GL/glew.h>
#include "ObjectData.h"

ObjectData GetQuad() {
    float vertices[] = {
        //Positions             //Textures
        3.0f, 3.0f, 0.0f,       1.0f, 1.0f, //top right
        3.0f, -3.0f, 0.0f,      1.0f, 0.0f, //bottom right
        -3.0f, -3.0f, 0.0f,     0.0f, 0.0f, //bottom left
        -3.0f, 3.0f, 0.0f,      0.0f, 1.0f  //top left
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position coordinate
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return { vao, sizeof(indices) / sizeof(unsigned int) };
}

