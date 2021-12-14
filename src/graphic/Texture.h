#ifndef __MT_TEXTURE_H__
#define __MT_TEXTURE_H__

namespace mt {
namespace graphic {

class Texture
{

public:

	// Variable
	unsigned int gl_textureId;

	// Forward
	Texture();
	~Texture();
	void init(std::string filepath, bool flip = false);

	// Native
	void bind(unsigned int location = 0);

};

}}

#endif
