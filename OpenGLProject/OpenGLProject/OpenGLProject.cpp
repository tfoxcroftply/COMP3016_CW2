
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int ResX = 1280;
int ResY = 720;
float SkyColor[3] = {138,220,255}; // must be float
float UnderwaterColor[3] = {0,71,135};

const int NumberOfObjects = 2;

using namespace std;
using namespace glm;

Camera mainCamera;

float minFrameTime = 1 / MaxFPS;
static int displayInterval;
int currentWindowWidth;
int currentWindowHeight;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

enum VAO_IDs { Triangles, Indices, Colours, Textures};
GLuint VAOs[NumberOfObjects];

//Buffer types
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };

//Buffer objects
GLuint Buffers[NumBuffers];

vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

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


    float vertices[] = {
        //Positions             //Textures
        3.0f, 3.0f, 0.0f,       1.0f, 1.0f, //top right
        3.0f, -3.0f, 0.0f,      1.0f, 0.0f, //bottom right
        -3.0f, -3.0f, 0.0f,     0.0f, 0.0f, //bottom left
        -3.0f, 3.0f, 0.0f,      0.0f, 1.0f  //top left
    };

    unsigned int indices[] = {
        0, 1, 3, //first triangle
        1, 2, 3 //second triangle
    };

    //Sets index of VAO
    glGenVertexArrays(NumberOfObjects, VAOs); //NumVAOs, VAOs
    //Binds VAO to a buffer
    glBindVertexArray(VAOs[0]); //VAOs[0]
    //Sets indexes of all required buffer objects
    glGenBuffers(NumBuffers, Buffers); //NumBuffers, Buffers
    //glGenBuffers(1, &EBO);

    //Binds vertex object to array buffer
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]); //Buffers[Triangles]
    //Allocates buffer memory for the vertices of the 'Triangles' buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Binding & allocation for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]); //Buffers[Indices]
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Allocation & indexing of vertex attribute memory for vertex shader
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Textures
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Unbinding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //Texturea
    unsigned int texture; //Index
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, colourChannels;
    unsigned char* data = stbi_load("resources/textures/water.jpg", &width, &height, &colourChannels, 0);
    if (data){
        log("Textures loaded successfully.");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        log("Textures failed to load.");
        return -1;
    }

    // Clear memory
    stbi_image_free(data);

    //Model matrix
    mat4 model = mat4(1.0f);
    model = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)); // Do not use offset or it affects camera

    // Camera setup
    mainCamera.SetSpeed(CameraSpeed);
    mainCamera.SetSensitivity(CameraSensitivity);
    mainCamera.SetPosition(vec3(0.0f, 0.1f, 0.0f));
    mat4 projection = perspective(radians(FOV), (float)ResX / (float)ResY, 0.01f, 100.0f);

    int waveHeightLimit = 100;
    int curWaveHeight = 0;
    bool waveFlip = false;
    float lastWaveMovement = 0;
    float waveUpdateFreq = 1.0f / 60;

    log("Starting render loop.");
    while (!glfwWindowShouldClose(mainWindow) && mainWindow)
    {
        //Time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (mainCamera.GetPosition().y >= 0) {
            glClearColor(SkyColor[0] / 255, SkyColor[1] / 255, SkyColor[2] / 255, 1);
        } else {
            glClearColor(UnderwaterColor[0] / 255, UnderwaterColor[1] / 255, UnderwaterColor[2] / 255, 1);
        }



        if (currentFrame > lastWaveMovement + waveUpdateFreq) {
            lastWaveMovement = currentFrame;
            if (waveFlip) {
                if (curWaveHeight < waveHeightLimit) {
                    curWaveHeight++;
                    model = translate(model, vec3(0.0f, -0.0001f, -0.0001f));
                }
                else {
                    waveFlip = !waveFlip;
                }
            } else {
                if (curWaveHeight > 0) {
                    curWaveHeight--;
                    model = translate(model, vec3(0.0f, 0.0001f, 0.0001f));
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
        mat4 mvp = projection * view * model;
        int mvpLoc = glGetUniformLocation(program, "mvpIn");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvp));

        //Drawing
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAOs[0]); //Bind buffer object to render; VAOs[0]
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Refreshing
        glfwSwapBuffers(mainWindow); //Swaps the colour buffer
        glfwPollEvents(); //Queries all GLFW events

        while (glfwGetTime() - currentFrame < minFrameTime){} // FPS limiting, slightly inefficient but only one that worked
    }


    log("Render loop exiting.");
    glfwTerminate();
    return 0;
}

