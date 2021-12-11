#define __GAME_CPP__

#include "common.h"
#include "Game.h"
#include "graphic/Graphic.h"

// test import
#include "graphic/ShaderProgram.h"
#include "graphic/VertexArrayObject.h"

using namespace mt::engine;
using namespace mt::graphic;
using namespace mt::game;

Game::Game()
{
	this->mainloop = true;
}

Game::~Game()
{

}

void Game::run()
{
	try
	{
		this->init();

		this->framework();

		this->close();
	}
	catch (Exception e)
	{
		std::cerr << e.getMessage() << '\n';
	}
}

void Game::init()
{
	// Init module
	Graphic::ins.init();

	// #ADD
}

void Game::framework()
{

	// test playground
	ShaderProgram shader;
	shader.init("simple",	ShaderProgram::SHADER_TYPE::VERTEX |
							ShaderProgram::SHADER_TYPE::FRAGMENT);

	std::vector<glm::vec3> vertices;
	vertices.push_back(glm::vec3( 0.5f,  0.5f, 0.0f));
	vertices.push_back(glm::vec3( 0.5f, -0.5f, 0.0f));
	vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
	vertices.push_back(glm::vec3(-0.5f,  0.5f, 0.0f));

	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	VertexArrayObject VAO;
	VAO.init();
	// VAO.setCount(vertices.size());
	VAO.bind();
	VAO.addAttribute(vertices);
	VAO.addIndices(indices);
	VAO.unbind();

	// Main loop
	while (this->mainloop)
	{
		// input
		Graphic::ins.processInput();

		// rendering commands here
		Graphic::ins.renderPre();

		// test render
		shader.use();
		VAO.bind();
		// VAO.renderTriangle();
		VAO.drawElementTriangle();
		VAO.unbind();
		
		// #ADD

		// check and call events and swap the buffers
		if (!Graphic::ins.checkWindow())
			this->mainloop = false;
		Graphic::ins.renderPost();
	}
}

void Game::close()
{
	// Close Module
	Graphic::ins.close();

	// #ADD
}
