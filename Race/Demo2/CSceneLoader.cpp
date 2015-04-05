#include "stdafx.h"
#include "CSceneLoader.h"
#include "GrassLand.h"
#include "RaceScene.h"
#include "TerrainHeightFieldShape.h"
#include "PhysicsEngine.h"
#include "PhysicsLoader.h"

bool CSceneLoader::load(const char* filename, RaceScene* scene, SPhysicalLoadingInfo& physicalInfo)
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
	//driver->setDeferredShading(true);
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

	BuildLandscape(scene, header, physicalInfo);
	
	ReadMeshNodes(smgr, fp, header.MeshNodeCount, physicalInfo);
	ReadCollectionNodes(smgr, fp, header.CollectionNodeCount, physicalInfo);
	ReadLightNodes(smgr, fp, header.LightNodeCount);

	fclose(fp);

	return true;
}

void CSceneLoader::ReadMeshNodes(ISceneManager* smgr, FILE* fp, u32 meshCount,
	SPhysicalLoadingInfo& physicalInfo)
{
	IMeshManager* meshManager = IMeshManager::getInstance();

	SSceneFileMeshNodeInfo info;
	for (u32 i = 0; i < meshCount; i++)
	{
		fread(&info, sizeof(info), 1, fp);
		if (info.Category != MESH_CATEGORY)
			continue;

		std::string meshName = info.MeshName;
		IModelMesh* mesh = meshManager->getModelMesh(meshName, true);
		if (!mesh)
			continue;

		IMeshNode* node = smgr->addModelMeshNode(mesh, nullptr, info.Static,
			info.Position, info.Rotation, info.Scaling);

		node->setTag(info.Tag);
		if (info.ShadowCasting)
			node->addShadow(1);

		if (info.BoundingPhysics)
			AddPhysicalBoundings(meshName, info.Static, info.Position, 
				info.Rotation, info.Scaling, physicalInfo, smgr);
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

void CSceneLoader::ReadCollectionNodes(ISceneManager* smgr, FILE* fp, u32 count,SPhysicalLoadingInfo& physicalInfo)
{
	for (u32 i = 0; i < count; i++)
	{
		ReadCollectionNode(smgr, fp, physicalInfo);
	}
}

void CSceneLoader::ReadCollectionNode(ISceneManager* smgr, FILE* fp, SPhysicalLoadingInfo& physicalInfo)
{
	IMeshManager* meshManager = IMeshManager::getInstance();

	SSceneFileMeshNodeInfo fileinfo;
	fread(&fileinfo, sizeof(fileinfo), 1, fp);
	u32 instanceCount;
	fread(&instanceCount, sizeof(u32), 1, fp);

	std::string meshName = fileinfo.MeshName;
	IModelMesh* mesh = meshManager->getModelMesh(meshName, true);
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

		
		if (instanceInfo.BoundingPhysics)
			AddPhysicalBoundings(meshName, instanceInfo.Static, instanceInfo.Position,
			instanceInfo.Rotation, instanceInfo.Scaling, physicalInfo, smgr);
		
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

bool CSceneLoader::BuildLandscape(RaceScene* scene, const SSceneFileHeader& header, SPhysicalLoadingInfo& physicalInfo)
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
	physicalInfo.TerrainMesh = terrainMesh;

	// set grass land
	scene->mGrassLand = buildGrassLand(smgr, terrainNode);
	

	return true;
}

void CSceneLoader::AddPhysicalBoundings(const std::string& meshName, bool bStatic,
	const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scaling,
	SPhysicalLoadingInfo& physicalInfo,
	ISceneManager* smgr)
{
	SPhysicalBoundingCollection* collection = PhysicsLoader::GetBoundings(meshName);

	for (u32 i = 0; i < collection->Boundings.size(); i++)
	{
		const SPhysicalBounding* bounding = &collection->Boundings[i];
		if (bounding->Category == BOX_BOUNDING)
			AddBoxPhysicalBounding(bounding, bStatic, position, rotation, scaling, physicalInfo,smgr);
	}
}

void CSceneLoader::AddBoxPhysicalBounding(const SPhysicalBounding* bounding, bool bStatic,
	const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scaling,
	SPhysicalLoadingInfo& physicalInfo,
	ISceneManager* smgr)
{
	XMMATRIX rot1 = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX rot2 = XMMatrixRotationRollPitchYaw(bounding->Rotation.x, bounding->Rotation.y, bounding->Rotation.z);
	XMMATRIX R = rot2 * rot1;
	
	XMMATRIX tran1 = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX tran2 = XMMatrixTranslation(bounding->Center.x, bounding->Center.y, bounding->Center.z);
	
	XMMATRIX M = tran2 * rot1 * tran1;
	XMFLOAT4X4 T;
	XMStoreFloat4x4(&T, M);

	XMFLOAT3 pos = XMFLOAT3(T._41, T._42, T._43);
	XMVECTOR quat_v = XMQuaternionRotationMatrix(R);
	XMFLOAT4 quat;
	XMStoreFloat4(&quat, quat_v);

	XMFLOAT3 boxSize(bounding->Box.Size[0] * scaling.x * 0.5f, 
		bounding->Box.Size[1] * scaling.y * 0.5f, 
		bounding->Box.Size[2] * scaling.z * 0.5f);
	/*
	hkpBoxShape* boxShape = new hkpBoxShape(boxSize, 0);
	hkpRigidBodyCinfo bodyInfo;
	bodyInfo.m_shape = boxShape;
	bodyInfo.m_motionType = (bStatic) ? hkpMotion::MOTION_FIXED : hkpMotion::MOTION_DYNAMIC;
	bodyInfo.m_position.set(pos.x, pos.y, pos.z);
	bodyInfo.m_mass = bounding->Mass;
	bodyInfo.m_friction = bounding->Friction;
	bodyInfo.m_rotation.set(quat.x, quat.y, quat.z, quat.w);
	hkpRigidBody* boxBody = new hkpRigidBody(bodyInfo);
	PhysicsEngine* engine = PhysicsEngine::getInstance();
	engine->getWorld()->markForWrite();
	PhysicsEngine::getInstance()->addRigidBody(boxBody);
	engine->getWorld()->unmarkForWrite();
	boxShape->removeReference();
	boxBody->removeReference();
	*/
	SPhysicalBoxLoadingInfo boxInfo;
	boxInfo.HalfSize = boxSize;
	boxInfo.Static = bStatic;
	boxInfo.Position = pos;
	boxInfo.Rotation = quat;
	boxInfo.Mass = bounding->Mass;
	boxInfo.Friction = bounding->Friction;
	physicalInfo.Boxes.push_back(boxInfo);

	/*
	IMeshManager* meshManager = IMeshManager::getInstance();
	ISimpleMesh* mesh = meshManager->getSimpleMesh("debug_box");
	if (!mesh)
	{
		mesh = meshManager->createCubeMesh("debug_box");
	}

	IMeshNode* node = smgr->addMeshNode(mesh, nullptr);
	node->scale(bounding->Box.Size[0] * scaling.x, bounding->Box.Size[1] * scaling.y,
		bounding->Box.Size[2] * scaling.z);

	node->transform(R);
	node->translate(pos.x, pos.y, pos.z);
	node->setMaterialName("picking_cube_material");
	node->setTag(4);
	*/
}

void CSceneLoader::LoadPhysics(SPhysicalLoadingInfo& physicalInfo)
{
	PhysicsEngine* engine = PhysicsEngine::getInstance();

	hkpSampledHeightFieldBaseCinfo ci;

	ITerrainMesh* terrainMesh = physicalInfo.TerrainMesh;

	// 这里的xRes,zRes应该不是指cellNum，而是地形每边的顶点数
	ci.m_xRes = terrainMesh->getRowCellNum() + 1;
	ci.m_zRes = terrainMesh->getRowCellNum() + 1;

	f32 heightScale = terrainMesh->getHeightScale();
	f32 widthScale = terrainMesh->getVertexSpace();
	hkReal scale = static_cast<hkReal>(widthScale);

	ci.m_scale.set(widthScale, 1.0f, widthScale);
	ci.m_maxHeight = heightScale * 255.0f + 1.0f;

	hkpRigidBodyCinfo groundInfo;
	TerrainHeightFieldShape* heightFieldShape = new TerrainHeightFieldShape(ci,
		terrainMesh);

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
	
	engine->getWorld()->markForWrite();
	engine->addRigidBody(groundbody);
	groundbody->removeReference();
	engine->getWorld()->unmarkForWrite();


	for (u32 i = 0; i < physicalInfo.Boxes.size(); i++)
	{
		SPhysicalBoxLoadingInfo& info = physicalInfo.Boxes[i];
		hkVector4 halfSize(info.HalfSize.x, info.HalfSize.y, info.HalfSize.z);
		hkpBoxShape* boxShape = new hkpBoxShape(halfSize, 0);

		hkpRigidBodyCinfo ci;
		ci.m_shape = boxShape;
		ci.m_motionType = (info.Static) ? hkpMotion::MOTION_FIXED : hkpMotion::MOTION_DYNAMIC;
		ci.m_position.set(info.Position.x, info.Position.y, info.Position.z);
		ci.m_mass = info.Mass;
		ci.m_friction = info.Friction;
		ci.m_rotation.set(info.Rotation.x, info.Rotation.y, info.Rotation.z, info.Rotation.w);

		hkpRigidBody* boxBody = new hkpRigidBody(ci);
		PhysicsEngine* engine = PhysicsEngine::getInstance();
		engine->getWorld()->markForWrite();
		engine->addRigidBody(boxBody);
		engine->getWorld()->unmarkForWrite();
		boxShape->removeReference();
		boxBody->removeReference();
	}

}
