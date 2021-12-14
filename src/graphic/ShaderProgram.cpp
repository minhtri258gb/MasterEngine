#define __MT_SHADER_PROGRAM_CPP__

#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "ShaderProgram.h"
#include "engine/Config.h"

using namespace mt::engine;
using namespace mt::graphic;

ShaderProgram::ShaderProgram()
{
	this->programId = -1;
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
	locations.clear();

	glDeleteProgram(this->programId);
}

void ShaderProgram::use()
{
	glUseProgram(this->programId);
}

void ShaderProgram::addLocation(std::string _unifromName)
{
	int location = glGetUniformLocation(this->programId, _unifromName.c_str());
	this->locations.push_back(location);
}

void ShaderProgram::setUnifrom(int _locationId, bool _value)
{         
    glUniform1i(this->locations.at(_locationId), (int)_value); 
}

void ShaderProgram::setUnifrom(int _locationId, int _value)
{ 
    glUniform1i(this->locations.at(_locationId), _value); 
}

void ShaderProgram::setUnifrom(int _locationId, float _value)
{ 
    glUniform1f(this->locations.at(_locationId), _value); 
} 

void ShaderProgram::setUnifrom(int _locationId, glm::vec2 _value)
{
	glUniform2f(this->locations.at(_locationId), _value.x, _value.y);
}

void ShaderProgram::setUnifrom(int _locationId, glm::ivec2 _value)
{
	glUniform2i(this->locations.at(_locationId), _value.x, _value.y);
}

void ShaderProgram::setUnifrom(int _locationId, glm::vec3 _value)
{
	glUniform3f(this->locations.at(_locationId), _value.x, _value.y, _value.z);
}

void ShaderProgram::setUnifrom(int _locationId, glm::ivec3 _value)
{
	glUniform3i(this->locations.at(_locationId), _value.x, _value.y, _value.z);
}

void ShaderProgram::setUnifrom(int _locationId, glm::vec4 _value)
{
	glUniform4f(this->locations.at(_locationId), _value.x, _value.y, _value.z, _value.w);
}

void ShaderProgram::setUnifrom(int _locationId, glm::mat4 _value)
{
	glUniformMatrix4fv(this->locations.at(_locationId), 1, GL_FALSE, glm::value_ptr(_value));
}

unsigned int ShaderProgram::initShaderPath(std::string _filepath, SHADER_TYPE _type)
{
	// Read source
	std::ifstream shaderFile;
	shaderFile.open(_filepath);
	if (!shaderFile)
		throw Exception("Unable to open file " + _filepath, __FILE__, __LINE__);

	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();
	
	std::string shaderSrc = shaderStream.str();
	const char* bufferSource = shaderSrc.c_str();

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

	return shaderId;
}
