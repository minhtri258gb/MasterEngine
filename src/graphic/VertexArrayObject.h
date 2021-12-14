#ifndef __MT_VERTEX_ARRAY_OBJECT_H__
#define __MT_VERTEX_ARRAY_OBJECT_H__

#include "VertexBufferObject.h"
#include "ElementBufferObject.h"

namespace mt {
namespace graphic {

class VertexArrayObject
{

public:

	// Variable

	// Forward
	VertexArrayObject();
	~VertexArrayObject();
	void init();

	void addAttribute(std::vector<glm::vec2> data);
	void addAttribute(std::vector<glm::vec3> data);
	void addAttribute(std::vector<glm::vec4> data);
	void addIndices(std::vector<unsigned int> indices);

	// Native
	void bind();
	void unbind();

	void drawTriangle();
	void drawElementTriangle();
	
	int getCount();
	void setCount(int count);

private:

	// Variable
	unsigned int gl_VAO;
	std::vector<VertexBufferObject*> VBOs;
	ElementBufferObject *EBO;
	int count;

};

}}

#endif
