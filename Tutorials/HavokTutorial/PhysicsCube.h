#pragma once

using namespace gf;

class PhysicsCube
{
public:
	PhysicsCube(IMeshNode* node, XMFLOAT3 halfExtends, XMFLOAT3 pos, 
		bool bStatic, f32 restitution, f32 friction);
	void update(f32 dt);

private:

	IMeshNode*				mMeshNode;
	hkpRigidBody*			mRigidBody;
};


