#ifndef __MT_SHADER_PROGRAM_H__
#define __MT_SHADER_PROGRAM_H__

namespace mt {
namespace graphic {

class ShaderProgram
{

public:

	// Datatype
	enum SHADER_TYPE
	{
		VERTEX = 1 << 0,
		FRAGMENT = 1 << 1,
		GEOMETRY = 1 << 2
	};

	// Variable
	unsigned int programId;
	std::vector<int> locations;

	// Forward
	ShaderProgram();
	~ShaderProgram();
	void init(std::string filename, int flag);
	void close();

	// Native
	void use();
	void addLocation(std::string unifromName);
	void setUnifrom(int locationId, bool value);
	void setUnifrom(int locationId, int value);
	void setUnifrom(int locationId, float value);
	void setUnifrom(int locationId, glm::vec2 value);
	void setUnifrom(int locationId, glm::ivec2 value);
	void setUnifrom(int locationId, glm::vec3 value);
	void setUnifrom(int locationId, glm::ivec3 value);
	void setUnifrom(int locationId, glm::vec4 value);
	void setUnifrom(int locationId, glm::mat4 value);

private:

	// Forward
	unsigned int initShaderPath(std::string filepath, SHADER_TYPE type);

};

}}

#endif
