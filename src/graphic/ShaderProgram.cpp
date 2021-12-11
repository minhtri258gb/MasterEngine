#define __SHADER_PROGRAM_CPP__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "ShaderProgram.h"
#include "engine/Config.h"
#include "engine/load/TextFile.h"

using namespace mt::engine;
using namespace mt::graphic;

ShaderProgram::ShaderProgram()
{
	// #TODO
}

ShaderProgram::~ShaderProgram()
{

}

void ShaderProgram::init(std::string _filename, int _flag)
{
	// Get config
	_filename = Config::ins.shader_path + _filename;

	unsigned int vertexShaderId = -1;
	if (_flag & SHADER_TYPE::VERTEX)
		vertexShaderId = this->initShaderPath(_filename+".vs.glsl", SHADER_TYPE::VERTEX);

	unsigned int fragmentShaderId = -1;
	if (_flag & SHADER_TYPE::FRAGMENT)
		fragmentShaderId = this->initShaderPath(_filename+".fs.glsl", SHADER_TYPE::FRAGMENT);

	unsigned int geographyShaderId = -1;
	if (_flag & SHADER_TYPE::GEOMETRY)
		geographyShaderId = this->initShaderPath(_filename+".gs.glsl", SHADER_TYPE::GEOMETRY);

	// # ADD geo and more path

	// Compile Program
	this->programId = glCreateProgram();
	if (_flag & SHADER_TYPE::VERTEX)
		glAttachShader(this->programId, vertexShaderId);
	if (_flag & SHADER_TYPE::FRAGMENT)
		glAttachShader(this->programId, fragmentShaderId);
	if (_flag & SHADER_TYPE::GEOMETRY)
		glAttachShader(this->programId, geographyShaderId);
	glLinkProgram(this->programId);

	// Clean
	if (_flag & SHADER_TYPE::VERTEX)
		glDeleteShader(vertexShaderId);
	if (_flag & SHADER_TYPE::FRAGMENT)
		glDeleteShader(fragmentShaderId);
	if (_flag & SHADER_TYPE::GEOMETRY)
		glDeleteShader(geographyShaderId);
	
	// Check
	int success;
	char infoLog[512];
	glGetProgramiv(this->programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->programId, 512, NULL, infoLog);
		throw Exception(std::string("Failed to compile vertex shader: ")+infoLog, __FILE__, __LINE__);
	}
}

void ShaderProgram::close()
{
	// #TODO
}

void ShaderProgram::use()
{
	glUseProgram(this->programId);
}

unsigned int ShaderProgram::initShaderPath(std::string _filepath, SHADER_TYPE _type)
{
	// Read source
	TextFile file(_filepath);
	long length = file.getLength();
	if (length == 0)
		throw Exception("File is empty: "+_filepath, __FILE__, __LINE__);

	char *bufferSource = new char[length];
	file.read(bufferSource, length);

	// Create shader
	unsigned int shaderId;
	switch (_type)
	{
	case VERTEX:
		shaderId = glCreateShader(GL_VERTEX_SHADER);
		break;
	case FRAGMENT:
		shaderId = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	case GEOMETRY:
		shaderId = glCreateShader(GL_GEOMETRY_SHADER);
		break;
	}
	
	glShaderSource(shaderId, 1, &bufferSource, NULL);
	glCompileShader(shaderId);

	int success;
	char infoLog[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		throw Exception("Failed to compile shader: "+_filepath+'\n'+infoLog, __FILE__, __LINE__);
	}

	// Clean
	delete bufferSource;

	return shaderId;
}
