#include "stdafx.h"
#include "CSceneLoader.h"
#include "GrassLand.h"
#include "RaceScene.h"
#include "TerrainHeightFieldShape.h"
#include "PhysicsEngine.h"

bool CSceneLoader::load(const char* filename, RaceScene* scene)
{
	IResourceGroupManager* rgmr = IResourceGroupManager::getInstance();
	std::string fullpath;
	if (!rgmr->getFileFullPath(filename, fullpath))
		return false;

	FILE* fp = fopen(fullpath.c_str(), "rb");
	if (!fp)
		return false;

	IDevice* device = IDevice::getInstance();
	IVideoDriver* driver = IVideoDriver::getInstance();
	IShaderManager* shaderManager = IShaderManager::getInstance();
	ITextureManager* textureManager = ITextureManager::getInstance();

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0.0f, 0);
	aabb.Extents = XMFLOAT3(3000.0f, 1000.0f, 3000.0f);

	ISceneManager* smgr = device->createSceneManager(aabb);
	scene->mSceneManager = smgr;

	// set deferred shading
	driver->setDeferredShading(true);
	driver->setDeferredShadingAlgorithm(EDSA_CS_TILE_BASED_DEFERRED_SHADING);
	IShader* cs_shader = shaderManager->load(EST_COMPUTE_SHADER, "defer_shader_cs.hlsl", "cs_main");
	smgr->setTileBasedDeferredShadingCS(cs_shader);

	SSceneFileHeader header;
	fread(&header, sizeof(header), 1, fp);

	// set sky box
	ITextureCube* skyTexture = textureManager->loadCubeTexture("skybox1.dds");
	smgr->setSkyDome(skyTexture);

	// set ambient
	smgr->setAmbient(header.Ambient);

	// set global directional light
	ILightNode* light = smgr->addDirectionalLight(1, nullptr, header.GlobalLightDirection);
	light->setDiffuse(header.GlobalLightDiffuse);
	light->setSpecular(header.GlobalLightSpecular);
	light->enableShadow(true);
	SDirectionalLight lightData;
	light->getLightData(&lightData);
	cs_shader->setRawData("gLight", &lightData, sizeof(SDirectionalLight));

	// set camera
	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, header.CameraPosition);
	camera->look(header.CameraLook);
	camera->setShadowRange(300.0f);
	scene->mFreeCamera = camera;

	BuildLandscape(scene, header);

	
	ReadMeshNodes(smgr, fp, header.MeshNodeCount);
	ReadCollectionNodes(smgr, fp, header.CollectionNodeCount);
	ReadLightNodes(smgr, fp, header.LightNodeCount);

	fclose(fp);

	return true;
}

void CSceneLoader::ReadMeshNodes(ISceneManager* smgr, FILE* fp, u32 meshCount)
{
	IMeshManager* meshManager = IMeshManager::getInstance();

	SSceneFileMeshNodeInfo info;
	for (u32 i = 0; i < meshCount; i++)
	{
		fread(&info, sizeof(info), 1, fp);
		if (info.Category != MESH_CATEGORY)
			continue;

		IModelMesh* mesh = meshManager->getModelMesh(info.MeshName, true);
		if (!mesh)
			continue;

		IMeshNode* node = smgr->addModelMeshNode(mesh, nullptr, info.Static,
			info.Position, info.Rotation, info.Scaling);

		node->setTag(info.Tag);
		if (info.ShadowCasting)
			node->addShadow(1);
	}
}

IInstanceCollectionNode* CSceneLoader::CreateCollectionNode(ISceneManager* smgr, IModelMesh* mesh, int maxNum)
{
	IInstanceCollectionNode* collectionNode = smgr->addInstanceCollectionNode(mesh, nullptr, maxNum, 0);
	IMaterialManager* materialManager = IMaterialManager::getInstance();
	IPipelineManager* pipelineManager = IPipelineManager::getInstance();

	std::string prefix = "multi_";
	for (u32 i = 0; i < mesh->getSubsetCount(); i++)
	{
		IMaterial* material = mesh->getMaterial(i);
		if (!material)
			continue;

		std::string multiMaterialName = prefix + material->getName();

		IMaterial* multi_material = materialManager->get(multiMaterialName, false);
		if (!multi_material)
		{
			SMaterial material2(*material);
			material2.setName(multiMaterialName);
			for (u32 j = 0; j < EPU_COUNT; j++)
			{
				IPipeline* pipeline = material2.getPipeline(j);
				if (pipeline)
				{
					std::string pipeline_name2 = prefix + pipeline->getName();
					IPipeline* pipeline2 = pipelineManager->get(pipeline_name2);
					material2.setPipeline(j, pipeline2);
				}
			}

			materialManager->add(material2);
			multi_material = materialManager->get(multiMaterialName, false);
		}

		collectionNode->setMaterial(i, multi_material);
	}

	collectionNode->addShadow(1);
	return collectionNode;
}

void CSceneLoader::ReadCollectionNodes(ISceneManager* smgr, FILE* fp, u32 count)
{
	for (u32 i = 0; i < count; i++)
	{
		ReadCollectionNode(smgr, fp);
	}
}

