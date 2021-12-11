#define __GRAPHIC_CPP__

#include "common.h"
#include "Graphic.h"

using namespace mt::engine;
using namespace mt::graphic;

Graphic Graphic::ins;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Graphic::Graphic()
{

}

Graphic::~Graphic()
{

}

void Graphic::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	this->gl_window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (this->gl_window == NULL)
	{
		glfwTerminate();
		throw Exception("Failed to create GLFW window", __FILE__, __LINE__);
	}
	glfwMakeContextCurrent(this->gl_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw Exception("Failed to initialize GLAD", __FILE__, __LINE__);
	}

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(this->gl_window, framebuffer_size_callback);
}

void Graphic::close()
{
	glfwTerminate();
}

bool Graphic::checkWindow()
{
	return !glfwWindowShouldClose(this->gl_window);
}

void Graphic::processInput()
{
	if(glfwGetKey(this->gl_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->gl_window, true);
}

void Graphic::renderPre()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Graphic::renderPost()
{
	glfwPollEvents();    
	glfwSwapBuffers(this->gl_window);
}
