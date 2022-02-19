#define __MT_FRUSTUM_CULLING_CPP__

#include "common.h"
#include "engine/Input.h"
#include "Graphic.h"
#include "FrustumCulling.h"

using namespace std;
using namespace mt;
using namespace mt::engine;
using namespace mt::graphic;

bool debug_frustumCulling = false;

FrustumCulling::FrustumCulling()
{
}

FrustumCulling::~FrustumCulling()
{
}

void FrustumCulling::update()
{
	if (Input::ins.checkPress(292)) // F3
		debug_frustumCulling = !debug_frustumCulling;
	if (debug_frustumCulling)
		return;

	mat4 clip = Graphic::ins.scene.proj * Graphic::ins.scene.view;

	frustum[0][0] = clip[ 3] - clip[ 0];
	frustum[0][1] = clip[ 7] - clip[ 4];
	frustum[0][2] = clip[11] - clip[ 8];
	frustum[0][3] = clip[15] - clip[12];
	frustum[1][0] = clip[ 3] + clip[ 0];
	frustum[1][1] = clip[ 7] + clip[ 4];
	frustum[1][2] = clip[11] + clip[ 8];
	frustum[1][3] = clip[15] + clip[12];
	frustum[2][0] = clip[ 3] + clip[ 1];
	frustum[2][1] = clip[ 7] + clip[ 5];
	frustum[2][2] = clip[11] + clip[ 9];
	frustum[2][3] = clip[15] + clip[13];
	frustum[3][0] = clip[ 3] - clip[ 1];
	frustum[3][1] = clip[ 7] - clip[ 5];
	frustum[3][2] = clip[11] - clip[ 9];
	frustum[3][3] = clip[15] - clip[13];
	frustum[4][0] = clip[ 3] - clip[ 2];
	frustum[4][1] = clip[ 7] - clip[ 6];
	frustum[4][2] = clip[11] - clip[10];
	frustum[4][3] = clip[15] - clip[14];
	frustum[5][0] = clip[ 3] + clip[ 2];
	frustum[5][1] = clip[ 7] + clip[ 8];
	frustum[5][2] = clip[11] + clip[10];
	frustum[5][3] = clip[15] + clip[12];

	float t;
	for (int i = 0; i < 6; i++)
	{
		t = (float) sqrt(frustum[i][0] * frustum[i][0] + frustum[i][1] * frustum[i][1] + frustum[i][2] * frustum[i][2]);
		frustum[i][0] /= t;
		frustum[i][1] /= t;
		frustum[i][2] /= t;
		frustum[i][3] /= t;
	}
}

bool FrustumCulling::point(vec3 pos)
{
	for (int i = 0; i < 6; i++)
		if (frustum[i][0] * pos.x + frustum[i][1] * pos.y + frustum[i][2] * pos.z + frustum[i][3] <= 0)
			return false;
	return true;
}

bool FrustumCulling::sphere(vec3 pos, float _radius)
{
	for (int i = 0; i < 6; i++)
		if (frustum[i][0] * pos.x + frustum[i][1] * pos.y + frustum[i][2] * pos.z + frustum[i][3] <= -_radius)
			return false;
	return true;
}

bool FrustumCulling::AABB(vec3 pos, float _size)
{
	for (int i = 0; i < 6; i++)
	{
		if (frustum[i][0] * (pos.x - _size) + frustum[i][1] * (pos.y - _size) + frustum[i][2] * (pos.z - _size) + frustum[i][3] > 0)
			continue;
		if (frustum[i][0] * (pos.x + _size) + frustum[i][1] * (pos.y - _size) + frustum[i][2] * (pos.z - _size) + frustum[i][3] > 0)
			continue;
		if (frustum[i][0] * (pos.x - _size) + frustum[i][1] * (pos.y + _size) + frustum[i][2] * (pos.z - _size) + frustum[i][3] > 0)
			continue;
		if (frustum[i][0] * (pos.x + _size) + frustum[i][1] * (pos.y + _size) + frustum[i][2] * (pos.z - _size) + frustum[i][3] > 0)
			continue;
		if (frustum[i][0] * (pos.x - _size) + frustum[i][1] * (pos.y - _size) + frustum[i][2] * (pos.z + _size) + frustum[i][3] > 0)
			continue;
		if (frustum[i][0] * (pos.x + _size) + frustum[i][1] * (pos.y - _size) + frustum[i][2] * (pos.z + _size) + frustum[i][3] > 0)
			continue;
		if (frustum[i][0] * (pos.x - _size) + frustum[i][1] * (pos.y + _size) + frustum[i][2] * (pos.z + _size) + frustum[i][3] > 0)
			continue;
		if (frustum[i][0] * (pos.x + _size) + frustum[i][1] * (pos.y + _size) + frustum[i][2] * (pos.z + _size) + frustum[i][3] > 0)
			continue;
		return false;
	}
	return true;
}
