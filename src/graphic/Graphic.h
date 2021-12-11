#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace mt {
namespace graphic {

class Graphic {

public:

	// Variable
	static Graphic ins;
	GLFWwindow* gl_window;

	// Forward
	Graphic();
	~Graphic();
	void init();
	void close();

	// Native
	bool checkWindow();
	void processInput();
	void renderPre();
	void renderPost();

	// Stock

};

}}

#endif
