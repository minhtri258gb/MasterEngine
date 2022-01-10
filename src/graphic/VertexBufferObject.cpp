#define __MT_VERTEX_BUFFER_OBJECT_CPP__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "VertexBufferObject.h"

using namespace mt::engine;
using namespace mt::graphic;

VertexBufferObject::VertexBufferObject()
{
	this->gl_VBO = -1;
}

VertexBufferObject::~VertexBufferObject()
{
	glDeleteBuffers(1, &this->gl_VBO);
}

void VertexBufferObject::init(int location, std::vector<glm::vec2> _vertices)
{
	glGenBuffers(1, &this->gl_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->gl_VBO);
	glBufferData(GL_ARRAY_BUFFER, 8 * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 8, (void*)0);
	glEnableVertexAttribArray(location);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferObject::init(int location, std::vector<glm::vec3> _vertices)
{
	glGenBuffers(1, &this->gl_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->gl_VBO);
	glBufferData(GL_ARRAY_BUFFER, 12 * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);
	glEnableVertexAttribArray(location);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferObject::init(int location, std::vector<glm::vec4> _vertices)
{
	glGenBuffers(1, &this->gl_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->gl_VBO);
	glBufferData(GL_ARRAY_BUFFER, 16 * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 16, (void*)0);
	glEnableVertexAttribArray(location);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferObject::init(int location, std::vector<glm::ivec4> _vertices)
{
	glGenBuffers(1, &this->gl_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->gl_VBO);
	glBufferData(GL_ARRAY_BUFFER, 16 * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

	glVertexAttribIPointer(location, 4, GL_INT, 16, (void*)0);
	glEnableVertexAttribArray(location);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferObject::close()
{
	// #TODO
}
