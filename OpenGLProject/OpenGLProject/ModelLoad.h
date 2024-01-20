#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "ObjectData.h"
#include "Log.h"

using namespace std;

struct Vertex { // simple structure for holding vertex/texcoords data
    float position[3];
    float texCoord[2];
};

ObjectData loadOBJ(const char* path) {
    vector<float> temp_vertices; // temporary vertex positions
    vector<float> temp_texCoords; // temporary texture coordinates
    vector<Vertex> vertices; // final vertex data
    vector<unsigned int> indices; // final indices
    ifstream file(path);
    string line;

    if (!file) {
        log("Model failed to load. Path: '" + string(path) + "'", LogType::Error);
    }

    while (getline(file, line)) { // loop for every line in the obj file
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
            std::string vertexInfo[3];
            unsigned int vIndex[3], tIndex[3];

            for (int i = 0; i < 3; i++) {
                iss >> vertexInfo[i];
                
                if (sscanf(vertexInfo[i].c_str(), "%d/%d", &vIndex[i], &tIndex[i]) != 2) {
                    log("Face data is in an unknown format.", LogType::Error);
                }

                vIndex[i]--; // vertex 
                tIndex[i]--; // texture coord

                Vertex vertex; // remake the vertex from scratch
                vertex.position[0] = temp_vertices[3 * vIndex[i]]; // in the format of (v,v,v, tc,tc) like the quad
                vertex.position[1] = temp_vertices[3 * vIndex[i] + 1];
                vertex.position[2] = temp_vertices[3 * vIndex[i] + 2];
                vertex.texCoord[0] = temp_texCoords[2 * tIndex[i]];
                vertex.texCoord[1] = temp_texCoords[2 * tIndex[i] + 1];

                vertices.push_back(vertex); // put the now complete vertex into data
                indices.push_back(indices.size());
            }
        }
    }

    GLuint vao, vbo, ebo; // Do the same thing as the other scripts
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

    return { vao, static_cast<GLsizei>(indices.size()) }; // return vao and size for drawing
}