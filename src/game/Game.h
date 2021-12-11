#ifndef __GAME_H__
#define __GAME_H__

namespace mt {
namespace game {

class Game {

public:

	// Forward
	Game();
	~Game();
	void run();

	// Native

	// Stock

private:

	// Variable
	bool mainloop;

	// Forward
	void init();
	void framework();
	void close();

	// Native

};

}}

#endif
