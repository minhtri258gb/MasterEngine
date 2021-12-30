#define __MT_CAMERA_CPP__

#include "common.h"
#include "engine/Input.h"
#include "engine/Timer.h"
#include "Graphic.h"
#include "Camera.h"

using namespace std;
using namespace glm;
using namespace mt::engine;
using namespace mt::graphic;

Camera::Camera()
{
	this->right = vec3(1,0,0);
	this->up = vec3(0,1,0);
	this->forward = vec3(0,0,-1);
	this->position = vec3();
	this->velocity = vec3();
	this->angle.y = 1;
}

Camera::~Camera()
{
}

void Camera::update()
{
	bool updateView = false;
	if (Input::ins.getCursorX() || Input::ins.getCursorY())
	{
		// Delta cursor
		double xoffset = Input::ins.getCursorX();
		double yoffset = Input::ins.getCursorY();

		// Update quaternion
		float angleQ = xoffset * this->sensitivity;
		this->angle = cross(this->angle, angleAxis(angleQ, vec3(0,1,0)));

		angleQ = yoffset * sensitivity;
		quat tmp = cross(angleAxis(angleQ, vec3(1,0,0)), this->angle);
		if(tmp.x * tmp.x + tmp.z * tmp.z < 0.48f)
			this->angle = tmp;

		normalize(this->angle);

		// Update matrix view
		this->forward = vec3(	2.0f * (this->angle.x*this->angle.z - this->angle.y*this->angle.w),
						2.0f * (this->angle.y*this->angle.z + this->angle.x*this->angle.w),
						1.0f - 2.0f * (this->angle.x*this->angle.x + this->angle.y*this->angle.y));

		this->right = cross(this->forward, vec3(0,1,0));
		normalize(this->right);

		updateView = true;
		
		// Update frustum culling
		// this->frustumCulling.update();
	}

	if (Input::ins.checkHold(87))
		this->velocity = this->movement(this->velocity, this->forward,  5.0f, 10.0f, true);
	if (Input::ins.checkHold(83))
		this->velocity = this->movement(this->velocity, this->forward, -5.0f, 10.0f, true);
	if (Input::ins.checkHold(65))
		this->velocity = this->movement(this->velocity, this->right, -5.0f, 10.0f, true);
	if (Input::ins.checkHold(68))
		this->velocity = this->movement(this->velocity, this->right,  5.0f, 10.0f, true);
	
	if (this->velocity.x || this->velocity.y || this->velocity.z)
	{
		// origin = body->getOrigin();

		// g_pView->setPosition(vec3(origin.x, origin.y + 40.0f, origin.z)); // 24 + 40 = 64 / 68

		// vecFront = g_pView->getVeclook();

		// this->right = cross(this->forward, vec3(0,1,0));
		// normalize(this->right);

		// Velocity
		// if(abs(this->velocity.x) < 0.1f && abs(this->velocity.y) < 0.1f && abs(this->velocity.z) < 0.1f)
		// 	this->velocity = vec3();
		// else
		// 	this->velocity *= 0.01f;//0.8f;

		this->position += this->velocity * Timer::ins.getTimePassed();
		this->velocity = vec3();

		updateView = true;
	}
	
	if (updateView)
	{
		Graphic::ins.scene.view = lookAt(this->position, this->position + this->forward, vec3(0,1,0));
		Graphic::ins.shaderProgramMgr.setSceneView(Graphic::ins.scene.view);
	}
}

vec3 Camera::movement(vec3 velocity, vec3 vecdir, float speed, float max_speed, bool yMovement)
{
	vec3 newVelocity = vecdir;

	if (!yMovement)
	{
		newVelocity.y = 0;
		normalize(newVelocity);
	}

	newVelocity *= speed;

	newVelocity.x += velocity.x;
	newVelocity.z += velocity.z;

	if (yMovement)
		newVelocity.y += velocity.y;
	else
		newVelocity.y = velocity.y;

	float lenghtspeed = newVelocity.length();
	if (lenghtspeed > max_speed)
	{
		newVelocity *= (max_speed / lenghtspeed);
	}
	return newVelocity;
}
