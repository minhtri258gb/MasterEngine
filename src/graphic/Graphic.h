#ifndef __MT_GRAPHIC_H__
#define __MT_GRAPHIC_H__

#include "ShaderProgramMgr.h"
#include "models/ModelMgr.h"
#include "Scene.h"
#include "Screen.h"
#include "Camera.h"

namespace mt {
namespace graphic {

class Graphic {

public:

	// Variable
	static Graphic ins;

	ShaderProgramMgr shaderProgramMgr;
	ModelMgr modelMgr;
	Scene scene;
	Screen screen;
	Camera camera;

	// Forward
	Graphic();
	~Graphic();
	void init();
	void close();

	void processInput();
	void renderPre();
	void renderPost();

	// Native
	bool checkWindow();
	double getTime();
	void setDepthTest(bool value = true);

	// Stock
	void cullFaceToogle(bool);
	void cullFaceBack(bool);
	void cullFaceClock(bool);

private:

	// Variable
	class GraphicImpl;
	GraphicImpl* impl;

};

}}

#endif
