
// User definable variables
float FOV = 70;
float MaxFPS = 144;
float CameraSpeed = 0.5f;
float CameraSensitivity = 0.05;

#include <GL/glew.h>
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr

#include <iostream>
#include "Main.h"
#include "Shaders.h"
#include "Camera.h"
#include "Log.h"
#include "Textures.h"
#include "ModelLoad.h"
#include "ProceduralGeneration.h"
#include "ObjectData.h"
#include "QuadLoader.h"
#include "ModelObject.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int ResX = 1280;
int ResY = 720;
float SkyColor[] = {138,220,255}; // must be float
float UnderwaterColor[] = {0,73,135};
int waveHeightLimit = 400;
int waveMovementSpeed = 160; // Steps a second
float sandNodes[] = {10,10};

using namespace std;
using namespace glm;

// Object declaration
Camera mainCamera;

// Time keeping
float minFrameTime = 1 / MaxFPS;
static int displayInterval;
int currentWindowWidth;
int currentWindowHeight;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Wave motion
int curWaveHeight = 0;
bool waveFlip = false;
float lastWaveMovement = 0;
float waveUpdateFreq = 1.0f / (float)waveMovementSpeed;

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    mainCamera.MouseInput(window, xpos, ypos);
}

int main()
{
    // GLFW loading
    log("Starting program.");
    if (!glfwInit()) {
        log("GLFW failed to start.", LogType::Fatal);
        glfwTerminate();
        return -1;
    }
    else {
        log("GLFW started successfully.");
    }

    // Window creation
    GLFWwindow* mainWindow = glfwCreateWindow(ResX, ResY, "OpenGL Demonstration by Tobey Foxcroft", NULL, NULL);
    glfwMakeContextCurrent(mainWindow);

    // GLEW loading
    log("Attempting to initialise GLEW.");
    GLenum errorCode = glewInit();
    if (errorCode != GLEW_OK) {
        stringstream temp;
        temp << '"' << glewGetErrorString(errorCode) << '"';
        log("GLEW failed to start. Provided error: " + temp.str(), LogType::Fatal);
        glfwTerminate();
        return -1;
    }
    else {
        log("GLEW started successfully.");
    }

    // Shader definition
    ShaderInfo shaders[] =
    {
        { GL_VERTEX_SHADER, "resources/shaders/base.vert" },
        { GL_FRAGMENT_SHADER, "resources/shaders/base.frag" },
        { GL_NONE, NULL }
    };

    // Shader loading
    program = LoadShaders(shaders);
    glUseProgram(program);
    log("Shaders successfully loaded.");

    int mvpVar = glGetUniformLocation(program, "mvpIn");
    int filterVar = glGetUniformLocation(program, "applyFilter");
    int isSand = glGetUniformLocation(program, "isSand"); // For disabling texture coordinates


    // Window settings
    glViewport(0, 0, ResX, ResY);
    glfwSetFramebufferSizeCallback(mainWindow, framebuffer_resize_callback);
    glfwSetCursorPosCallback(mainWindow, mouse_callback);
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Ship model
    ModelObject Ship;
    Ship.data = loadOBJ("resources/objects/ship.obj");
    Ship.ApplyTexture("resources/textures/metal.png");

    Ship.projection = scale(Ship.projection, vec3(0.1f, 0.1f, 0.1f));
    Ship.projection = translate(Ship.projection, vec3(0.0f, 0.5f, 0.0f));


    // Sand model
    ModelObject Sand;
    Sand.data = GetQuad();
    Sand.ApplyTexture("resources/textures/sand.png");

    Sand.projection = rotate(Sand.projection, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)); // Do not use offset or it affects camera
    Sand.projection = translate(Sand.projection, vec3(0.0f, 0.0f, -1.0f));
    Sand.projection = scale(Sand.projection, vec3(2.0f, 2.0f, 2.0f));

    // Water model
    ModelObject Water;
    Water.data = GenerateTerrain(sandNodes[0], sandNodes[1]);
    Water.ApplyTexture("resources/textures/water.png");

    Water.projection = translate(Water.projection, vec3(-3.0f, 0.0f, -3.0f));
    Water.projection = scale(Water.projection, vec3(0.9f, 1.0f, 0.9f));

    reference_wrapper<ModelObject> ModelsToRender[] = { Ship, Sand, Water };

    // Camera setup
    mainCamera.SetSpeed(CameraSpeed);
    mainCamera.SetSensitivity(CameraSensitivity);
    mainCamera.SetPosition(vec3(0.0f, 0.1f, 1.0f));
    mainCamera.projection = perspective(radians(FOV), (float)ResX / (float)ResY, 0.01f, 100.0f);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    log("Starting render loop.");
    while (!glfwWindowShouldClose(mainWindow) && mainWindow)
    {
        // Time management
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (mainCamera.GetPosition().y >= 0) {
            glClearColor(SkyColor[0] / 255, SkyColor[1] / 255, SkyColor[2] / 255, 1);
            glUniform1i(filterVar, 0);
        } else {
            glClearColor(UnderwaterColor[0] / 255, UnderwaterColor[1] / 255, UnderwaterColor[2] / 255, 1);
            glUniform1i(filterVar, 1);
        }

        if (currentFrame > lastWaveMovement + waveUpdateFreq) {
            lastWaveMovement = currentFrame;
            if (waveFlip) {
                if (curWaveHeight < waveHeightLimit) {
                    curWaveHeight++;
                    Water.projection = translate(Water.projection, vec3(0.0f, -0.0001f, -0.0001f));
                }
                else {
                    waveFlip = !waveFlip;
                }
            } else {
                if (curWaveHeight > 0) {
                    curWaveHeight--;
                    Water.projection = translate(Water.projection, vec3(0.0f, 0.0001f, 0.0001f));
                }
                else {
                    waveFlip = !waveFlip;
                }
            }
        }

        //Input keyboard data
        mainCamera.InputData(mainWindow,deltaTime);

        //Rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        mat4 projection = mainCamera.projection;
        mat4 view = mainCamera.GetViewMatrix();

        for (auto& modelRef : ModelsToRender) {
            ModelObject& currentObject = modelRef.get();
            mat4 mvp = projection * view * currentObject.projection;
            glUniformMatrix4fv(mvpVar, 1, GL_FALSE, value_ptr(mvp));

            glBindTexture(GL_TEXTURE_2D, currentObject.GetTexture());
            glBindVertexArray(currentObject.data.vao);
            glDrawElements(GL_TRIANGLES, currentObject.data.vertexCount, GL_UNSIGNED_INT, 0);
        }


        //Refreshing
        glfwSwapBuffers(mainWindow); //Swaps the colour buffer
        glfwPollEvents(); //Queries all GLFW events

        while (glfwGetTime() - currentFrame < minFrameTime){} // FPS limiting, slightly inefficient but only one that worked
    }


    log("Render loop exiting.");
    glfwTerminate();
    return 0;
}

