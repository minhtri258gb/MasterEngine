#define __MT_MODEL_MANAGER_CPP__

#include "common.h"
#include "ModelMgr.h"
#include "SimpleModel.h"

using namespace std;
using namespace mt::graphic;

ModelMgr::ModelMgr()
{
}

ModelMgr::~ModelMgr()
{
}

void ModelMgr::init()
{
	// Simple Model
	SimpleModel::shader.init("simple", ShaderProgram::SHADER_TYPE::VERTEX | ShaderProgram::SHADER_TYPE::FRAGMENT);
	SimpleModel::shader.addLocation("view");		// 0
	SimpleModel::shader.addLocation("proj");		// 1
	SimpleModel::shader.addLocation("model");		// 2
	SimpleModel::shader.addLocation("ourTexture");	// 3
	SimpleModel::shader.setUnifrom(3, 0);
	
	// #EXTRA
}

void ModelMgr::close()
{
	// Clear Model
	map<string, Model*>::iterator it;
	for (it = this->models.begin(); it != this->models.end(); it++)
		delete it->second;
	this->models.clear();

	// Simple Model
	SimpleModel::shader.close();

	// #EXTRA
}

Model* ModelMgr::getModel(std::string name)
{
	if (this->models.count(name) == 1)
		return this->models.at(name);
	return nullptr;
}

void ModelMgr::addModel(std::string name, Model* model)
{
	this->models.emplace(name, model);
}
