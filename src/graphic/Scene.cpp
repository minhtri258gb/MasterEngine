#define __MT_SCENE_CPP__

#include "common.h"
#include "Scene.h"

using namespace mt::engine;
using namespace mt::graphic;

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::init()
{
	int width = 800;
	int height = 600;
	this->proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
}
