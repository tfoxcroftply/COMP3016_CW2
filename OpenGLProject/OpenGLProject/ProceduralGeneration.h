#pragma once

#include <GL/glew.h>
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr
#include "ObjectData.h"
#include <random>

using namespace std;

float Intensity = 0.1f;
float Scale = 2;

float GetRandomHeight() {
    static mt19937 randomEngine(std::random_device{}());
    static uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(randomEngine);
}

ObjectData GenerateTerrain(int width, int height) {
    vector<float> vertices;
    vector<unsigned int> indices;

    // Generate vertices
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            vertices.push_back((float)x);
            vertices.push_back(GetRandomHeight() * Intensity); // height of terrain
            vertices.push_back((float)y);

            vertices.push_back((float)x / (width - 1));
            vertices.push_back((float)y / (height - 1));
        }
    }

    for (int x = 0; x < width - 1; ++x) {
        for (int y = 0; y < height - 1; ++y) {
            int topLeft = x * height + y;
            int topRight = topLeft + 1;
            int bottomLeft = (x + 1) * height + y;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    return { vao, static_cast<GLsizei>(indices.size()) };
}
