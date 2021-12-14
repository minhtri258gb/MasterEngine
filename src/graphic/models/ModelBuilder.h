#ifndef __MT_MODEL_BUILDER_H__
#define __MT_MODEL_BUILDER_H__

namespace mt {
namespace graphic {

class ModelBuilder
{

public:

	// Variable
	static ModelBuilder ins;

	// Forward
	ModelBuilder();
	~ModelBuilder();
	void clear();

	// Native
	Model* cache(std::string name);

private:

	// Variable

};

}}

#endif
