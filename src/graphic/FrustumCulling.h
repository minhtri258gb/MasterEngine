#ifndef __MT_FRUSTUM_CULLING_H__
#define __MT_FRUSTUM_CULLING_H__

namespace mt {
namespace graphic {

class FrustumCulling
{
public:

	// Forwards
	FrustumCulling();
	~FrustumCulling();
	void update();

	// Natives
	bool point(vec3 pos);
	bool sphere(vec3 pos, float radius);
	bool AABB(vec3 pos, float size);

private:

	// Variable
	float frustum[6][4];

};

}}

#endif
