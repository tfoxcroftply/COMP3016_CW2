#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>

using namespace std;
using namespace glm;

class Camera {
private:
    // View
    vec3 position = vec3(0.0f, 0.0f, 3.0f);
    vec3 orientation = vec3(0.0f, 0.0f, 1.0f);
    vec3 front = vec3(0.0f, 0.0f, -1.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);

    // Variables
    float speed = 1.0f;
    float sensitivity = 0.1f;

    // Mouse
    float lastMouseX, lastMouseY;
    float mouseYaw, mousePitch;
    bool mouseHasMoved = false;
public:
    // Functions
    Camera(){}
    mat4 GetViewMatrix();
    void SetSpeed(float inputSpeed);
    void SetSensitivity(float inputSens);
    void InputData(GLFWwindow* inputWindow, float delta);
    void MouseInput(GLFWwindow* inputWindow, double xpos, double ypos);
    vec3 GetPosition();
    void SetPosition(vec3 inputPos);
};