#define __MT_GRAPHIC_CPP__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "engine/Config.h"
#include "engine/Input.h"
#include "Graphic.h"

using namespace std;
using namespace mt::engine;
using namespace mt::graphic;

Graphic Graphic::ins;

class Graphic::GraphicImpl
{
public:
	GLFWwindow* gl_window;
};

void cbk_framebuffer_size(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void cbk_cursor_pos(GLFWwindow *window, double xpos, double ypos)
{
	Input::ins.cursorPos(xpos, ypos);
}

void cbk_key(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// cout << key << endl;

	if(key == -1)
		return;
	if(action == GLFW_PRESS)
		Input::ins.keyPress(key, true);
	else if(action == GLFW_RELEASE)
		Input::ins.keyPress(key, false);
}

void cbk_mouse_button(GLFWwindow *window, int button, int action, int mods)
{
	bool press;
	switch (action) {
	case GLFW_PRESS:
		press = true;
		break;
	case GLFW_RELEASE:
		press = false;
		break;
	default:
		return;
	}

	if(button > 2)
		return;

	Input::ins.keyPress(351 + button, press);
}

Graphic::Graphic()
{
	// Impliment
	this->impl = new GraphicImpl();
}

Graphic::~Graphic()
{
	// Impliment
	delete this->impl;
}

void Graphic::init()
{
	// Data
	int width = Config::ins.windowWidth;
	int height = Config::ins.windowHeight;
	string name = Config::ins.windowName;

	// Init OpenGL
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	this->impl->gl_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	if (this->impl->gl_window == NULL)
	{
		glfwTerminate();
		throw error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(this->impl->gl_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw error("Failed to initialize GLAD");
	}

	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(this->impl->gl_window, cbk_framebuffer_size);
	glfwSetCursorPosCallback(this->impl->gl_window, cbk_cursor_pos);
	glfwSetKeyCallback(this->impl->gl_window, cbk_key);
	glfwSetMouseButtonCallback(this->impl->gl_window, cbk_mouse_button);

	glfwSetInputMode(this->impl->gl_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(this->impl->gl_window, GLFW_STICKY_KEYS, GLFW_TRUE);

	// Set default
	this->setDepthTest();

	// Init component
	this->shaderProgramMgr.init();

	// Init Scene
	this->scene.init();

	// Init Screen
	this->screen.init();

	// Init Model
	this->modelMgr.init();

	// #EXTRA
}

void Graphic::close()
{
	// Clear
	this->modelMgr.clear();

	// Close component
	this->shaderProgramMgr.close();

	// #EXTRA

	// Close GLFW
	glfwTerminate();
}

void Graphic::processInput()
{
	if(glfwGetKey(this->impl->gl_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->impl->gl_window, true);
}

void Graphic::renderPre()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphic::renderPost()
{
	Input::ins.resetStatus(); // end frame key press
	glfwPollEvents(); // start frame key press
	glfwSwapBuffers(this->impl->gl_window);
}

bool Graphic::checkWindow()
{
	return !glfwWindowShouldClose(this->impl->gl_window);
}

double Graphic::getTime()
{
	return glfwGetTime();
}

void Graphic::setDepthTest(bool _value)
{
	if (_value)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}
