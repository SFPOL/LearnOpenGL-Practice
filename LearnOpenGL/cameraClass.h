#include <iostream>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:

	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 cameraDirection;

	glm::vec3 up;
	glm::vec3 cameraRight;

	glm::vec3 cameraUp;

	float yaw;
	float pitch;

	float FOV = 45.0f;

	Camera(
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f), 
		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f)
	) {
		cameraPos = pos;
		cameraTarget = target;
		cameraDirection = glm::normalize(cameraTarget - cameraPos);

		up = upVector;
		cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);

		yaw = glm::degrees(glm::atan(cameraDirection.z, cameraDirection.x));
		pitch = glm::degrees(glm::asin(-cameraDirection.y));
	}

	glm::mat4 getLookAt() {
		cameraTarget = cameraPos + cameraDirection;
		return glm::lookAt(cameraPos, cameraTarget, cameraUp);
	}

	glm::mat4 getPerspective(float aspectRatio) {
		return glm::perspective(glm::radians(FOV), aspectRatio, 0.1f, 100.0f);
	}

	void moveFrontBack(float delta) {
		cameraPos += cameraDirection * delta;
	}

	void moveLeftRight(float delta) {
		cameraPos += cameraRight * delta;
	}

	void moveUpDown(float delta) {
		cameraPos += cameraUp * delta;
	}

	void moveDirection(float offsetYaw, float offsetPitch) {

		yaw += offsetYaw;
		pitch += offsetPitch;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		
		cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection.y = sin(glm::radians(pitch));
		cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		cameraDirection = glm::normalize(cameraDirection);

		cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);
	}

	void changeFOV(float offset) {
		FOV -= offset;
		if (FOV < 1.0f)
			FOV = 1.0f;
		if (FOV > 120.0f)
			FOV = 120.0f;
	}	

private:
};