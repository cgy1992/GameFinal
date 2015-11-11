#include "stdafx.h"
#include "CGrassLand.h"
#include "IDevice.h"
#include "ISceneManager.h"

namespace gf
{
	CGrassLand::CGrassLand(
		ITerrainNode* terrainNode,
		const XMFLOAT2& grassSize)
		:mGrassSize(grassSize)
		, mTerrainNode(terrainNode)
		, mMeshNode(nullptr)
		, mMaterial(nullptr)
	{
		
	}

	void CGrassLand::update(f32 dt)
	{
		mMaterial->setAttribute("GrassWidth", mGrassSize.x);
		mMaterial->setAttribute("GrassHeight", mGrassSize.y);

		ITimer* timer = IDevice::getInstance()->getTimer();
		//u32 milliseconds = timer->getElapseTime();
		f32 sec = timer->getElapseTime();

		f32 fluctuation = mBladeAmplitude * sinf(sec * 1.0f);

		mMaterial->setAttribute("BladeFluctuation", fluctuation);
		mMaterial->setAttribute("WindDirection", mWindDirection);

		f32 width = mTerrainNode->getTerrainMesh()->getTotalWidth();
		mMaterial->setAttribute("TerrainWidth", width);
		mMaterial->setAttribute("InvTerrainWidth", 1.0f / width);
	}

	ITexture* CGrassLand::createRadiansTexture()
	{
		const u32 width = 1024;
		std::vector<XMFLOAT2> data(width);

		for (u32 i = 0; i < width; i++)
		{
			f32 rad = static_cast<f32>(i) / static_cast<f32>(width)* XM_2PI;
			data[i].x = sinf(rad);
			data[i].y = cosf(rad);
		}

		ITexture* texture = ITextureManager::getInstance()->createTexture1D("radians_texture", width,
			ETBT_SHADER_RESOURCE, (void*)&data[0], 1, EGF_R32G32_FLOAT);

		return texture;
	}


	CGrassLand::~CGrassLand()
	{
		if (mMeshNode)
			mMeshNode->destroy();
	}

	bool CGrassLand::init(u32 grassCount, u32 pitchNumPerRow, 
		const std::string& materialName,
		f32 amplitude,
		XMFLOAT3 windDirection)
	{
		ISceneManager* smgr = mTerrainNode->getSceneManager();
		IMeshManager* meshManager = IMeshManager::getInstance();
		IMaterialManager* materialManager = IMaterialManager::getInstance();
		ITextureManager* textureManager = ITextureManager::getInstance();
		ITerrainMesh* terrainMesh = mTerrainNode->getTerrainMesh();

		math::SAxisAlignedBox aabb;
		f32 pitchSize = terrainMesh->getTotalWidth() / pitchNumPerRow;
		f32 maxHeight = terrainMesh->getHeightScale() * 255.0f;
		aabb.Extents.y = (maxHeight + mGrassSize.y) * 0.5f;
		aabb.Extents.x = (mGrassSize.x + pitchSize) * 0.5f;
		aabb.Extents.z = (mGrassSize.x + pitchSize) * 0.5f;
		aabb.Center = XMFLOAT3(0, aabb.Extents.y, 0);

		f32 range = pitchSize / 2;
		const f32 grassY = 0;
		std::vector<XMFLOAT3> v(grassCount);
		for (u32 i = 0; i < grassCount; i++)
		{
			v[i].x = math::RandomFloat(-range, range);
			v[i].y = grassY;
			v[i].z = math::RandomFloat(-range, range);
		}

		ISimpleMesh* mesh = meshManager->createSimpleMesh("grass", EVF_POSITION, (void*)&v[0],
			0, v.size(), sizeof(XMFLOAT3), 0, aabb, false);

		mMaterial = materialManager->get(materialName);
		if (!mMaterial)
			return false;

		ITexture* radiansTexture = textureManager->get("radians_texture");
		if (!radiansTexture)
		{
			radiansTexture = createRadiansTexture();
		}

		mMaterial->setTexture(1, radiansTexture);

		f32 terrainWidth = mTerrainNode->getTerrainMesh()->getTotalWidth();
		mPitchNumPerRow = pitchNumPerRow;

		mMeshNode = smgr->addInstanceCollectionNode(mesh, nullptr, mPitchNumPerRow * mPitchNumPerRow, 0);
		mMeshNode->setMaterial(mMaterial);

		f32 startX = -terrainWidth * 0.5f + pitchSize * 0.5f;
		f32 startZ = terrainWidth * 0.5f - pitchSize * 0.5f;

		for (u32 row = 0; row < mPitchNumPerRow; row++)
		{
			for (u32 col = 0; col < mPitchNumPerRow; col++)
			{
				XMFLOAT3 pos = XMFLOAT3(startX + col * pitchSize, 0, startZ - row * pitchSize);
				IInstanceNode* instanceNode = mMeshNode->addInstance(true, pos);
				instanceNode->addShadow(1);
				instanceNode->setFarCullingDistance(300.0f);
			}
		}

		mMeshNode->setRenderOrder(ERO_SKYDOME + 1);
		mMeshNode->addShadow(1);

		mWindDirection = math::GetNormalizedVector(windDirection.x, windDirection.y, windDirection.z);
		mBladeAmplitude = amplitude;

		ITexture* heightMapTexture = mTerrainNode->getTerrainMesh()->getHeightMapTexture();
		mMaterial->setTexture(2, heightMapTexture);
		return true;
	}


}

