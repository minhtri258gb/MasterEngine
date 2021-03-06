#define __MT_GROUND_ENTITY_CPP__

#include "common.h"
// #include "engine/Engine.h"
#include "graphic/Graphic.h"
// #include "module/physic/Physic.h"
#include "GroundEnt.h"

using namespace std;
using namespace mt::graphic;
using namespace mt::game;

GroundEnt::GroundEnt(string name)
{
	this->model = Graphic::ins.modelMgr.cache("plane");
}

GroundEnt::~GroundEnt()
{
}

void GroundEnt::update()
{
}

void GroundEnt::render()
{
	this->model->position = vec3(0,-2,0);
	this->model->angle = quat(0,0,0,1);
	this->model->scale = vec3(10,10,10);
	this->model->render();
}
