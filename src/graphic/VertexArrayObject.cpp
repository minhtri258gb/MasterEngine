#define __MT_VERTEX_ARRAY_OBJECT_CPP__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "VertexArrayObject.h"

using namespace std;
using namespace mt;
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

void VertexArrayObject::addAttribute(vector<vec2> data)
{
	int location = VBOs.size();
	VertexBufferObject* VBO = new VertexBufferObject();
	VBO->init(location, data);
	VBOs.push_back(VBO);
}

void VertexArrayObject::addAttribute(vector<vec3> data)
{
	int location = VBOs.size();
	VertexBufferObject* VBO = new VertexBufferObject();
	VBO->init(location, data);
	VBOs.push_back(VBO);
}

void VertexArrayObject::addAttribute(vector<vec4> data)
{
	int location = VBOs.size();
	VertexBufferObject* VBO = new VertexBufferObject();
	VBO->init(location, data);
	VBOs.push_back(VBO);
}

void VertexArrayObject::addAttribute(vector<vec4i> data)
{
	int location = VBOs.size();
	VertexBufferObject* VBO = new VertexBufferObject();
	VBO->init(location, data);
	VBOs.push_back(VBO);
}

void VertexArrayObject::addDynamicAttribute(int maxInstance, vec3 type)
{
	int location = VBOs.size();
	VertexBufferObject* VBO = new VertexBufferObject();
	VBO->initDynamic(location, maxInstance, type);
	VBOs.push_back(VBO);
}

void VertexArrayObject::addIndices(vector<unsigned int> indices)
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

void VertexArrayObject::drawPointInstance()
{
	glDrawArraysInstanced(GL_POINT, 0, 1, 10);
}

void VertexArrayObject::drawTriangle()
{
	glDrawArrays(GL_TRIANGLES, 0, this->count);
}

void VertexArrayObject::drawElementTriangle()
{
	glDrawElements(GL_TRIANGLES, this->count, GL_UNSIGNED_INT, nullptr);
}

void VertexArrayObject::drawTriangleTrip()
{
	glDrawArrays(GL_TRIANGLE_STRIP, 0, this->count);
}

int VertexArrayObject::getCount()
{
	return this->count;
}

void VertexArrayObject::setCount(int _count)
{
	this->count = _count;
}