void CSceneLoader::ReadCollectionNode(ISceneManager* smgr, FILE* fp)
{
	IMeshManager* meshManager = IMeshManager::getInstance();

	SSceneFileMeshNodeInfo fileinfo;
	fread(&fileinfo, sizeof(fileinfo), 1, fp);
	u32 instanceCount;
	fread(&instanceCount, sizeof(u32), 1, fp);

	IModelMesh* mesh = meshManager->getModelMesh(fileinfo.MeshName, true);
	IInstanceCollectionNode* collectionNode = CreateCollectionNode(smgr, mesh, instanceCount);
	collectionNode->setTag(fileinfo.Tag);

	for (u32 i = 0; i < instanceCount; i++)
	{
		SSceneFileInstanceNodeInfo instanceInfo;
		fread(&instanceInfo, sizeof(instanceInfo), 1, fp);

		IInstanceNode* instanceNode = collectionNode->addInstance(instanceInfo.Static, 
			instanceInfo.Position, instanceInfo.Rotation, instanceInfo.Scaling);
		instanceNode->setTag(instanceInfo.Tag);
		if (instanceInfo.ShadowCasting)
			instanceNode->addShadow(1);
	}
}

void CSceneLoader::ReadLightNodes(ISceneManager* smgr, FILE* fp, u32 count)
{
	SSceneFilePointLightInfo fileinfo;
	for (u32 i = 0; i < count; i++)
	{
		fread(&fileinfo, sizeof(fileinfo), 1, fp);
		ILightNode* light = smgr->addPointLight(fileinfo.Id, nullptr, fileinfo.Static,
			fileinfo.Position, fileinfo.Range);

		light->setTag(fileinfo.Tag);
		light->setDiffuse(fileinfo.Diffuse);
		light->setSpecular(fileinfo.Specular);
		light->setAttenuation(fileinfo.Attenuations.x, fileinfo.Attenuations.y, fileinfo.Attenuations.z);
	}
}

GrassLand* CSceneLoader::buildGrassLand(ISceneManager* smgr, ITerrainNode* terrainNode)
{
	f32 range = 100.0f;
	f32 grassWidth = 5.0f;
	f32 grassHeight =3.0f;

	const u32 grassCount = 10000;
	const f32 grassY = 0;
	std::vector<XMFLOAT3> v(grassCount);
	for (u32 i = 0; i < grassCount; i++)
	{
		v[i].x = math::RandomFloat(-range, range);
		v[i].y = grassY;
		v[i].z = math::RandomFloat(-range, range);
	}

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, grassHeight * 0.5f, 0);
	aabb.Extents = XMFLOAT3(range + grassWidth * 0.5f, grassHeight, range + grassWidth * 0.5f);

	GrassLand* land = new GrassLand(smgr, terrainNode, v, range * 2, XMFLOAT2(grassWidth, grassHeight));
	return land;
}

bool CSceneLoader::BuildLandscape(RaceScene* scene, const SSceneFileHeader& header)
{
	// set terrain.
	ISceneManager* smgr = scene->mSceneManager;
	IMeshManager* meshManager = IMeshManager::getInstance();

	
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", header.TerrainHeightFile,
	header.TerrainVertexSpace, 0.15f, false, true, 1.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh);
	terrainNode->setMaterialName("terrain/grass_terrain_material");
	
	/*
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", header.TerrainHeightFile,
		header.TerrainVertexSpace, header.TerrainHeightScale, true, false, 1.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh);
	terrainNode->setMaterialName(header.TerrainMaterial);
	*/
	scene->mTerrainNode = terrainNode;

	// set grass land
	scene->mGrassLand = buildGrassLand(smgr, terrainNode);

	hkpRigidBodyCinfo groundInfo;

	hkpSampledHeightFieldBaseCinfo ci;

	// 这里的xRes,zRes应该不是指cellNum，而是地形每边的顶点数
	ci.m_xRes = terrainMesh->getRowCellNum() + 1;
	ci.m_zRes = terrainMesh->getRowCellNum() + 1;


	f32 heightScale = terrainMesh->getHeightScale();
	f32 widthScale = terrainMesh->getVertexSpace();
	hkReal scale = static_cast<hkReal>(widthScale);

	ci.m_scale.set(widthScale, 1.0f, widthScale);
	ci.m_maxHeight = heightScale * 255.0f + 1.0f;

	TerrainHeightFieldShape* heightFieldShape = new TerrainHeightFieldShape(ci, terrainMesh);

	// Wrap the heightfield in a hkpTriSampledHeightFieldCollection:
	hkpTriSampledHeightFieldCollection* collection = new hkpTriSampledHeightFieldCollection(heightFieldShape);

	// Now wrap the hkpTriSampledHeightFieldCollection in a hkpTriSampledHeightFieldBvTreeShape
	hkpTriSampledHeightFieldBvTreeShape* bvTree = new hkpTriSampledHeightFieldBvTreeShape(collection);

	groundInfo.m_shape = bvTree;
	groundInfo.m_position.set(-0.5f * (ci.m_xRes - 1) * ci.m_scale(0), 0, -0.5f * (ci.m_zRes - 1) * ci.m_scale(2));

	groundInfo.m_motionType = hkpMotion::MOTION_FIXED;
	groundInfo.m_friction = 0.9f;
	groundInfo.m_restitution = 0.9f;
	groundInfo.m_allowedPenetrationDepth = 0;
	hkpRigidBody* groundbody = new hkpRigidBody(groundInfo);

	heightFieldShape->removeReference();
	bvTree->removeReference();
	collection->removeReference();

	PhysicsEngine* engine = PhysicsEngine::getInstance();
	engine->getWorld()->markForWrite();
	engine->addRigidBody(groundbody);

	groundbody->removeReference();

	engine->getWorld()->unmarkForWrite();
	return true;
}
