
// Library and C++ built in header loading

#include <GL/glew.h> // OpenGL dependencies
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Main.h" // Provided in labs
#include "Shaders.h" // Provided in labs
#include "Camera.h" // My own work
#include "Log.h" // My own work
#include "Textures.h" // My own work
#include "ModelLoad.h" // My own work
#include "ProceduralGeneration.h" // My own work
#include "ObjectData.h" // My own work
#include "QuadLoader.h" // Derived from lab 5
#include "ModelObject.h" // My own work

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Provided in labs


// User definable varialbes
float FOV = 70;
float MaxFPS = 144;
float CameraSpeed = 0.5f;
float CameraSensitivity = 0.05;
int ResX = 1280;
int ResY = 720;
float SkyColor[] = { 138,220,255 }; // must be float
float UnderwaterColor[] = { 0,73,135 };
int waveHeightLimit = 400;
int waveMovementSpeed = 160; // Steps a second
int rotLimit = 100; // steps
int rotSpeed = 100; // Steps a second
float sandNodes[] = { 10,10 }; // X,Y


// Namespaces
using namespace std;
using namespace glm;

// Object declaration
Camera mainCamera;

// Time tracking
float minFrameTime = 1 / MaxFPS;
static int displayInterval;
int currentWindowWidth;
int currentWindowHeight;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Wave motion tracking
int curWaveHeight = 0;
bool waveFlip = false;
float lastWaveMovement = 0;
float waveUpdateFreq = 1.0f / (float)waveMovementSpeed;

