#define __MT_MODEL_BUILDER_CPP__

#include "common.h"
#include "../Graphic.h"
#include "ModelBuilder.h"
#include "SimpleModel.h"

using namespace std;
using namespace glm;
using namespace mt::graphic;

ModelBuilder ModelBuilder::ins;

ModelBuilder::ModelBuilder()
{
}

ModelBuilder::~ModelBuilder()
{
}

void ModelBuilder::clear()
{

}

Model* ModelBuilder::cache(std::string name)
{
	Model* model = Graphic::ins.modelMgr.getModel(name);
	if (model == nullptr)
	{
		// Create memory
		SimpleModel *newModel = new SimpleModel();

		// Read config #TODO

		// Read data #TODO
		vector<vec3> vertices;
		vertices.push_back(vec3( 0.5f,  0.5f, 0.0f));
		vertices.push_back(vec3( 0.5f, -0.5f, 0.0f));
		vertices.push_back(vec3(-0.5f, -0.5f, 0.0f));
		vertices.push_back(vec3(-0.5f,  0.5f, 0.0f));
		
		vector<vec2> texcoords;
		texcoords.push_back(vec2(1.0f, 1.0f));
		texcoords.push_back(vec2(1.0f, 0.0f));
		texcoords.push_back(vec2(0.0f, 0.0f));
		texcoords.push_back(vec2(0.0f, 1.0f));
		
		vector<unsigned int> indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(3);

		// Load data to VAO
		newModel->VAO.init();
		// VAO.setCount(vertices.size());
		newModel->VAO.bind();
		newModel->VAO.addAttribute(vertices);
		newModel->VAO.addAttribute(texcoords);
		newModel->VAO.addIndices(indices);
		newModel->VAO.unbind();

		// Load Material
		newModel->texture.init("../res/textures/wall.jpg");

		// Override class
		model = newModel;

		Graphic::ins.modelMgr.addModel(name, model);
	}

	return model;
}



