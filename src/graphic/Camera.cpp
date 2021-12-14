#define __MT_CAMERA_CPP__

#include "common.h"
#include "Camera.h"

using namespace mt::engine;
using namespace mt::graphic;

Camera::Camera()
{
	this->matProj = glm::mat4(1.0f);
	this->matView = glm::mat4(1.0f);
	this->position = glm::vec3();
	this->forward = glm::vec3();
	this->right = glm::vec3();
	this->up = glm::vec3();
}

Camera::~Camera()
{
}

void Camera::update()
{
}
