#define __MT_SHADER_PROGRAM_NAMAGER_CPP__

#include "common.h"
#include "ShaderProgramMgr.h"
#include "models/SimpleModel.h"
#include "models/animation/AnimationModel.h"
// #include "particles/Sprite.h"

using namespace std;
using namespace mt;
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
	SimpleModel::shader.setUniform(3, 0);
	// SimpleModel::shader.setInt(3, 0);

	// Animation Model
	AnimationModel::shader.init("skinning", ShaderProgram::SHADER_TYPE::VERTEX | ShaderProgram::SHADER_TYPE::FRAGMENT);
	AnimationModel::shader.use();
	AnimationModel::shader.addLocation("proj");			// 0
	AnimationModel::shader.addLocation("view");			// 1
	AnimationModel::shader.addLocation("model");		// 2
	AnimationModel::shader.addLocation("bones");		// 3
	AnimationModel::shader.addLocation("diff_texture");	// 4
	AnimationModel::shader.setUniform(4, 0);
	// AnimationModel::shader.setInt(4, 0);
	
	// Sprite
	// Sprite::shader.init("particles", ShaderProgram::SHADER_TYPE::VERTEX | ShaderProgram::SHADER_TYPE::FRAGMENT); //  | ShaderProgram::SHADER_TYPE::GEOMETRY
	// Sprite::shader.use();
	// Sprite::shader.addLocation("proj");			// 0
	// Sprite::shader.addLocation("view");			// 1
	// Sprite::shader.addLocation("cameraPos");	// 2
	// Sprite::shader.addLocation("texture");		// 3
	// Sprite::shader.setUniform(3, 0);
	// Sprite::shader.setInt(3, 0);

	// #EXTRA
}

void ShaderProgramMgr::close()
{
	// Simple Model
	SimpleModel::shader.close();
	
	// Animation Model
	AnimationModel::shader.close();

	// Sprite
	// Sprite::shader.close();

	// #EXTRA
}

void ShaderProgramMgr::setSceneProj(mat4 _value)
{
	SimpleModel::shader.use();
	SimpleModel::shader.setMat4(0, _value);
	AnimationModel::shader.use();
	AnimationModel::shader.setMat4(0, _value);
	// Sprite::shader.use();
	// Sprite::shader.setMat4(0, _value);
}

void ShaderProgramMgr::setSceneView(mat4 _value)
{
	SimpleModel::shader.use();
	SimpleModel::shader.setMat4(1, _value);
	AnimationModel::shader.use();
	AnimationModel::shader.setMat4(1, _value);
	// Sprite::shader.use();
	// Sprite::shader.setMat4(1, _value);
}

void ShaderProgramMgr::setScreenProj(mat4 _value)
{

}

void ShaderProgramMgr::setScreenView(mat4 _value)
{

}

void ShaderProgramMgr::setCameraPosition(vec3 _value)
{
	// Sprite::shader.use();
	// Sprite::shader.setVec3(2, _value);
}
