#ifndef __MT_GRAPHIC_H__
#define __MT_GRAPHIC_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgramMgr.h"
#include "models/ModelMgr.h"
#include "Scene.h"
#include "Screen.h"

namespace mt {
namespace graphic {

class Graphic {

public:

	// Variable
	static Graphic ins;

	ShaderProgramMgr shaderProgramMgr;
	ModelMgr modelMgr;
	Scene scene;
	Screen screen;

	// Forward
	Graphic();
	~Graphic();
	void init();
	void close();

	void processInput();
	void renderPre();
	void renderPost();

	// Native
	bool checkWindow();
	void setDepthTest(bool value = true);

	// Stock

private:

	// Variable
	GLFWwindow* gl_window;

};

}}

#endif
