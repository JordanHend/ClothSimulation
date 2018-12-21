#ifndef GLCAMERA_H
#define GLCAMERA_H

#include <glad/glad.h>
#include <glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>


#define PI 3.141593657
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 30.5f;
const float SENSITIVITY = 0.1f;
const float JOY_SENSITIVITY = 1.0f;
const float ZOOM = 45.0f;


const int FIRSTPERSONMODE = 0;
const int THIRDPERSONMODE = 1;


glm::vec3 getRight(glm::vec3 vector);



class GLCamera
{
public:

	glm::vec3 * positionToFollow;
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float JoystickSensitivity;
	float Zoom;
	float depth = 20.0f;
	float Theta = 0, Phi = 0;
	float cursorDistance = 50;
	glm::vec3 getFront();

	glm::vec3 getRight();
	int mode = FIRSTPERSONMODE;
	GLCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		Right = { -1.0f, 0.0f, 0.0f };
		JoystickSensitivity = JOY_SENSITIVITY;
		updateCameraVectors();
	
	};
	GLCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		Right = { -1.0f, 0.0f, 0.0f };
		JoystickSensitivity = JOY_SENSITIVITY;
		updateCameraVectors();
	};
	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);
	void ProcessJoystickMovement(float xDir, float yDir, float Velocity, float deltaTime);
	void update();



private:
	void updateCameraVectors();
	glm::vec3 sphericalToCartesian(float thet, float phi);

};

#endif