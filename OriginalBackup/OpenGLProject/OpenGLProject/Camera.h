#pragma once
#include <freeglut.h>

class Camera {
private:
	float x = 0;
	float y = 0;
	float z = 0;
	float speed = 10;
public:
	Camera() {};
	void SetPosition(float x, float y, float z);
	void GetPosition();
	void OnKeyboard(unsigned char key);
	void SetSpeed(float speed);
};

void Camera::SetPosition(float x, float y, float z) {
}

void Camera::GetPosition() {

}

void Camera::SetSpeed(float speed) {

}

void Camera::OnKeyboard(unsigned char key) {

}