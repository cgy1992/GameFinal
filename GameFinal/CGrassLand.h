#ifndef __GF_GRASSLAND_CLASS__
#define __GF_GRASSLAND_CLASS__

#include "IInstanceCollectionNode.h"
#include "ITerrainNode.h"

namespace gf
{
	class CGrassLand
	{
	public:
		CGrassLand(ITerrainNode* terrainNode,
			const XMFLOAT2& grassSize);

		bool init(u32 grassCount, u32 pitchNumPerRow,
			const std::string& materialName,
			f32 amplitude,
			XMFLOAT3 windDirection);

		ITexture* createRadiansTexture();

		void update(f32 dt);

		~CGrassLand();

	private:
		XMFLOAT2		mGrassSize;
		IMaterial*		mMaterial;
		IInstanceCollectionNode*	mMeshNode;
		ITerrainNode*	mTerrainNode;

		XMFLOAT3		mWindDirection;
		f32				mBladeAmplitude;
		u32				mPitchNumPerRow;
	};
}


#endif
