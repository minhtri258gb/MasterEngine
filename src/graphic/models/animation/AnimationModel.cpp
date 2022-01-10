#define	__MT_ANIMATION_MODEL_CPP__

#include <cstdlib>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <stb/stb_image.h>

#include "common.h"
#include "engine/Config.h"
#include "engine/Timer.h"
#include "../../Graphic.h"
#include "AnimationModel.h"

#include "../../VertexArrayObject.h"
#include "../../Texture.h"

using namespace std;
using namespace glm;
using namespace mt::engine;
using namespace mt::graphic;

inline glm::mat4 assimpToGlmMatrix(aiMatrix4x4 mat) {
	glm::mat4 m;
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
			m[x][y] = mat[y][x];
	return m;
}

inline glm::vec3 assimpToGlmVec3(aiVector3D vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::quat assimpToGlmQuat(aiQuaternion quat) {
	glm::quat q;
	q.x = quat.x;
	q.y = quat.y;
	q.z = quat.z;
	q.w = quat.w;
	return q;
}

// vertex of an animated model
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 boneIds = glm::vec4(0);
	glm::vec4 boneWeights = glm::vec4(0.0f);
};

// structure to hold bone tree (skeleton)
struct Bone {
	int id = 0; // position of the bone in final upload array
	string name = "";
	glm::mat4 offset = glm::mat4(1.0f);
	vector<Bone> children = {};
};

// sturction representing an animation track
struct BoneTransformTrack {
	vector<float> positionTimestamps = {};
	vector<float> rotationTimestamps = {};
	vector<float> scaleTimestamps = {};

	vector<glm::vec3> positions = {};
	vector<glm::quat> rotations = {};
	vector<glm::vec3> scales = {};
};

// structure containing animation information
struct Animation {
	float duration = 0.0f;
	float ticksPerSecond = 1.0f;
	unordered_map<string, BoneTransformTrack> boneTransforms = {};
};

class AnimationModel::AnimationModelImpl
{
public:
	VertexArrayObject VAO;
	Texture texture;

	uint boneCount = 0;
	Animation animation;
	Bone skeleton;
	glm::mat4 globalInverseTransform;
	
	glm::mat4 identity;
	vector<glm::mat4> currentPose = {};

	AnimationModelImpl() {
		this->identity = glm::mat4(1.0);
	}
	
