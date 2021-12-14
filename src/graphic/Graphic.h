#ifndef __MT_GRAPHIC_H__
#define __MT_GRAPHIC_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "models/ModelMgr.h"

namespace mt {
namespace graphic {

class Graphic {

public:

	// Variable
	static Graphic ins;
	ModelMgr modelMgr;

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

private:

	// Variable
	GLFWwindow* gl_window;

};

}}

#endif
