#define __MT_SCENE_CPP__

#include "common.h"
#include "engine/Config.h"
#include "Scene.h"
#include "Graphic.h"

using namespace glm;
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
	float width = Config::ins.windowWidth;
	float height = Config::ins.windowHeight;
	this->proj = perspective(radians(80.0f), width/height, 0.1f, 100.0f);
	// this->proj = mat4(1.0f);
	// this->view = translate(mat4(1.0f), vec3(0.0f, 0.0f, -3.0f));
	this->view = mat4(1.0f);

	Graphic::ins.shaderProgramMgr.setSceneProj(this->proj);
	Graphic::ins.shaderProgramMgr.setSceneView(this->view);
}