// Ship wobble tracking
float curShipRot = 0;
bool rotFlip = false;
float lastRotMovement = 0;
float rotUpdateFreq = 1.0f / (float)rotSpeed;

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

    // Window settings
    glViewport(0, 0, ResX, ResY); // set resolution and callbacks
    glfwSetFramebufferSizeCallback(mainWindow, framebuffer_resize_callback);
    glfwSetCursorPosCallback(mainWindow, mouse_callback);
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // prevents mouse from escaping window


    // Shader definition
    ShaderInfo shaders[] = // Info that gets passed to the shader
    {
        { GL_VERTEX_SHADER, "resources/shaders/base.vert" },
        { GL_FRAGMENT_SHADER, "resources/shaders/base.frag" },
        { GL_NONE, NULL }
    };

    // Shader loading
    program = LoadShaders(shaders);

    if (program == 0) {
        log("Shader loading failed, terminating.", LogType::Fatal);
        glfwTerminate();
        return -1;
    }

    glUseProgram(program);

    // Shader parameters
    int mvpVar = glGetUniformLocation(program, "mvpIn");
    int filterVar = glGetUniformLocation(program, "applyFilter"); // Underwater filter
    int isSand = glGetUniformLocation(program, "isSand"); // For disabling texture coordinates, possibly use later
    int secondaryTextureEnabled = glGetUniformLocation(program, "mixTextures"); // For enabling texture mixing, i dont want to mix textures that dont exist

    GLint textureLocation = glGetUniformLocation(program, "textureIn"); // retrieve texture input locations to reference it underneath
    GLint textureMixLocation = glGetUniformLocation(program, "textureMix");

    glUniform1i(textureLocation, 0); // Assign it to the right GL_TEXTURE
    glUniform1i(textureMixLocation, 1);

    // Ship model
    ModelObject Ship; // Declare a new ship object
    Ship.data = loadOBJ("resources/objects/ship.obj"); // Give it some data from obj loader
    Ship.ApplyTexture("resources/textures/metal.png"); // Give it a texture id for future referencing

    Ship.projection = scale(Ship.projection, vec3(0.1f, 0.1f, 0.1f)); // Set some positions for it
    Ship.projection = translate(Ship.projection, vec3(0.0f, 0.5f, 0.0f));

    // Ship model. Above repeated for the rest of the models
    ModelObject Ball;
    Ball.data = loadOBJ("resources/objects/ball.obj");
    Ball.ApplyTexture("resources/textures/plastic.png");

    Ball.projection = translate(Ball.projection, vec3(1.0f, 0.04f, 1.0f));
    Ball.projection = scale(Ball.projection, vec3(0.04f, 0.04f, 0.04f));


    // Buoy model
    ModelObject BuoyRing;
    BuoyRing.data = loadOBJ("resources/objects/lifebuoy.obj");
    BuoyRing.ApplyTexture("resources/textures/plastic.png");

    BuoyRing.projection = translate(BuoyRing.projection, vec3(-1.0f, 0.06f, 1.0f));
    BuoyRing.projection = scale(BuoyRing.projection, vec3(0.05f, 0.05f, 0.05f));

    // Quad sample. I wasn't sure if a standalone quad was required
    ModelObject Sand;
    Sand.data = GetQuad(); // Instead of loading a texture, use the quad header to get the data
    Sand.ApplyTexture("resources/textures/sand.png");

    Sand.projection = rotate(Sand.projection, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    Sand.projection = translate(Sand.projection, vec3(0.0f, 0.0f, -1.0f));
    Sand.projection = scale(Sand.projection, vec3(1.0f,1.0f,1.0f));

    // Water model
    ModelObject Water;
    Water.data = GenerateTerrain(sandNodes[0], sandNodes[1]);
    Water.ApplyTexture("resources/textures/water.png");
    Water.ApplySecondaryTexture("resources/textures/ripples.png"); // Use the secondary texture function so the object now holds two textures

    Water.projection = translate(Water.projection, vec3(-3.0f, 0.0f, -3.0f));
    Water.projection = scale(Water.projection, vec3(0.9f, 1.0f, 0.9f));

    reference_wrapper<ModelObject> ModelsToRender[] = { Ship, Sand, Water, Ball, BuoyRing }; // Using references otherwise copies are made and results in movement requests changing nothing

    // Camera setup
    mainCamera.SetSpeed(CameraSpeed); // Get some variables from the top and send it to the main camera object
    mainCamera.SetSensitivity(CameraSensitivity); 
    mainCamera.SetPosition(vec3(0.0f, 0.1f, 1.0f)); // Starting position
    mainCamera.projection = perspective(radians(FOV), (float)ResX / (float)ResY, 0.01f, 100.0f); // Starting projection. Needs FOV, resolution and render distances

    glDisable(GL_CULL_FACE); // Used these to prevent glitching, possibly unrequired but keeping as everything works
    glEnable(GL_DEPTH_TEST);

    log("Starting render loop.");
    while (!glfwWindowShouldClose(mainWindow) && mainWindow) // Forever do the render loop unless the main window disappears or user has requested to close
    {
        // Time management
        float currentFrame = glfwGetTime(); // Calculate current times and time taken
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        // Events
        if (mainCamera.GetPosition().y >= 0) { // Conditional statement to change the overlay color for being underwater
            glClearColor(SkyColor[0] / 255, SkyColor[1] / 255, SkyColor[2] / 255, 1); // Change backdrop to normal sky
            glUniform1i(filterVar, 0); // 0 is false
        } else {
            glClearColor(UnderwaterColor[0] / 255, UnderwaterColor[1] / 255, UnderwaterColor[2] / 255, 1); // Change backdrop to be darker
            glUniform1i(filterVar, 1); // 1 is true, so turn on water filter in fragment shader
        }

        if (currentFrame > lastWaveMovement + waveUpdateFreq) { // This tracks whether the calculations should run or not. i dont want it to be dependent on framerate
            lastWaveMovement = currentFrame; // update the tracker for next loop
            if (waveFlip) {
                if (curWaveHeight < waveHeightLimit) { // if under the limit ..
                    curWaveHeight++; // then continue raising wave
                    Water.projection = translate(Water.projection, vec3(0.0f, -0.0001f, -0.0001f));
                    Ball.projection = translate(Ball.projection, vec3(0.0f, -0.001f, 0.0f));
                    BuoyRing.projection = translate(BuoyRing.projection, vec3(0.0f, -0.001f, 0.0f));
                }
                else {
                    waveFlip = !waveFlip; // otherwise tell it to go the other direction
                }
            }
            else {
                if (curWaveHeight > 0) { // Same here but for the other side and direction
                    curWaveHeight--;
                    Water.projection = translate(Water.projection, vec3(0.0f, 0.0001f, 0.0001f));
                    Ball.projection = translate(Ball.projection, vec3(0.0f, 0.001f, 0.0f));
                    BuoyRing.projection = translate(BuoyRing.projection, vec3(0.0f, 0.001f, 0.0f));
                }
                else {
                    waveFlip = !waveFlip;
                }
            }
        }

        if (currentFrame > lastRotMovement + rotUpdateFreq) { // Similar method for ship rotation, instead of height its rotation
            lastRotMovement = currentFrame;
            if (rotFlip) {
                if (curShipRot < (float)rotLimit) {
                    curShipRot++;
                    Ship.projection = rotate(Ship.projection, radians(0.01f), vec3(0.0f,0.0f,1.0f));
                }
                else {
                    rotFlip = !rotFlip;
                }
            }
            else {
                if (curShipRot > -(float)rotLimit) {
                    curShipRot--;
                    Ship.projection = rotate(Ship.projection, radians(-0.01f), vec3(0.0f, 0.0f, 1.0f));
                }
                else {
                    rotFlip = !rotFlip;
                }
            }
        }

        //Input keyboard data
        mainCamera.InputData(mainWindow,deltaTime);

        // Rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 projection = mainCamera.projection;
        mat4 view = mainCamera.GetViewMatrix();

        for (auto& modelRef : ModelsToRender) { // For every model reference, render it
            ModelObject& currentObject = modelRef.get(); // get up to date model from reference
            mat4 mvp = projection * view * currentObject.projection; // get the new mvp
            glUniformMatrix4fv(mvpVar, 1, GL_FALSE, value_ptr(mvp)); // send the mvp to the shader

            unsigned int MixTexture = currentObject.GetSecondaryTexture();// attempt to load secondary texture


            glUniform1i(secondaryTextureEnabled, 0); // Reset just in case
            glActiveTexture(GL_TEXTURE0); // Select the main texture input for fragment shader
            glBindTexture(GL_TEXTURE_2D, currentObject.GetTexture()); // And then apply it

            if (MixTexture != -1) { // If the secondary texture exists then do the same but for GL_TEXTURE1
                glUniform1i(secondaryTextureEnabled, 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, currentObject.GetSecondaryTexture());
            }

            glBindVertexArray(currentObject.data.vao); // grab the assigned vao
            glDrawElements(GL_TRIANGLES, currentObject.data.vertexCount, GL_UNSIGNED_INT, 0); // and then draw it
        }


        //Refreshing
        glfwSwapBuffers(mainWindow); //Swaps the colour buffer
        glfwPollEvents(); //Queries all GLFW events

        while (glfwGetTime() - currentFrame < minFrameTime){} // FPS limiting, slightly inefficient but only one that worked
    }


    log("Render loop exiting.");
    glfwTerminate(); // ensures that everything finishes
    return 0;
}

