#include "stdafx.h"
#include "TireTrail.h"


TireTrail::TireTrail(ISceneManager* sceneManager, ITerrainNode* terrainNode)
:mSceneManager(sceneManager)
, mTerrainNode(terrainNode)
{
	mMaxMarkNumPerTire = 300;
	u32 vertexNum = mMaxMarkNumPerTire * 2 * 4;
	u32 indiceNum = (mMaxMarkNumPerTire - 1) * 6 * 4;

	mVertices = (Vertex*)malloc(sizeof(Vertex)* vertexNum);
	mIndices = (u16*)malloc(sizeof(u16)* indiceNum);

	IMeshManager* meshManager = IMeshManager::getInstance();
	mTireMesh = meshManager->createSimpleMesh("trail", 0, 0, 0, vertexNum,
		sizeof(Vertex), indiceNum, sceneManager->getAabb(), false, EMU_DYNAMIC);

	mTireMeshNode = mSceneManager->addMeshNode(mTireMesh, nullptr, nullptr, false);
	mTireMeshNode->setMaterialName("tiretrail_material");
}


TireTrail::~TireTrail()
{
	free(mVertices);
	free(mIndices);
}

void TireTrail::update(const Vector3 positions[], 
	const Vector3 tireDirs[], f32 dt, bool forward)
{
	bool bNeedUpdate = false;
	for (u32 i = 0; i < 4; i++)
	{
		Vertex v;
		v.Position = positions[i];

		if (mTireVertexQueues[i].empty())
		{
			pushTireVertices(mTireVertexQueues[i], positions[i], tireDirs[i], forward);
			bNeedUpdate = true;
			continue;
		}

		u32 size = mTireVertexQueues[i].size();
		Vector3 lastPosition = (mTireVertexQueues[i][size - 1].Position + mTireVertexQueues[i][size - 2].Position) * 0.5f;
		f32 sqrDist = positions[i].distanceSquared(lastPosition);
		if (sqrDist > 0.5f && checkTireOnGround(positions[i]))
		{
			pushTireVertices(mTireVertexQueues[i], positions[i], tireDirs[i], forward);
			bNeedUpdate = true;
		}
	}

	fillMeshBuffer();
}



void TireTrail::pushTireVertices(std::deque<Vertex>& Q,
	const Vector3& pos, const Vector3& dir, bool forward)
{
	/*
	v1 - v2
 	|    |
	v3 - v4
	*/
	
	// vector from v1 to v2
	Vector3 up(0, 1.0f, 0);
	Vector3 d1 = (forward)?up.cross(dir) : dir.cross(up);
	d1.normalize();

	const f32 tireWidth = 0.12f;
	Vector3 v1 = pos - d1 * tireWidth;
	Vector3 v2 = pos + d1 * tireWidth;

	Q.push_back(Vertex(v1));
	Q.push_back(Vertex(v2));

	if (Q.size() > mMaxMarkNumPerTire)
	{
		Q.pop_front();
		Q.pop_front();
	}
}

void TireTrail::fillMeshBuffer()
{
	u32 indiceCount = 0;
	u32 vertexCount = 0;
	for (u32 i = 0; i < 4; i++)
	{
		u32 firstVertexIndex = vertexCount;
		for (u32 j = 0; j < mTireVertexQueues[i].size(); j++)
		{
			mVertices[vertexCount++] = mTireVertexQueues[i][j];
		}

		if (mTireVertexQueues[i].size() > 2)
		{
			for (int j = 0; j < (mTireVertexQueues[i].size() / 2 - 1); j++)
			{
				mIndices[indiceCount++] = firstVertexIndex + 2 * j;
				mIndices[indiceCount++] = firstVertexIndex + 2 * (j + 1) + 1;
				mIndices[indiceCount++] = firstVertexIndex + 2 * j + 1;

				mIndices[indiceCount++] = firstVertexIndex + 2 * j;
				mIndices[indiceCount++] = firstVertexIndex + 2 * (j + 1);
				mIndices[indiceCount++] = firstVertexIndex + 2 * (j + 1) + 1;
			}
		}
	}

	IMeshBuffer* buffer = dynamic_cast<IMeshBuffer*>(mTireMesh->getRenderableBuffer());
	buffer->setVertexData(mVertices, vertexCount);
	buffer->setIndiceData(mIndices, indiceCount);
}

void TireTrail::render()
{
	mSceneManager->draw(mTireMeshNode);
}

bool TireTrail::checkTireOnGround(Vector3 pos)
{
	f32 height = mTerrainNode->getHeight(pos.x, pos.z);
	if (fabs(pos.y - height) < 0.1f)
		return true;
	
	return false;
}


