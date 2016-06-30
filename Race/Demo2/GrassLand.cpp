#include "stdafx.h"
#include "GrassLand.h"
#include "gfMath.h"

GrassLand::GrassLand(ISceneManager* smgr, 
	ITerrainNode* terrainNode,
	const std::vector<XMFLOAT3>& positions, 
	f32 pitchSize,
	const XMFLOAT2& grassSize)
	:mGrassSize(grassSize)
	, mTerrainNode(terrainNode)
{
	IMeshManager* meshManager = IMeshManager::getInstance(); 
	IMaterialManager* materialManager = IMaterialManager::getInstance();
	ITextureManager* textureManager = ITextureManager::getInstance();
	ITerrainMesh* terrainMesh = terrainNode->getTerrainMesh();

	math::SAxisAlignedBox aabb;
	f32 maxHeight = terrainMesh->getHeightScale() * 255.0f;
	aabb.Extents.y = (maxHeight + grassSize.y) * 0.5f;
	aabb.Extents.x = (grassSize.x + pitchSize) * 0.5f;
	aabb.Extents.z = (grassSize.x + pitchSize) * 0.5f;
	aabb.Center = XMFLOAT3(0, aabb.Extents.y, 0);

	ISimpleMesh* mesh = meshManager->createSimpleMesh("grass", EVF_POSITION, (void*)&positions[0],
		0, positions.size(), sizeof(XMFLOAT3), 0, aabb, false);

	mMaterial = materialManager->get("grassland_material");
	ITexture* radiansTexture = textureManager->get("radians_texture", false);
	if (!radiansTexture)
	{
		radiansTexture = createRadiansTexture();
	}

	mMaterial->setTexture(1, radiansTexture);
	
	f32 terrainWidth = terrainNode->getTerrainMesh()->getTotalWidth();
	mPitchNumPerRow = ceilf(terrainWidth / pitchSize);

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
			instanceNode->setFarCullingDistance(200.0f);
		}
	}

	mMeshNode->setRenderOrder(ERO_SKYDOME + 1);
	//mMeshNode->addShadow(1);

	mWindDirection = math::GetNormalizedVector(-1.0f, 0, 1.0f);
	mBladeAmplitude = 0.5f;

	ITexture* heightMapTexture = mTerrainNode->getTerrainMesh()->getHeightMapTexture();
	mMaterial->setTexture(2, heightMapTexture);
}

void GrassLand::update(f32 dt)
{
	mMaterial->setAttribute("GrassWidth", mGrassSize.x);
	mMaterial->setAttribute("GrassHeight", mGrassSize.y);

	ITimer* timer = IApplication::getInstance()->getTimer();
	//u32 milliseconds = timer->getElapseTime();
	f32 sec = timer->getElapseTime();

	f32 fluctuation = mBladeAmplitude * sinf(sec * 1.0f);

	mMaterial->setAttribute("BladeFluctuation", fluctuation);
	mMaterial->setAttribute("WindDirection", mWindDirection);

	f32 vertexSpace = mTerrainNode->getTerrainMesh()->getVertexSpace();
	u32 vertexNumPerRow = mTerrainNode->getTerrainMesh()->getRowVertexNum();
	f32 width = vertexSpace * vertexNumPerRow;

	mMaterial->setAttribute("TerrainWidth", width);
	mMaterial->setAttribute("InvTerrainWidth", 1.0f / width);
}

ITexture* GrassLand::createRadiansTexture()
{
	const u32 width = 1024;
	std::vector<XMFLOAT2> data(width);

	for (u32 i = 0; i < width; i++)
	{
		f32 rad = static_cast<f32>(i) / static_cast<f32>(width) * XM_2PI;
		data[i].x = sinf(rad);
		data[i].y = cosf(rad);
	}

	ITexture* texture = ITextureManager::getInstance()->createTexture1D("radians_texture", width,
		ETBT_SHADER_RESOURCE, (void*)&data[0], 1, EGF_R32G32_FLOAT);

	return texture;
}


GrassLand::~GrassLand()
{


}