	// a recursive function to read all bones and form skeleton
	bool readSkeleton(Bone& boneOutput, aiNode* node, unordered_map<string, pair<int, glm::mat4>>& boneInfoTable) {

		if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end()) { // if node is actually a bone
			boneOutput.name = node->mName.C_Str();
			boneOutput.id = boneInfoTable[boneOutput.name].first;
			boneOutput.offset = boneInfoTable[boneOutput.name].second;

			for (int i = 0; i < node->mNumChildren; i++) {
				Bone child;
				readSkeleton(child, node->mChildren[i], boneInfoTable);
				boneOutput.children.push_back(child);
			}
			return true;
		}
		else { // find bones in children
			for (int i = 0; i < node->mNumChildren; i++) {
				if (readSkeleton(boneOutput, node->mChildren[i], boneInfoTable)) {
					return true;
				}

			}
		}
		return false;
	}

	void loadModel(const aiScene* scene, aiMesh* mesh, vector<Vertex>& verticesOutput, vector<uint>& indicesOutput, Bone& skeletonOutput, uint &nBoneCount) {
		
		verticesOutput = {};
		indicesOutput = {};
		//load position, normal, uv
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			//process position 
			Vertex vertex;
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;
			//process normal
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
			//process uv
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = vec;

			vertex.boneIds = glm::ivec4(0);
			vertex.boneWeights = glm::vec4(0.0f);

			verticesOutput.push_back(vertex);
		}

		//load boneData to vertices
		unordered_map<string, pair<int, glm::mat4>> boneInfo = {};
		vector<uint> boneCounts;
		boneCounts.resize(verticesOutput.size(), 0);
		nBoneCount = mesh->mNumBones;

		//loop through each bone
		for (uint i = 0; i < nBoneCount; i++) {
			aiBone* bone = mesh->mBones[i];
			glm::mat4 m = assimpToGlmMatrix(bone->mOffsetMatrix);
			boneInfo[bone->mName.C_Str()] = { i, m };

			//loop through each vertex that have that bone
			for (int j = 0; j < bone->mNumWeights; j++) {
				uint id = bone->mWeights[j].mVertexId;
				float weight = bone->mWeights[j].mWeight;
				boneCounts[id]++;
				switch (boneCounts[id]) {
				case 1:
					verticesOutput[id].boneIds.x = i;
					verticesOutput[id].boneWeights.x = weight;
					break;
				case 2:
					verticesOutput[id].boneIds.y = i;
					verticesOutput[id].boneWeights.y = weight;
					break;
				case 3:
					verticesOutput[id].boneIds.z = i;
					verticesOutput[id].boneWeights.z = weight;
					break;
				case 4:
					verticesOutput[id].boneIds.w = i;
					verticesOutput[id].boneWeights.w = weight;
					break;
				default:
					//cout << "err: unable to allocate bone to vertex" << endl;
					break;

				}
			}
		}

		//normalize weights to make all weights sum 1
		for (int i = 0; i < verticesOutput.size(); i++) {
			glm::vec4 & boneWeights = verticesOutput[i].boneWeights;
			float totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
			if (totalWeight > 0.0f) {
				verticesOutput[i].boneWeights = glm::vec4(
					boneWeights.x / totalWeight,
					boneWeights.y / totalWeight,
					boneWeights.z / totalWeight,
					boneWeights.w / totalWeight
				);
			}
		}

		//load indices
		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indicesOutput.push_back(face.mIndices[j]);
		}

		// create bone hirerchy
		readSkeleton(skeletonOutput, scene->mRootNode, boneInfo);
	}

	void loadAnimation(const aiScene* scene, Animation& animation) {
		//loading  first Animation
		aiAnimation* anim = scene->mAnimations[0];

		if (anim->mTicksPerSecond != 0.0f)
			animation.ticksPerSecond = anim->mTicksPerSecond;
		else
			animation.ticksPerSecond = 1;


		animation.duration = anim->mDuration * anim->mTicksPerSecond;
		animation.boneTransforms = {};

		//load positions rotations and scales for each bone
		// each channel represents each bone
		for (int i = 0; i < anim->mNumChannels; i++) {
			aiNodeAnim* channel = anim->mChannels[i];
			BoneTransformTrack track;
			for (int j = 0; j < channel->mNumPositionKeys; j++) {
				track.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
				track.positions.push_back(assimpToGlmVec3(channel->mPositionKeys[j].mValue));
			}
			for (int j = 0; j < channel->mNumRotationKeys; j++) {
				track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
				track.rotations.push_back(assimpToGlmQuat(channel->mRotationKeys[j].mValue));

			}
			for (int j = 0; j < channel->mNumScalingKeys; j++) {
				track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
				track.scales.push_back(assimpToGlmVec3(channel->mScalingKeys[j].mValue));
		
			}
			animation.boneTransforms[channel->mNodeName.C_Str()] = track;
		}
	}

	pair<uint, float> getTimeFraction(vector<float>& times, float& dt) {
		uint segment = 0;
		while (dt > times[segment])
			segment++;
		float start = times[segment - 1];
		float end = times[segment];
		float frac = (dt - start) / (end - start);
		return {segment, frac};
	}

	void getPose(Bone& skeletion, float dt, vector<glm::mat4>& output, glm::mat4 &parentTransform) {
		BoneTransformTrack& btt = animation.boneTransforms[skeletion.name];
		dt = fmod(dt, animation.duration);
		pair<uint, float> fp;
		//calculate interpolated position
		fp = getTimeFraction(btt.positionTimestamps, dt);

		glm::vec3 position1 = btt.positions[fp.first - 1];
		glm::vec3 position2 = btt.positions[fp.first];

		glm::vec3 position = glm::mix( position1, position2, fp.second);

		//calculate interpolated rotation
		fp = getTimeFraction(btt.rotationTimestamps, dt);
		glm::quat rotation1 = btt.rotations[fp.first - 1];
		glm::quat rotation2 = btt.rotations[fp.first];

		glm::quat rotation = glm::slerp( rotation1, rotation2,fp.second);

		//calculate interpolated scale
		fp = getTimeFraction(btt.scaleTimestamps, dt);
		glm::vec3 scale1 = btt.scales[fp.first - 1];
		glm::vec3 scale2 = btt.scales[fp.first];

		glm::vec3 scale = glm::mix(scale1, scale2, fp.second);

		glm::mat4 positionMat = glm::mat4(1.0),
			scaleMat = glm::mat4(1.0);

		// calculate localTransform
		positionMat = glm::translate(positionMat, position);
		glm::mat4 rotationMat = glm::toMat4(rotation);
		scaleMat = glm::scale(scaleMat, scale);
		glm::mat4 localTransform = positionMat * rotationMat * scaleMat;
		glm::mat4 globalTransform = parentTransform * localTransform;

		output[skeletion.id] = globalInverseTransform * globalTransform * skeletion.offset;
		//update values for children bones
		for (Bone& child : skeletion.children) {
			getPose(child, dt, output, globalTransform);
		}
		//cout << dt << " => " << position.x << ":" << position.y << ":" << position.z << ":" << endl;
	}

};

