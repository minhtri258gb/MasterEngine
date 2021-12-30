
#ifndef OGLDEV_SKINNED_MESH_H
#define	OGLDEV_SKINNED_MESH_H

using namespace std;

class SkinnedMesh
{
public:
    SkinnedMesh();

    ~SkinnedMesh();

    bool LoadMesh(const string& Filename);

    void Render();
	
    unsigned int NumBones() const;
    
    void BoneTransform(float TimeInSeconds); // , vector<Matrix4f>& Transforms
    
private:
    class SkinnedMeshImpl;
    SkinnedMeshImpl* impl;
};

#endif
