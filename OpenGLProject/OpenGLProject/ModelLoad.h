#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

struct Vertex {
    float position[3];
    float texCoord[2];
};

GLuint& loadOBJ(const char* path, unsigned int& numElements) {
    std::vector<float> temp_vertices; // Temporary vertex positions
    std::vector<float> temp_texCoords; // Temporary texture coordinates
    std::vector<Vertex> vertices; // Final vertex data
    std::vector<unsigned int> indices;
    std::ifstream file(path);
    std::string line;

    GLuint vao;

    if (!file) {
        std::cerr << "Object did not load properly." << std::endl;
    }

    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") { // v in obj means vertices data
            float x, y, z;
            iss >> x >> y >> z;
            temp_vertices.insert(temp_vertices.end(), { x, y, z });
        }
        else if (prefix == "vt") { // vt in obj means texture coordinates
            float u, v;
            iss >> u >> v;
            temp_texCoords.insert(temp_texCoords.end(), { u, v });
        }
        else if (prefix == "f") { // f in obj means faces
            std::string vertex1, vertex2, vertex3;
            iss >> vertex1 >> vertex2 >> vertex3;
            unsigned int vIndex[3], tIndex[3];
            sscanf(vertex1.c_str(), "%d/%d", &vIndex[0], &tIndex[0]);
            sscanf(vertex2.c_str(), "%d/%d", &vIndex[1], &tIndex[1]);
            sscanf(vertex3.c_str(), "%d/%d", &vIndex[2], &tIndex[2]);


            for (int i = 0; i < 3; ++i) {
                vIndex[i] -= 1;
                tIndex[i] -= 1;

                Vertex vertex;
                vertex.position[0] = temp_vertices[3 * vIndex[i]];
                vertex.position[1] = temp_vertices[3 * vIndex[i] + 1];
                vertex.position[2] = temp_vertices[3 * vIndex[i] + 2];
                vertex.texCoord[0] = temp_texCoords[2 * tIndex[i]];
                vertex.texCoord[1] = temp_texCoords[2 * tIndex[i] + 1];

                vertices.push_back(vertex);
                indices.push_back(indices.size());
            }
        }
    }

    numElements = vertices.size();

    cout << glGetError();

    GLuint vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vao;
}