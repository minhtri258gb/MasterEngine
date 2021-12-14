#define __MT_GAME_CPP__

// #define LOG

#include "common.h"
#include "Game.h"
#include "graphic/Graphic.h"

// test import
#include "graphic/ShaderProgram.h"
#include "game/maps/LobbyMap.h"

using namespace std;
using namespace glm;
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

	#ifdef LOG
	cout << __FILE__ << " | " << __LINE__ << '\n';
	#endif

	try
	{
		this->init();

	#ifdef LOG
	cout << __FILE__ << " | " << __LINE__ << '\n';
	#endif

		this->framework();

	#ifdef LOG
	cout << __FILE__ << " | " << __LINE__ << '\n';
	#endif
	
		this->close();
	
	#ifdef LOG
	cout << __FILE__ << " | " << __LINE__ << '\n';
	#endif
	
	}
	catch (Exception e)
	{
		
		#ifdef LOG
		cout << __FILE__ << " | " << __LINE__ << '\n';
		#endif
	
		cerr << e.getMessage() << '\n';
	}
	catch (std::exception e)
	{
		
		#ifdef LOG
		cout << __FILE__ << " | " << __LINE__ << '\n';
		#endif
	
		cerr << e.what() << '\n';
	}
	
	#ifdef LOG
	cout << __FILE__ << " | " << __LINE__ << '\n';
	#endif
	
}

void Game::init()
{
	// Init module
	Graphic::ins.init();

	// Init component
	this->map = new LobbyMap();

	// #EXTRA
}

void Game::framework()
{

	#ifdef LOG
	cout << __FILE__ << " | " << __LINE__ << '\n';
	#endif

	// Main loop
	while (this->mainloop)
	{
		// input
		Graphic::ins.processInput();

		// input network

		// loading
		this->map->load();

		// rendering commands here
		Graphic::ins.renderPre();

		#ifdef LOG
		cout << __FILE__ << " | " << __LINE__ << '\n';
		#endif

		// Map render
		this->map->render();

		#ifdef LOG
		cout << __FILE__ << " | " << __LINE__ << '\n';
		#endif

		// #ADD

		// check and call events and swap the buffers
		if (!Graphic::ins.checkWindow())
			this->mainloop = false;
		Graphic::ins.renderPost();
	}
}

void Game::close()
{
	// Clear component
	
	// #EXTRA
	this->map->clear();
	delete this->map;

	// Close Module
	
	// #EXTRA
	
	Graphic::ins.close();
}
