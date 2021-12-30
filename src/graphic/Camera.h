#ifndef __MT_CAMERA_H__
#define __MT_CAMERA_H__

namespace mt {
namespace graphic {

class Camera
{

public:

	// Variable
	glm::vec3 forward, right, up;
	glm::vec3 position, velocity;
	glm::quat angle;
	const float sensitivity = 0.002f;

	// Forward
	Camera();
	~Camera();
	void update();

	// Native

private:

	// Variables
	double mx, my;

	// Stock
	glm::vec3 movement(glm::vec3 velocity, glm::vec3 vecdir, float speed, float max_speed, bool yMovement);

};

}}

#endif
