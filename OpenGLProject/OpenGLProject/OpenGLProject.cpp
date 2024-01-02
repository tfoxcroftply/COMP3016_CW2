#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/common.hpp>
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr

#include <Windows.h>
#include "Camera.h"
#include <iostream>
#include <ctime>

#include "Shaders.h"
#include "Log.h"

GLuint program;
using namespace std;
using namespace glm;

// User defined variables
int ResX = 1280;
int ResY = 720;
int desiredFramerate = 60;
float backgroundRGB[3] = { 138, 220, 255 };

// Program globals
enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 1 };
GLuint VAOs[NumVAOs];
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
GLuint Buffers[NumBuffers];

GLFWwindow* mainWindow;
Camera mainCamera;

void framebufferResize(GLFWwindow *mainWindow, int width, int height) {
    mainCamera.SetResolution(width, height);
    float AspectRatio = float(width) / float(height);
    glMatrixMode(GL_PROJECTION);
    if (width > height) {
        glViewport(0, (height - width) / 2, width, width);
    } else {
        glViewport((width - height) / 2, 0, height, height);
    }
}

void renderFrame() {

    // Scene initialisation
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mainCamera.InputData(mainWindow);


    // Rendering
    glBindVertexArray(VAOs[0]); //Bind buffer object to render
    glDrawArrays(GL_TRIANGLES, 0, 3); //Render buffer object


    // Refreshing
    glfwSwapBuffers(mainWindow);
    glfwPollEvents();
}



int main(int argc, char *argv[])
{
    log("Starting program");
    if (!glfwInit()) {
        log("GLFW failed to start.", LogType::Fatal);
        glfwTerminate();
        return -1;
    } else {
        log("GLFW started successfully.");
    }

    log("Creating window.");
    mainWindow = glfwCreateWindow(ResX, ResY, "OpenGL Scene by Tobey Foxcroft", NULL, NULL);
    glfwMakeContextCurrent(mainWindow);
    glfwSetFramebufferSizeCallback(mainWindow, framebufferResize);

    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
    };

    log("Attempting to initialise GLEW.");
    GLenum errorCode = glewInit();
    if (errorCode != GLEW_OK) {
        stringstream temp;
        temp << '"' << glewGetErrorString(errorCode) << '"';
        log("GLEW failed to start. Provided error: " + temp.str(), LogType::Fatal);
        glfwTerminate();
        return -1;
    } else {
        log("GLEW started successfully.");
    }   

    ShaderInfo shaders[] =
    {
        { GL_VERTEX_SHADER, "resources/shaders/base.vert" },
        { GL_FRAGMENT_SHADER, "resources/shaders/base.frag" },
        { GL_NONE, NULL }
    };

    program = LoadShaders(shaders);
    glUseProgram(program);

    log("Preparing window");
    glClearColor(backgroundRGB[0] / 255, backgroundRGB[1] / 255, backgroundRGB[2] / 255, 1);
    framebufferResize(mainWindow, ResX, ResY);
    mainCamera.SetResolution(1280, 720);

    glGenVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[0]);
    glGenBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (glfwWindowShouldClose(mainWindow) or !mainWindow) {
        log("Window did not load correctly.", LogType::Error);
    } else {
        log("Window successfully initialised.");
    }

    log("Starting render loop.");
    while (!glfwWindowShouldClose(mainWindow) or !mainWindow) { 
        if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            log("User requested program termination.");
            glfwSetWindowShouldClose(mainWindow, true);
        }
        renderFrame();
        Sleep(1000 / desiredFramerate);
    }
    log("Program terminating.");
    glfwTerminate();
    log("Program terminated.");
}