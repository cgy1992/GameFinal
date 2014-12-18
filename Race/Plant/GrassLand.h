#pragma once

using namespace gf;

class GrassLand
{
public:
	GrassLand(ISceneManager* smgr,
		ITerrainNode* terrainNode,
		const std::vector<XMFLOAT3>& positions,
		f32 pitchSize,
		const XMFLOAT2& grassSize);

	ITexture* createRadiansTexture();

	void update(f32 dt);

	~GrassLand();

private:
	XMFLOAT2		mGrassSize;
	IMaterial*		mMaterial; 
	IInstanceCollectionNode*	mMeshNode;
	ITerrainNode*	mTerrainNode;

	XMFLOAT3		mWindDirection;
	f32				mBladeAmplitude;
	u32				mPitchNumPerRow;
};

