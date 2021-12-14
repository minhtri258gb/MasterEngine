#ifndef __MT_CAMERA_H__
#define __MT_CAMERA_H__

namespace mt {
namespace graphic {

class Camera
{

public:

	// Variable
	glm::mat4 matProj, matView;
	glm::vec3 position;
	glm::vec3 forward, right, up;

	// Forward
	Camera();
	~Camera();
	void update();

	// Native

};

}}

#endif