ShaderProgram AnimationModel::shader;

AnimationModel::AnimationModel()
{
	// Implement
	this->impl = new AnimationModelImpl();

	//load model file
	Assimp::Importer importer;
	const char* filePath = "../res/models/man/model.dae";
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
	}
	aiMesh* mesh = scene->mMeshes[0];

	//as the name suggests just inverse the global transform
	this->impl->globalInverseTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
	this->impl->globalInverseTransform = glm::inverse(this->impl->globalInverseTransform);
	
	vector<Vertex> vertices = {};
	vector<uint> indices = {};
	this->impl->loadModel(scene, mesh, vertices, indices, this->impl->skeleton, this->impl->boneCount);
	this->impl->loadAnimation(scene, this->impl->animation);

	// Prepare data
	vector<glm::vec3> position_tmp;
	vector<glm::vec2> texcoords_tmp;
	vector<glm::vec3> normals_tmp;
	vector<glm::ivec4> bondIds_tmp;
	vector<glm::vec4> weights_tmp;
	vector<unsigned int> indices_tmp;
	for (Vertex v : vertices)
	{
		position_tmp.push_back(v.position);
		texcoords_tmp.push_back(v.uv);
		normals_tmp.push_back(v.normal);
		bondIds_tmp.push_back(glm::ivec4((int)v.boneIds.x,(int)v.boneIds.y,(int)v.boneIds.z,(int)v.boneIds.w)); 
		weights_tmp.push_back(v.boneWeights);
	}
	for (uint id : indices)
	{
		indices_tmp.push_back(id);
	}

	// Init Vertices Buffer
	this->impl->VAO.init();
	this->impl->VAO.bind();
	this->impl->VAO.addAttribute(position_tmp);
	this->impl->VAO.addAttribute(texcoords_tmp);
	this->impl->VAO.addAttribute(normals_tmp);
	this->impl->VAO.addAttribute(bondIds_tmp);
	this->impl->VAO.addAttribute(weights_tmp);
	this->impl->VAO.addIndices(indices_tmp);
	this->impl->VAO.unbind();

	this->impl->texture.init("../res/models/man/diffuse.png");

	this->impl->currentPose.resize(this->impl->boneCount, this->impl->identity); // use this for no animation
}

AnimationModel::~AnimationModel()
{
	// Implement
	delete this->impl;
}

void AnimationModel::update()
{
}

void AnimationModel::render()
{
	// Shader
	this->shader.use();

	// model matrix
	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 1.0f, -5.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(.3f, .3f, .3f));
	this->shader.setUnifrom(2, modelMatrix);
	
	// bone matrix
	float elapsedTime = (float)glfwGetTime();
	this->impl->getPose(this->impl->skeleton, elapsedTime, this->impl->currentPose, this->impl->identity);
	this->shader.setUniform(3, this->impl->currentPose);
	
	// draw
	this->impl->texture.bind();

	this->impl->VAO.bind();
	this->impl->VAO.drawElementTriangle();
	this->impl->VAO.unbind();
}
