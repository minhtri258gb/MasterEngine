#ifndef __MT_MODEL_H__
#define __MT_MODEL_H__

namespace mt {
namespace graphic {

class Model
{

public:

	// Variables
	glm::vec3 position;
	glm::quat angle;
	glm::vec3 scale;

	// Forwards
	Model();
	virtual ~Model();
	virtual void render();

	// Natives

};

}}

#endif
