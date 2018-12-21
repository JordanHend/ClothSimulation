#include "GLCamera.h"




glm::vec3 GLCamera::getFront()
{


	return Front;	
}

glm::vec3 GLCamera::getRight()
{

	return Right;
}

glm::mat4 GLCamera::GetViewMatrix()
{
	if(mode == FIRSTPERSONMODE)
	return glm::lookAt(Position, Position + Front, Up);
	else
		return glm::lookAt(Position, *positionToFollow, Up);
}
void GLCamera::update()
{
	if (mode == FIRSTPERSONMODE)
	{

	}
	else
	{
		Position = *positionToFollow + (Front) * (depth);
	}
	updateCameraVectors();
}
void GLCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	if (mode == FIRSTPERSONMODE)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}
}

void GLCamera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	if (mode == FIRSTPERSONMODE)
	{
		xoffset *= 0.1f;
		yoffset *= 0.1f;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		updateCameraVectors();

	}
	else
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Phi += xoffset;
		Theta += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Theta * PI / 180 > 9 * PI / 10)
				Theta = (9 * PI / 10) * 180 / PI;
			if (Theta * PI / 180 < PI / 4)
				Theta = (PI / 4) * 180 / PI;
			//	std::cout << "THETA: " << Theta << std::endl;
				//std::cout << "PHI: " << Phi << std::endl;
			if (Phi > 360.0f)
				Phi = 0.0f;
			if (Phi < 0)
				Phi = 360.0f;
		}
	}
	// Update Front, Right and Up Vectors using the updated Eular angles
	updateCameraVectors();
}

void GLCamera::ProcessMouseScroll(float yoffset)
{
	depth -= yoffset;
	
	cursorDistance -= yoffset;
	if (cursorDistance > 50 || cursorDistance < 5)
		cursorDistance += yoffset;

}

void GLCamera::ProcessJoystickMovement(float xDir, float yDir, float Velocity, float deltaTime)
{
	float xoffset = xDir * (Velocity * deltaTime) * JOY_SENSITIVITY;
	float yoffset = yDir * (Velocity * deltaTime) * JOY_SENSITIVITY;
	Phi += xoffset;
	Theta += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	
		if (Theta * PI / 180 > 9 * PI / 10)
			Theta = (9 * PI / 10) * 180 / PI;
		if (Theta * PI / 180 < PI / 4)
			Theta = (PI / 4) * 180 / PI;
		//	std::cout << "THETA: " << Theta << std::endl;
		//std::cout << "PHI: " << Phi << std::endl;
		if (Phi > 360.0f)
			Phi = 0.0f;
		if (Phi < 0)
			Phi = 360.0f;
	

	// Update Front, Right and Up Vectors using the updated Eular angles
	updateCameraVectors();
}

void GLCamera::updateCameraVectors()
{
	if (mode == FIRSTPERSONMODE)
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
	else
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front = sphericalToCartesian(Theta * PI / 180, Phi * PI / 180);
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = (glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		if (Right.x == 0 && Right.y == 0 && Right.z == 0)
		{
			;
		}
		else
		{
			Right = glm::normalize(Right);
		}
		Up = glm::normalize(glm::cross(Right, Front));
	}
}

glm::vec3 GLCamera::sphericalToCartesian(float thet, float phi)
{
	return glm::vec3(cos(phi) * (sin(thet)), cos(thet), sin(phi) * sin(thet));
}

glm::vec3 getRight(glm::vec3 vector)
{
	glm::vec3 right;
	right = (glm::cross(vector, glm::vec3(0.0f, 1.0f, 0.0f)));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	if (right.x == 0 && right.y == 0 && right.z == 0)
	{
		;
	}
	else
	{
		right = glm::normalize(right);
	}
	return right;
}
