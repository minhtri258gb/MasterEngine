#define __MT_TEST_ENTITY_CPP__

#include "common.h"
// #include "engine/Engine.h"
#include "graphic/models/ModelMgr.h"
#include "graphic/models/Model.h"
#include "graphic/models/ModelBuilder.h"
// #include "module/physic/Physic.h"
#include "TestEnt.h"

using namespace mt::graphic;
using namespace mt::game;

TestEnt::TestEnt(std::string name)
{
	this->model = ModelBuilder::ins.cache("simple");
}

TestEnt::~TestEnt()
{
}

void TestEnt::update()
{
}

void TestEnt::render()
{
	this->model->render();
}
