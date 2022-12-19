#ifndef CAMERA
#include "camera.hpp"
#define CAMERA
#endif

Camera::Camera(int viewportWidth, int viewportHeight)
{
	this->aspectRatio = viewportWidth / viewportHeight;
}

glm::mat4 Camera::getViewProjection() const
{
	glm::mat4 viewMatrix = glm::lookAt(this->position, this->position + this->direction, this->up);
	glm::mat4 projectionMatrix = glm::perspective(this->fieldOfView, this->aspectRatio, this->zNear, this->zFar);

	return projectionMatrix * viewMatrix;
}

void Camera::moveFoward(float amount)
{
	position += glm::normalize(direction) * amount * speed;
}

void Camera::moveRight(float amount)
{
	glm::vec3 right = glm::normalize(glm::cross(this->direction, this->up));
	position += right * amount * speed;
}

void Camera::look(glm::vec2 amount)
{
	const glm::mat4 identityMatrix = glm::identity<glm::mat4>();
	glm::mat4 yawRotation = glm::rotate(identityMatrix, glm::radians(amount.x), this->up);

	const glm::vec3 right = glm::normalize(glm::cross(this->DEFAULT_DIRECTION , this->up));
	glm::mat4 pitchRotation = glm::rotate(identityMatrix, glm::radians(amount.y), right);
	this->direction = yawRotation * pitchRotation * glm::vec4{ this->DEFAULT_DIRECTION, 1.0f };
}

void Camera::roll(float amount)
{
	const glm::mat4 identityMatrix = glm::identity<glm::mat4>();
	glm::mat4 rollRotation = glm::rotate(identityMatrix, glm::radians(amount), {0.0f, 0.0f, 1.0f});

	this->up = rollRotation * glm::vec4{ this->up, 0.0f };
}