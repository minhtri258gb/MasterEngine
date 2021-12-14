#ifndef __MT_SIMPLE_MODEL_H__
#define __MT_SIMPLE_MODEL_H__

#include "Model.h"
#include "../ShaderProgram.h"
#include "../VertexArrayObject.h"
#include "../Texture.h"

namespace mt {
namespace graphic {

class SimpleModel : public Model
{

public:

	// Variable
	static ShaderProgram shader;
	friend class ModelBuilder;

	// Forward
	SimpleModel();
	~SimpleModel();
	void render();

	// Native

private:

	// Variable
	VertexArrayObject VAO;
	Texture texture;

};

}}

#endif
