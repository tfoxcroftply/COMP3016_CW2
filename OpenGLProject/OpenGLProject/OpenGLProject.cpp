#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;

void error(string error) {
    glfwTerminate();
    system("cls");
    cout << "Program has been terminated due to an error" << endl;
    if (error != "" && &error != nullptr) {
        cout << "An error reason has been provided by the program: " << error << endl;
    }
}

int main()
{
    cout << "Starting program\n";

    glfwInit();
    GLFWwindow* mainWindow = glfwCreateWindow(1280, 720, "OpenGL Scene by Tobey Foxcroft", NULL, NULL);

    if (mainWindow == NULL) {
        error("GLFW did not start correctly");
        return -1;
    }

    glfwMakeContextCurrent(mainWindow);
    glewInit(); 
    glViewport(0, 0, 1280, 720);

}