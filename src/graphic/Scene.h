#ifndef __MT_SCENE_H__
#define __MT_SCENE_H__

namespace mt {
namespace graphic {

class Scene
{

public:

	// Variable
	glm::mat4 proj;
	glm::mat4 view;

	// Forward
	Scene();
	~Scene();
	void init();

	// Native

};

}}

#endif
