#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>
#include <algorithm>

#include "Camera.h"

using namespace std;
using namespace glm;

void Camera::SetSpeed(float inputSpeed) { // sets camera speed
    speed = inputSpeed;
}

void Camera::SetSensitivity(float inputSens) { // sets camera sensitivity
    sensitivity = inputSens;
}

void Camera::InputData(GLFWwindow* inputWindow, float delta) { // reads all key data and applies neccessary transformations
    float tempSpeed = speed * delta;
    if (glfwGetKey(inputWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(inputWindow, true);
    }
    if (glfwGetKey(inputWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += tempSpeed * front;
    }
    if (glfwGetKey(inputWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        position -= tempSpeed * front;
    }
    if (glfwGetKey(inputWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        position -= normalize(cross(front, up)) * tempSpeed;
    }
    if (glfwGetKey(inputWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += normalize(cross(front, up)) * tempSpeed;
    }
    if (glfwGetKey(inputWindow, GLFW_KEY_Q) == GLFW_PRESS) {
        position -= tempSpeed * up;
    }
    if (glfwGetKey(inputWindow, GLFW_KEY_E) == GLFW_PRESS) {
        position += tempSpeed * up;
    }
};

mat4 Camera::GetViewMatrix() { // encapsulated view matrix
    mat4 view = lookAt(position, position + front, up);
    return view;
}

void Camera::MouseInput(GLFWwindow* inputWindow, double xpos, double ypos) { // parse mouse movements
    if (!mouseHasMoved)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        mouseHasMoved = true; // for setting the first movement as baseline
    }

    float xChange = xpos - lastMouseX; // apply changes
    float yChange = lastMouseY - ypos;
    lastMouseX = xpos;
    lastMouseY = ypos;

    xChange *= sensitivity; // sensitivity as multiplier
    yChange *= sensitivity;

    mouseYaw += xChange; // apply changes
    mousePitch += yChange;

    mousePitch = std::clamp(mousePitch, -89.9f, 89.9f); // must still use std:: for some reason. limits camera movement

    front = normalize(vec3( // sets the new aiming vector
        cos(radians(mouseYaw)) * cos(radians(mousePitch)), 
        sin(radians(mousePitch)), 
        sin(radians(mouseYaw)) * cos(radians(mousePitch))
));
}

vec3 Camera::GetPosition() { // encapsulated position
    return position;
}

void Camera::SetPosition(vec3 inputPos) { // sets encapsulated position
    position = inputPos;
}