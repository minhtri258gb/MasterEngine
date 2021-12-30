#define __MT_SHADER_PROGRAM_NAMAGER_CPP__

#include "common.h"
#include "ShaderProgramMgr.h"
#include "models/SimpleModel.h"
#include "models/iqm/IqmModel.h"

using namespace std;
using namespace glm;
using namespace mt::engine;
using namespace mt::graphic;

ShaderProgramMgr::ShaderProgramMgr()
{
}

ShaderProgramMgr::~ShaderProgramMgr()
{
}

void ShaderProgramMgr::init()
{
	// Simple Model
	SimpleModel::shader.init("simple", ShaderProgram::SHADER_TYPE::VERTEX | ShaderProgram::SHADER_TYPE::FRAGMENT);
	SimpleModel::shader.use();
	SimpleModel::shader.addLocation("proj");		// 0
	SimpleModel::shader.addLocation("view");		// 1
	SimpleModel::shader.addLocation("model");		// 2
	SimpleModel::shader.addLocation("ourTexture");	// 3
	SimpleModel::shader.setUnifrom(3, 0);

	// IQM Model
	// IqmModel::shader.init("simple", ShaderProgram::SHADER_TYPE::VERTEX | ShaderProgram::SHADER_TYPE::FRAGMENT);
	// IqmModel::shader.use();
	// IqmModel::shader.addLocation("proj");		// 0
	// IqmModel::shader.addLocation("view");		// 1
	// IqmModel::shader.addLocation("model");		// 2
	// IqmModel::shader.addLocation("ourTexture");	// 3
	// IqmModel::shader.setUnifrom(3, 0);
	
	// #EXTRA
}

void ShaderProgramMgr::close()
{
	// Simple Model
	SimpleModel::shader.close();
	
	// IQM Model
	// IqmModel::shader.close();

	// #EXTRA
}

void ShaderProgramMgr::setSceneProj(mat4 _value)
{
	SimpleModel::shader.use();
	SimpleModel::shader.setUnifrom(0, _value);
	// IqmModel::shader.use();
	// IqmModel::shader.setUnifrom(0, _value);
}

void ShaderProgramMgr::setSceneView(mat4 _value)
{
	SimpleModel::shader.use();
	SimpleModel::shader.setUnifrom(1, _value);
	// IqmModel::shader.use();
	// IqmModel::shader.setUnifrom(1, _value);
}

void ShaderProgramMgr::setScreenProj(mat4 _value)
{

}

void ShaderProgramMgr::setScreenView(mat4 _value)
{

}
