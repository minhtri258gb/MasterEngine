#ifndef __MT_SCREEN_H__
#define __MT_SCREEN_H__

namespace mt {
namespace graphic {

class Screen
{

public:

	// Variable
	glm::mat4 proj;

	// Forward
	Screen();
	~Screen();
	void init();

	// Native

};

}}

#endif