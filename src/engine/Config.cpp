#define __MT_CONFIG_CPP__

#include "common.h"
#include "Config.h"

using namespace mt::engine;

Config Config::ins;

Config::Config()
{
	// File path
	this->resource_dir = "../res/";
	this->shader_path = this->resource_dir + "shaders/";

	// Window
	this->windowWidth = 800;
	this->windowHeight = 600;
	this->windowName = "Master Engine";

	// Camera
	

	// #ADD
}

Config::~Config()
{

}
