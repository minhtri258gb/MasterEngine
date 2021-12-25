#define __MT_MODEL_MANAGER_CPP__

#include "common.h"
#include "ModelMgr.h"
#include "SimpleModel.h"

using namespace std;
using namespace glm;
using namespace mt::graphic;

ModelMgr::ModelMgr()
{
}

ModelMgr::~ModelMgr()
{
}

void ModelMgr::init()
{
}

void ModelMgr::clear()
{
	// Clear Model
	map<string, Model*>::iterator it;
	for (it = this->models.begin(); it != this->models.end(); it++)
	{
		delete it->second;
	}
	this->models.clear();
}

Model* ModelMgr::getModel(string name)
{
	if (this->models.count(name) == 1)
		return this->models.at(name);
	return nullptr;
}

void ModelMgr::addModel(string name, Model* model)
{
	this->models.emplace(name, model);
}
