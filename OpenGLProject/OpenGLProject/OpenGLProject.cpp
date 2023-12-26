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

using namespace std;

int ResX = 1280;
int ResY = 720;
int desiredFramerate = 60;

float backgroundRGB[3] = { 51,102,255 };

enum class LogType { General, Error, Fatal };

void log(std::string Input, LogType InputLogType = LogType::General) {
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm* localTime = std::localtime(&currentTime);

    std::stringstream ss;

    // Format hours, minutes, seconds
    ss << std::put_time(localTime, "%T");

    // Get milliseconds
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // Append milliseconds to the stringstream
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    string Placeholder = "";
    if (InputLogType == LogType::Error) {
        Placeholder = "FATAL: ";
    }

    cout << "[" << ss.str() << "]: " << Placeholder << Input << endl;
}

GLFWwindow* mainWindow;
Camera mainCamera;

float vertices[] = {
    0.0,0.5,0.0,
    -0.5,-0.5,0.0,
    0.5,-0.5,0.0
};

void framebufferResize(GLFWwindow *mainWindow, int width, int height) {

    mainCamera.SetResolution(width, height);

    /*float AspectRatio = float(width) / float(height);

    glMatrixMode(GL_PROJECTION);
    if (width > height) {
        glViewport(0, (height - width) / 2, width, width);
    } else {
        glViewport((width - height) / 2, 0, height, height);
    }*/
}

void renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT);

    mainCamera.InputData(mainWindow);
    mainCamera.Update(70.0f, 0.1f, 100.0f, "camMatrix");



    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);

    glfwSwapBuffers(mainWindow);
    glfwPollEvents();

}



int main(int argc, char *argv[])
{
    log("Starting program");

    if (!glfwInit()) {
        log("Glfw failed to start",LogType::Fatal);
        glfwTerminate();
;       return -1;
    }

    mainWindow = glfwCreateWindow(ResX, ResY, "OpenGL Scene by Tobey Foxcroft", NULL, NULL);

    glfwMakeContextCurrent(mainWindow);
    glfwSetFramebufferSizeCallback(mainWindow, framebufferResize);

    GLenum errorCode = glewInit();
    if (errorCode != GLEW_OK) {
        log("GLEW failed to start",LogType::Error);
        cout << glewGetErrorString(errorCode);
        glfwTerminate();
        return -1;
    }   

    glClearColor(backgroundRGB[0] / 255, backgroundRGB[1] / 255, backgroundRGB[2] / 255, 1);
    framebufferResize(mainWindow, ResX, ResY);
    mainCamera.SetResolution(1280, 720);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    while (!glfwWindowShouldClose(mainWindow) || !mainWindow) {  
        renderFrame();
        Sleep(1000 / desiredFramerate);
    }
    glfwTerminate();
}