
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


    // Window settings
    glViewport(0, 0, ResX, ResY);
    glfwSetFramebufferSizeCallback(mainWindow, framebuffer_resize_callback);
    glfwSetCursorPosCallback(mainWindow, mouse_callback);
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ObjectData Quad = GetQuad();
    ObjectData Ship = loadOBJ("resources/objects/ship.obj");
    ObjectData Terrain = GenerateTerrain(sandNodes[0],sandNodes[1]);

    glEnable(GL_DEPTH_TEST);

    //Texture loading
    unsigned int waterTexture = GenerateTexture("resources/textures/water.png");
    unsigned int metalTexture = GenerateTexture("resources/textures/metal.png");
    unsigned int sandTexture = GenerateTexture("resources/textures/sand.png");


    //Model matrix
    mat4 model = mat4(1.0f);
    model = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)); // Do not use offset or it affects camera
    model = translate(model, vec3(0.0f, 0.0f, -1.0f));
    model = scale(model, vec3(2.0f, 2.0f, 2.0f));

    mat4 model2 = mat4(1.0f);
    model2 = scale(model2, vec3(0.1f, 0.1f, 0.1f));
    model2 = translate(model2, vec3(0.0f, 0.5f, 0.0f));

    //Model matrix
    mat4 model3 = mat4(1.0f);
    model3 = translate(model3, vec3(-3.0f, 0.0f, -3.0f));
    model3 = scale(model3, vec3(0.9f, 1.0f, 0.9f));

    // Camera setup
    mainCamera.SetSpeed(CameraSpeed);
    mainCamera.SetSensitivity(CameraSensitivity);
    mainCamera.SetPosition(vec3(0.0f, 0.1f, 1.0f));
    mainCamera.projection = perspective(radians(FOV), (float)ResX / (float)ResY, 0.01f, 100.0f);




    glDisable(GL_CULL_FACE);

    int mvpVar = glGetUniformLocation(program, "mvpIn");
    int filterVar = glGetUniformLocation(program, "applyFilter");
    int isSand = glGetUniformLocation(program, "isSand"); // For disabling texture coordinates

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
                    model3 = translate(model3, vec3(0.0f, -0.0001f, -0.0001f));
                }
                else {
                    waveFlip = !waveFlip;
                }
            } else {
                if (curWaveHeight > 0) {
                    curWaveHeight--;
                    model3 = translate(model3, vec3(0.0f, 0.0001f, 0.0001f));
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



        //Transformations
        mat4 view = mainCamera.GetViewMatrix(); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
        mat4 mvp = mainCamera.projection * view * model;
        glUniformMatrix4fv(mvpVar, 1, GL_FALSE, value_ptr(mvp));

        //Drawing quad
        glBindTexture(GL_TEXTURE_2D, sandTexture);
        glBindVertexArray(Quad.vao);
        glDrawElements(GL_TRIANGLES, Quad.vertexCount, GL_UNSIGNED_INT, 0);



        //Transformations
        mat4 view2 = mainCamera.GetViewMatrix(); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
        mat4 mvp2 = mainCamera.projection * view * model2;
        glUniformMatrix4fv(mvpVar, 1, GL_FALSE, value_ptr(mvp2));

        glBindTexture(GL_TEXTURE_2D, metalTexture);
        glBindVertexArray(Ship.vao);
        glDrawElements(GL_TRIANGLES, Ship.vertexCount, GL_UNSIGNED_INT, 0);


        //Transformations
        mat4 view3 = mainCamera.GetViewMatrix(); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
        mat4 mvp3 = mainCamera.projection * view * model3;
        glUniformMatrix4fv(mvpVar, 1, GL_FALSE, value_ptr(mvp3));


        //Drawing quad
        glBindTexture(GL_TEXTURE_2D, waterTexture);
        glBindVertexArray(Terrain.vao);
        glDrawElements(GL_TRIANGLES, Terrain.vertexCount, GL_UNSIGNED_INT, 0);





        //Refreshing
        glfwSwapBuffers(mainWindow); //Swaps the colour buffer
        glfwPollEvents(); //Queries all GLFW events

        while (glfwGetTime() - currentFrame < minFrameTime){} // FPS limiting, slightly inefficient but only one that worked
    }


    log("Render loop exiting.");
    glfwTerminate();
    return 0;
}

