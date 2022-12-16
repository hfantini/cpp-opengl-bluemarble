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