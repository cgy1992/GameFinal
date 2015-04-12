#ifndef __TIRE_TRAIL_H__
#define __TIRE_TRAIL_H__

#include <queue>
#include "Vector3.h"
using namespace gf;

class TireTrail
{
public:
	struct Vertex
	{
		Vector3 Position;
		Vertex(){}
		Vertex(Vector3 p)
			:Position(p){}
	};

public:
	TireTrail(ISceneManager* sceneManager);
	~TireTrail();

	void update(const Vector3 positions[], const Vector3 tireDirs[], f32 dt);

private:

	void pushTireVertices(std::deque<Vertex>& Q, 
		const Vector3& pos, const Vector3& dir);

	void fillMeshBuffer();

private:
//	std::queue<Vertex>				mTirePositionQueues[4];
	std::deque<Vertex>				mTireVertexQueues[4];
	f32								mLastUpdateTimes[4];
	ISimpleMesh*					mTireMesh;
	ISceneManager*					mSceneManager;
	u32								mMaxMarkNumPerTire;
	Vertex*							mVertices;
	u16*							mIndices;
};


#endif
