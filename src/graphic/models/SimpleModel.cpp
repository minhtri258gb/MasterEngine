#define __MT_SIMPLE_MODEL_CPP__

#include "common.h"
#include "SimpleModel.h"

using namespace glm;
using namespace mt::graphic;

ShaderProgram SimpleModel::shader;

SimpleModel::SimpleModel()
{
}

SimpleModel::~SimpleModel()
{
}

void SimpleModel::render()
{
	// Shader
	this->shader.use();

	// Model mattrix
	mat4 matModel = mat4(1.0f);
	matModel = translate(matModel, vec3(0.2f, 0.2f, 0.0f));
	matModel = rotate(matModel, radians(45.0f), vec3(0.0, 0.0, 1.0));
	matModel = scale(matModel, vec3(0.5, 0.5, 0.5));
	shader.setUnifrom(2, matModel);

	// Texture
	this->texture.bind();

	// VAO
	this->VAO.bind();
	// VAO.renderTriangle();
	this->VAO.drawElementTriangle();
	this->VAO.unbind();
}
