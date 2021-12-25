#ifndef __MT_SHADER_PROGRAM_MANAGER_H__
#define __MT_SHADER_PROGRAM_MANAGER_H__

namespace mt {
namespace graphic {

class ShaderProgramMgr
{

public:

	// Variable

	// Forward
	ShaderProgramMgr();
	~ShaderProgramMgr();
	void init();
	void close();

	// Native
	void setSceneProj(glm::mat4);
	void setSceneView(glm::mat4);
	void setScreenProj(glm::mat4);
	void setScreenView(glm::mat4);

private:

	// Forward

};

}}

#endif
