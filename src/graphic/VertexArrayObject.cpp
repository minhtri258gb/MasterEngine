#define __VERTEX_ARRAY_OBJECT_CPP__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "VertexArrayObject.h"

using namespace mt::engine;
using namespace mt::graphic;

VertexArrayObject::VertexArrayObject()
{
	this->gl_VAO = -1;
	this->EBO = nullptr;
}

VertexArrayObject::~VertexArrayObject()
{
	for (VertexBufferObject* VBO : this->VBOs)
		delete VBO;
	VBOs.clear();

	if (this->EBO)
		delete this->EBO;
	
	glDeleteVertexArrays(1, &this->gl_VAO);
}

void VertexArrayObject::init()
{
	glGenVertexArrays(1, &this->gl_VAO);
}

void VertexArrayObject::addAttribute(std::vector<glm::vec2> data)
{
	int location = VBOs.size();
	VertexBufferObject* VBO = new VertexBufferObject();
	VBO->init(location, data);
	VBOs.push_back(VBO);
}

void VertexArrayObject::addAttribute(std::vector<glm::vec3> data)
{
	int location = VBOs.size();
	VertexBufferObject* VBO = new VertexBufferObject();
	VBO->init(location, data);
	VBOs.push_back(VBO);
}

void VertexArrayObject::addAttribute(std::vector<glm::vec4> data)
{
	int location = VBOs.size();
	VertexBufferObject* VBO = new VertexBufferObject();
	VBO->init(location, data);
	VBOs.push_back(VBO);
}

void VertexArrayObject::addIndices(std::vector<unsigned int> indices)
{
	this->count = indices.size();

	this->EBO = new ElementBufferObject();
	this->EBO->init(indices);
}

void VertexArrayObject::bind()
{
	glBindVertexArray(this->gl_VAO);
}

void VertexArrayObject::unbind()
{
	glBindVertexArray(0);
}

void VertexArrayObject::drawTriangle()
{
	glDrawArrays(GL_TRIANGLES, 0, this->count);
}

void VertexArrayObject::drawElementTriangle()
{
	glDrawElements(GL_TRIANGLES, this->count, GL_UNSIGNED_INT, nullptr);
}

int VertexArrayObject::getCount()
{
	return this->count;
}

void VertexArrayObject::setCount(int _count)
{
	this->count = _count;
}
