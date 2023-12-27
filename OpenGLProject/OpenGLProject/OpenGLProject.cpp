#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <Windows.h>
#include "Camera.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <format>
#include <sstream>
#include <fstream>
#include "Shaders.h"
#include "Log.h"

GLuint program;
using namespace std;

// User defined variables
int ResX = 1280;
int ResY = 720;
int desiredFramerate = 60;
float backgroundRGB[3] = { 138, 220, 255 };

// Program globals


// Rendering globals
//VAO vertex attribute positions in correspondence to vertex attribute type
enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 1 };
//VAOs
GLuint VAOs[NumVAOs];

//Buffer types
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
//Buffer objects
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
    glClear(GL_COLOR_BUFFER_BIT);


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
    -0.5f, -0.5f, 0.0f, //pos 0 | x, y, z
    0.5f, -0.5f, 0.0f, //pos 1
    0.0f, 0.5f, 0.0f //pos 2
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

    //Sets index of VAO
    glGenVertexArrays(NumVAOs, VAOs);
    //Binds VAO to a buffer
    glBindVertexArray(VAOs[0]);
    //Sets indexes of all required buffer objects
    glGenBuffers(NumBuffers, Buffers);

    //Binds vertex object to array buffer
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
    //Allocates buffer memory for the vertices of the 'Triangles' buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Allocates vertex attribute memory for vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //Index of vertex attribute for vertex shader
    glEnableVertexAttribArray(0);

    //Unbinding
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
        //Sleep(1000 / desiredFramerate);
    }
    log("Program terminating.");
    glfwTerminate();
    log("Program terminated.");
    

}