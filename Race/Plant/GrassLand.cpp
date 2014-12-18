#include "stdafx.h"
#include "GrassLand.h"
#include "gfMath.h"

GrassLand::GrassLand(ISceneManager* smgr, 
	ITerrainNode* terrainNode,
	const std::vector<XMFLOAT3>& positions, 
	const math::SAxisAlignedBox& aabb,
	f32 pitchSize,
	const XMFLOAT2& grassSize)
	:mGrassSize(grassSize)
	, mTerrainNode(terrainNode)
{
	IMeshManager* meshManager = IMeshManager::getInstance(); 
	IMaterialManager* materialManager = IMaterialManager::getInstance();
	ITextureManager* textureManager = ITextureManager::getInstance();

	ISimpleMesh* mesh = meshManager->createSimpleMesh("grass", EVF_POSITION, (void*)&positions[0],
		0, positions.size(), sizeof(XMFLOAT3), 0, aabb, false);

	mMaterial = materialManager->get("grassland_material");
	ITexture* radiansTexture = textureManager->get("radians_texture");
	if (!radiansTexture)
	{
		radiansTexture = createRadiansTexture();
	}

	mMaterial->setTexture(1, radiansTexture);

	mMeshNode = smgr->addMeshNode(mesh, mMaterial, nullptr, true);
	mMeshNode->setRenderOrder(ERO_SKYDOME + 1);
	mMeshNode->addShadow(1);

	mWindDirection = math::GetNormalizedVector(-1.0f, 0, 1.0f);
	mBladeAmplitude = 0.5f;

	ITexture* heightMapTexture = mTerrainNode->getTerrainMesh()->getHeightMapTexture();
	mMaterial->setTexture(2, heightMapTexture);
}

void GrassLand::update(f32 dt)
{
	mMaterial->setAttribute("GrassWidth", mGrassSize.x);
	mMaterial->setAttribute("GrassHeight", mGrassSize.y);

	ITimer* timer = IDevice::getInstance()->getTimer();
	u32 milliseconds = timer->getElapseTime();
	f32 sec = milliseconds * 0.001f;

	f32 fluctuation = mBladeAmplitude * sinf(sec * 1.0f);

	mMaterial->setAttribute("BladeFluctuation", fluctuation);
	mMaterial->setAttribute("WindDirection", mWindDirection);

	f32 width = mTerrainNode->getTerrainMesh()->getTotalWidth();
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
		ETBT_SHADER, (void*)&data[0], 1, EGF_R32G32_FLOAT);

	return texture;
}


GrassLand::~GrassLand()
{


}

