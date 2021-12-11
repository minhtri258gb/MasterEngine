#ifndef __CONFIG_H__
#define __CONFIG_H__

namespace mt {
namespace engine {

class Config
{

public:

	// Variable
	static Config ins;

	// File Dir
	std::string resource_dir;
	std::string shader_path;

	// Forward
	Config();
	~Config();

	// Native

};

}}

#endif
