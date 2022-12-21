#ifndef CAMERA
#include "camera.hpp"
#define CAMERA
#endif

Camera::Camera(int viewportWidth, int viewportHeight)
{
	this->aspectRatio = viewportWidth / viewportHeight;
}

glm::mat4 Camera::getView() const
{
	return glm::lookAt(this->position, this->position + this->direction, this->up);
}

glm::mat4 Camera::getViewProjection() const
{
	glm::mat4 projectionMatrix = glm::perspective(this->fieldOfView, this->aspectRatio, this->zNear, this->zFar);
	return projectionMatrix * this->getView();
}

void Camera::moveFoward(float amount)
{
	this->position += glm::normalize(this->direction) * amount * this->speed;
}

void Camera::moveRight(float amount)
{
	glm::vec3 right = glm::normalize(glm::cross(this->direction, this->up));
	this->position += right * amount * speed;
}

void Camera::moveUp(float amount)
{
	this->position += glm::normalize(this->up) * amount * this->speed;
}

void Camera::moveMouseAxis(glm::vec2 amount, glm::vec3* anchor, bool useX)
{
	glm::vec3 right = glm::normalize(glm::cross(this->direction, this->up));
	glm::vec3 position = *anchor + (glm::normalize(this->up) * glm::vec3{ amount, 0.0f } *this->speed * -1.0f);
	
	if (useX)
	{
		position += (right * glm::vec3{ amount, 0.0f } *this->speed * -1.0f);
	}

	this->position = position;
}


void Camera::look(glm::vec2 amount)
{
	const glm::mat4 identityMatrix = glm::identity<glm::mat4>();
	glm::mat4 yawRotation = glm::rotate(identityMatrix, glm::radians(amount.x), this->up);

	const glm::vec3 right = glm::normalize(glm::cross(this->DEFAULT_DIRECTION , this->up));
	glm::mat4 pitchRotation = glm::rotate(identityMatrix, glm::radians(amount.y), right);
	this->direction = yawRotation * pitchRotation * glm::vec4{ this->DEFAULT_DIRECTION, 0.0f };
}

void Camera::roll(float amount)
{
	const glm::mat4 identityMatrix = glm::identity<glm::mat4>();
	glm::mat4 rollRotation = glm::rotate(identityMatrix, glm::radians(amount), {0.0f, 0.0f, 1.0f});

	this->up = rollRotation * glm::vec4{ this->up, 0.0f };
}

void Camera::reset()
{
	this->direction = DEFAULT_DIRECTION;
	this->position = DEFAULT_POSITION;
	this->up = DEFAULT_UP;
}