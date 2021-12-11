#ifndef __SHADER_PROGRAM_H__
#define __SHADER_PROGRAM_H__

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

	// Forward
	ShaderProgram();
	~ShaderProgram();
	void init(std::string filename, int flag);
	void close();

	// Native
	void use();

private:

	// Forward
	unsigned int initShaderPath(std::string filepath, SHADER_TYPE type);

};

}}

#endif
