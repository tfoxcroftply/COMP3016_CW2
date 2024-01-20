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
            vertices.push_back((float)x);                                            ///// generates in the same vertex + coords format as the quad
            vertices.push_back(GetRandomHeight() * Intensity); // height of terrain
            vertices.push_back((float)y);

            vertices.push_back((float)x / (width - 1)); // tex coords
            vertices.push_back((float)y / (height - 1));
        }
    }

    for (int row = 0; row < width - 1; ++row) {
        // Initialize the starting indices for the row
        int startTopLeftIndex = row * height;
        int startBottomLeftIndex = (row + 1) * height;

        for (int column = 0; column < height - 1; ++column) {
            // Calculate indices based on the starting indices
            int topLeftIndex = startTopLeftIndex + column;
            int topRightIndex = topLeftIndex + 1;
            int bottomLeftIndex = startBottomLeftIndex + column;
            int bottomRightIndex = bottomLeftIndex + 1;

            indices.push_back(topLeftIndex); // first triangle
            indices.push_back(bottomLeftIndex);
            indices.push_back(topRightIndex);

            indices.push_back(topRightIndex); // second triangle
            indices.push_back(bottomLeftIndex);
            indices.push_back(bottomRightIndex);
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
