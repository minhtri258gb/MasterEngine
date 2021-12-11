#ifndef __VERTEX_BUFFER_OBJECT_H__
#define __VERTEX_BUFFER_OBJECT_H__

namespace mt {
namespace graphic {

class VertexBufferObject
{

public:

	// Variable
	unsigned int gl_VBO;

	// Forward
	VertexBufferObject();
	~VertexBufferObject();
	void init(int location, std::vector<glm::vec2> vertices);
	void init(int location, std::vector<glm::vec3> vertices);
	void init(int location, std::vector<glm::vec4> vertices);
	void close();

	// Native
	void bind();
	void unbind();

};

}}

#endif
