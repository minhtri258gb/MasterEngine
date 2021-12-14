#ifndef __MT_MODEL_MANAGER_H__
#define __MT_MODEL_MANAGER_H__

#include "Model.h"

namespace mt {
namespace graphic {

class ModelMgr
{

public:

	// Variable

	// Forward
	ModelMgr();
	~ModelMgr();
	void init();
	void close();

	// Native
	Model* getModel(std::string name);
	void addModel(std::string name, Model*);

private:

	// Variable
	std::map<std::string, Model*> models;

};

}}

#endif
