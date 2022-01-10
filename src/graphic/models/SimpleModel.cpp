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
	matModel = translate(matModel, this->position);
	// matModel = rotate(matModel, radians(45.0f+(a*50.f)), vec3(0.0, 0.0, 1.0));
	// matModel = rotate(matModel, radians(45.0f+(a*20.f)), vec3(0.0, 1.0, 0.0));
	// matModel = rotate(matModel, radians(45.0f+(a*30.f)), vec3(1.0, 0.0, 0.0));
	matModel *= mat4_cast(this->angle);
	matModel = glm::scale(matModel, this->scale);
	shader.setUnifrom(2, matModel);

	// Texture
	this->texture.bind();

	// VAO
	this->VAO.bind();
	// VAO.renderTriangle();
	this->VAO.drawElementTriangle();
	this->VAO.unbind();
}
