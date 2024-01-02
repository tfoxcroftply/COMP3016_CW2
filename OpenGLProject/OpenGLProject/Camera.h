#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>

using namespace std;

class Camera {
private:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	int width;
	int height;

	float speed = 1.0f;
	float sensitivity = 100.0f;

public:
	string ShaderId;
	void SetPosition() {
		return;
	}
	void GetPosition() {
		return;
	}
	void SetSpeed(float inputSpeed) {
		speed = inputSpeed;
	}
	void SetResolution(int inputWidth, int inputHeight);
	void Update(float FOV, float planeNear, float planeFar, const char* uniform);
	void InputData(GLFWwindow* inputWindow);
};

void Camera::SetResolution(int inputWidth, int inputHeight) {
	width = inputWidth;
	height = inputHeight;
}

void Camera::Update(float fov,float planeNear,float planeFar,const char* uniform) {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	if (height != 0) {
		float aspect = float(width / height);
		view = glm::lookAt(position, position + orientation, up);
		projection = glm::perspective(glm::radians(fov), aspect, planeNear, planeFar);

	
	}
}

void Camera::InputData(GLFWwindow* inputWindow) {
	if (glfwGetKey(inputWindow, GLFW_KEY_W) == GLFW_PRESS) {
		position += speed * orientation;
		cout << "Moving";
	}
	if (glfwGetKey(inputWindow, GLFW_KEY_A) == GLFW_PRESS) {
		position += speed * -glm::normalize(glm::cross(orientation,up));
	}
	if (glfwGetKey(inputWindow, GLFW_KEY_S) == GLFW_PRESS) {
		position += speed * -orientation;
	}
	if (glfwGetKey(inputWindow, GLFW_KEY_D) == GLFW_PRESS) {
		position += speed * glm::normalize(glm::cross(orientation, up));
	}
};