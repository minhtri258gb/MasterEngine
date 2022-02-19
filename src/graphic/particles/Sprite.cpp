#define __MT_SPRITE_CPP__

#include "common.h"
#include "../Graphic.h"
#include "Sprite.h"
#include "../VertexArrayObject.h"
#include "../Texture.h"

using namespace std;
using namespace mt::graphic;

ShaderProgram Sprite::shader;

class Sprite::SpriteImpl
{
public:
	VertexArrayObject VAO;
	Texture texture;
};

Sprite::Sprite()
{
	// Implement
	this->impl = new SpriteImpl();
}

Sprite::~Sprite()
{
	// Implement
	delete this->impl;
}

void Sprite::load(string name)
{
	// Fake data
	vector<vec3> vertices;
	vertices.push_back(vec3(0.0f, 0.0f, 0.0f));
	vertices.push_back(vec3(0.0f, 1.0f, 0.0f));
	vertices.push_back(vec3(1.0f, 0.0f, 0.0f));
	vertices.push_back(vec3(1.0f, 1.0f, 0.0f));

	// Init VAO
	this->impl->VAO.init();
	this->impl->VAO.bind();

	this->impl->VAO.addAttribute(vertices);
	// this->impl->VAO.addDynamicAttribute(1024, vec3());

	this->impl->VAO.unbind();
}

void Sprite::update()
{
}

void Sprite::render()
{
	this->shader.use();

	Graphic::ins.cullFaceToogle(false);

	this->impl->VAO.bind();
	// this->impl->VAO.drawPointInstance();
	this->impl->VAO.drawTriangleTrip();
	this->impl->VAO.unbind();
	
	Graphic::ins.cullFaceToogle(true);

}
