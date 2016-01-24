#include "stdafx.h"
#include "gfUtil.h"
#include "CSceneManager.h"
#include "CMeshNode.h"
#include "CAnimatedMeshNode.h"
#include "CLightNode.h"
#include "CCameraNode.h"
#include "CFpsCameraNode.h"
#include "CModelMeshNode.h"
#include "COctreeManager.h"
#include "CTerrainNode.h"
#include "CEmptyNode.h"
#include "CMeshManager.h"
#include "CCompositor.h"
#include "CDirectionalLightNode.h"
#include "CPointLightNode.h"
#include "CInstanceCollectionNode.h"
#include "CReflectionPlane.h"

namespace gf
{
	CSceneManager::CSceneManager(IDevice* device, const math::SAxisAlignedBox& aabb)
		:ISceneManager(device, aabb)
		//, mMillisecondsDelta(0)
		, mSecondsDelta(0)
		, mSkyDomeNode(nullptr)
		, mQuadMeshNode(nullptr)
		, mDefaultOctree(nullptr)
		, mShadowMapCamera(nullptr)
		, mReflectCamera(nullptr)
		, mAmbient(0.5f, 0.5f, 0.5f, 1.0f)
		, mCurrentShadowLightID(0)
		, mDeferredShadingPipeline(nullptr)
		, mTileBasedDeferredShadingCS(nullptr)
		, mRenderingDeferredQuad(false)
	{
		//get Video Driver object
		mVideoDriver = device->getVideoDriver();

		// get material manager
		mMaterialManager = mVideoDriver->getMaterialManager();

		//get Resource Factory
		IResourceFactory* pResourceFactory = mVideoDriver->getResourceFactory();

		mCameraNodes.fill(nullptr);
		mReflectionPlanes.fill(nullptr);

		mShadowMapCamera = this->addCameraNode(SHADOW_CAMERA_ID);

		mReflectCamera = this->addCameraNode(REFLECT_CAMERA_ID);

		// create shadow map camera
		//mShadowMapCamera = this->addCameraNode(SHADOW_CAMERA_ID, nullptr, XMFLOAT3(0, 0, 0),
		//	XMFLOAT3(0, 0, 0.5), XMFLOAT3(0, 1.0f, 0), false, XM_PIDIV4, 0, 1000.0f);

		mActiveCameraId = EMPTY_CAMERA_ID;

		// create default octree.
		mDefaultOctree = this->addOctreeManagerNode(nullptr, aabb.Extents.x * 2.0f, aabb.Extents.y * 2.0f,
			aabb.Extents.z * 2.0f, aabb.Center, 8);

		// detach the octree from the scene manager.
		mDefaultOctree->remove();

		//setTileBasedDeferredShadingCS(nullptr);
		//setDeferredShadingPipeline(nullptr);
	}

	void CSceneManager::init()
	{

	}

	CSceneManager::~CSceneManager()
	{
		//mDefaultOctree->update();
		mDefaultOctree->destroy();

		//if (mSkyDomeNode)
		//	mSkyDomeNode->destroy();

		ReleaseReferenceCounted(mQuadMeshNode);
		ReleaseListElementCounted(mCompositors);
	}

	ISceneNode* CSceneManager::addEmptyNode(
		ISceneNode* parent,
		bool bStatic,
		const XMFLOAT3& position,
		const XMFLOAT3& rotation,
		const XMFLOAT3& scale)
	{
		parent = getParentNode(parent, bStatic);

		ISceneNode* node = new CEmptyNode(nullptr, this, position, rotation, scale);
		parent->addChild(node);

		return node;
	}


	IMeshNode* CSceneManager::addMeshNode(
		ISimpleMesh* mesh,
		IMaterial* material,
		ISceneNode* parent,
		bool bStatic,
		const XMFLOAT3& position,
		const XMFLOAT3& rotation,
		const XMFLOAT3& scale)
	{
		if (!mesh)
			return nullptr;

		parent = getParentNode(parent, bStatic);

		IMeshNode* node = new CMeshNode(nullptr, this, bStatic, mesh, material, position, rotation, scale);
		parent->addChild(node);

		return node;
	}

	IMeshNode* CSceneManager::addMeshNode(
		ISimpleMesh* mesh,
		const std::string& materialName,
		ISceneNode* parent,
		bool bStatic,
		const XMFLOAT3& position,
		const XMFLOAT3& rotation,
		const XMFLOAT3& scale)
	{
		IMaterial* material = mMaterialManager->get(materialName);
		if (!material) {
			GF_PRINT_CONSOLE_INFO("Material '%s' cound not be found.\n", materialName.c_str());
		}
		return this->addMeshNode(mesh, material, parent, bStatic,
			position, rotation, scale);
	}

	IMeshNode* CSceneManager::addModelMeshNode(
		IModelMesh* mesh,
		ISceneNode* parent,
		bool bStatic,
		const XMFLOAT3& position,
		const XMFLOAT3& rotation,
		const XMFLOAT3& scale)
	{
		if (!mesh)
			return nullptr;

		parent = getParentNode(parent, bStatic);

		IMeshNode* node = new CModelMeshNode(nullptr, this, bStatic, mesh, position, rotation, scale);
		parent->addChild(node);

		return node;
	}

	IAnimatedMeshNode* CSceneManager::addAnimatedMeshNode(
		IAnimatedMesh* mesh,
		ISceneNode* parent,
		bool bStatic,
		const XMFLOAT3& position,
		const XMFLOAT3& rotation,
		const XMFLOAT3& scale)
	{
		if (!mesh)
			return nullptr;

		parent = getParentNode(parent, bStatic);

		IAnimatedMeshNode* node = new CAnimatedMeshNode(nullptr, this, bStatic, mesh, position, rotation, scale);
		parent->addChild(node);

		return node;
	}

	IInstanceCollectionNode* CSceneManager::addInstanceCollectionNode(
		IMesh* mesh,
		ISceneNode* parent,
		u32 maxInstanceNum,
		u32 eachInstanceDataSize,
		const XMFLOAT3& position,
		const XMFLOAT3& rotation,
		const XMFLOAT3& scale)
	{
		if (!mesh)
			return nullptr;

		if (!parent)
			parent = this;

		CInstanceCollectionNode* node = new CInstanceCollectionNode(nullptr, this, position, rotation, scale);
		if (!node->init(mesh, maxInstanceNum, eachInstanceDataSize))
		{
			ReleaseReferenceCounted(node);
		}

		parent->addChild(node);
		return node;
	}


	ITerrainNode* CSceneManager::addTerrainNode(
		ITerrainMesh* mesh,
		IMaterial* material,
		ISceneNode* parent,
		const XMFLOAT3& position)
	{
		if (!mesh)
			return nullptr;

		if (!parent)
			parent = this;

		ITerrainNode* node = new CTerrainNode(parent, this, mesh, material, position);

		mTerrainNodes.push_back(node);

		return node;
	}


	ITerrainNode* CSceneManager::addTerrainNode(
		ITerrainMesh* mesh,
		const std::string& materialName,
		ISceneNode* parent,
		const XMFLOAT3& position)
	{
		IMaterial* material = mMaterialManager->get(materialName);
		if (!material) {
			GF_PRINT_CONSOLE_INFO("Material '%s' cound not be found.\n", materialName.c_str());
		}

		return this->addTerrainNode(mesh, material, parent, position);
	}

	IReflectionPlane* CSceneManager::addReflectionPlane(u32 id,
		XMFLOAT4 planeEquation, f32 planeSizeX, f32 planeSizeZ,
		u32 mapWidth, u32 mapHeight)
	{
		static u32 maxPlaneCount = mReflectionPlanes.size();
		if (id <= 0 || id >= maxPlaneCount)
		{
			GF_PRINT_CONSOLE_INFO("The reflection plane's id must be between 1 and %d.\n", 0, maxPlaneCount - 1);
			return nullptr;
		}

		if (mReflectionPlanes[id])
		{
			GF_PRINT_CONSOLE_INFO("The plane with id %d already existed!\n", id);
			return nullptr;
		}

		IReflectionPlane* plane = new CReflectionPlane(this, id, planeEquation,
			planeSizeX, planeSizeZ, mapWidth, mapHeight);

		mReflectionPlanes[id] = plane;
		return plane;
	}

	IReflectionPlane* CSceneManager::getReflectionPlane(u32 id)
	{
		static u32 maxPlaneCount = mReflectionPlanes.size();
		if (id <= 0 || id >= maxPlaneCount)
		{
			GF_PRINT_CONSOLE_INFO("The reflection plane's id must be between 1 and %d.\n", 0, maxPlaneCount - 1);
			return nullptr;
		}

		return mReflectionPlanes[id];
	}

	void CSceneManager::collectMeshNodeShaders(IMeshNode* node)
	{
		// collect all the shaders ( for per-frame variables optimization)
		E_PIPELINE_USAGE usage = mVideoDriver->getPipelineUsage();

		u32 subsetCount = node->getSubsetCount();
		for (u32 i = 0; i < subsetCount; i++)
		{
			IMaterial* material = node->getMaterial(i);
			if (material)
			{
				IPipeline* pipeline = material->getPipeline(usage);
				if (pipeline)
				{
					for (u32 k = 0; k < EST_SHADER_COUNT; k++)
					{
						IShader* shader = pipeline->getShader((E_SHADER_TYPE)k);
						if (shader != nullptr)
							mActiveShaders.insert(shader);
					}
				}
			}
		}
	}


	void CSceneManager::registerNodeForRendering(IMeshNode* node, E_NODE_TYPE nodeType)
	{
		if (!node->needCulling() || !isCulled(node))
		{
			if (!mVideoDriver->isRenderingShadowMap() ||
				node->isShadowCaster(mCurrentShadowLightID))
			{
				if (nodeType == ENT_SOLID_NODE)
				{
					collectMeshNodeShaders(node);
					mSolidNodes.push_back(node);
				}
				else if (nodeType == ENT_INSTANCE_NODE)
				{
					IInstanceNode* instanceNode = dynamic_cast<IInstanceNode*>(node);
					instanceNode->registerToCollectionForRendering();
				}
			}
		}

	}

	void CSceneManager::registerNodeForRendering(ILightNode* node)
	{
		// if not visible, it means that the light has been turned off.
		if (!node->isVisible())
			return;

		ICameraNode* camera = getActiveCameraNode();
		if (camera)
		{
			const math::SFrustum& frustum = camera->getFrustum();
			if (!node->needCulling() || !node->isCulled(frustum))
			{
				// if the light is in 


			}
		}
	}

	bool CSceneManager::isCulled(const IMeshNode* node)
	{
		const math::SOrientedBox& obb = node->getOrientedBox();
		ICameraNode* camera = getActiveCameraNode();
		if (!camera)
			return false;

		math::SFrustum frustum = camera->getFrustum();
		if (node->getFarCullingDistance() > 0)
		{
			frustum = camera->getFrustum(node->getFarCullingDistance());
		}

		if (math::IntersectOrientedBoxFrustum(obb, frustum) == math::EIS_OUTSIDE)
			return true;

		mRenderedMeshNum++;
		return false;
	}

	void CSceneManager::update(f32 delta)
	{
		mSecondsDelta = delta;
		//mSecondsDelta = static_cast<f32>(delta)* 0.001f;

		ITextureManager::getInstance()->updateTemporaryTextures(delta);

		ICameraNode* camera = getActiveCameraNode();
		if (camera && camera->getCameraType() == ECAT_FPS_CAMERA) {
			CFpsCameraNode* fpsCamera = dynamic_cast<CFpsCameraNode*>(camera);
			fpsCamera->beforeUpdate(delta);
		}

		// update skydome position
		if (mSkyDomeNode && camera)
		{
			mDefaultOctree->removeSceneNode(mSkyDomeNode);
			XMFLOAT3 pos = camera->getPosition();
			mSkyDomeNode->setPosition(pos.x, pos.y, pos.z);
			mDefaultOctree->addSceneNodeToOctree(mSkyDomeNode);
		}

		// remove all dynamic nodes from octree.
		mDefaultOctree->update(delta);

		// update all nodes.
		ISceneNode::update(delta);

		// add dynamic nodes to octree.
		mDefaultOctree->addSceneNodeToOctree(this);
	}

	void CSceneManager::render(E_PIPELINE_USAGE)
	{

	}

	void CSceneManager::draw(ISceneNode* node)
	{
		mSolidNodes.clear();
		mActiveShaders.clear();

		mRenderedMeshNum = 0;

		node->OnRegisterSceneNode();

		/* reset all the active shaders. (for per-frame variables optimizations.) */
		for (auto it = mActiveShaders.begin(); it != mActiveShaders.end(); it++)
		{
			(*it)->reset();
		}

		/* sort all the solid nodes according to the sort code. */
		std::sort(mSolidNodes.begin(), mSolidNodes.end(), [](ISceneNode* node1, ISceneNode* node2){
			return node1->getSortCode() < node2->getSortCode();
		});

		/* render all the solid nodes */
		auto it = mSolidNodes.begin();
		E_PIPELINE_USAGE usage = mVideoDriver->getPipelineUsage();
		for (; it != mSolidNodes.end(); it++)
		{
			(*it)->render(usage);
		}
	}

	void CSceneManager::draw(u32 tag)
	{
		mSolidNodes.clear();
		mActiveShaders.clear();

		mRenderedMeshNum = 0;

		mDefaultOctree->OnRegisterSceneNode(tag);

		/* reset all the active shaders. (for per-frame variables optimizations.) */
		for (auto it = mActiveShaders.begin(); it != mActiveShaders.end(); it++)
		{
			(*it)->reset();
		}

		/* sort all the solid nodes according to the sort code. */
		std::sort(mSolidNodes.begin(), mSolidNodes.end(), [](ISceneNode* node1, ISceneNode* node2){
			return node1->getSortCode() < node2->getSortCode();
		});

		/* render all the solid nodes */
		auto it = mSolidNodes.begin();
		E_PIPELINE_USAGE usage = mVideoDriver->getPipelineUsage();
		for (; it != mSolidNodes.end(); it++)
		{
			(*it)->render(usage);
		}
	}

	void CSceneManager::drawShadowMaps()
	{
		ICameraNode* activeCamera = getActiveCameraNode();
		const math::SFrustum& frustum = activeCamera->getFrustum();
		setActiveCamera(mShadowMapCamera);
		SViewport preViewport = mVideoDriver->getViewport();

		IRenderTarget* preRenderTarget = mVideoDriver->getRenderTarget();
		IDepthStencilSurface* preDepthStencilSurface = mVideoDriver->getDepthStencilSurface();

		mVideoDriver->clearShader(EST_PIXEL_SHADER);
		mVideoDriver->setRenderTargetAndDepthStencil(nullptr, nullptr);

		// generate all shadow maps.
		for (auto it = mLightNodes.begin(); it != mLightNodes.end(); it++)
		{
			ILightNode* light = it->second;
			if (light->castShadow() && !light->isCulled(frustum))
			{
				light->generateShadowMap(activeCamera);
			}
		}

		mVideoDriver->setPipelineUsage(EPU_FORWARD);
		mVideoDriver->setViewport(preViewport);
		//mVideoDriver->setDefaultRenderTargetAndDepthStencil();
		mVideoDriver->setRenderTargetAndDepthStencil(preRenderTarget, preDepthStencilSurface);
		setActiveCamera(activeCamera);
	}

	/* called by light objects */
	void CSceneManager::drawShadowMap(ILightNode* light)
	{
		mCurrentShadowLightID = light->getId();
		draw(mDefaultOctree);
	}

	void CSceneManager::drawReflectionMaps()
	{
		IRenderTarget* preRenderTarget = mVideoDriver->getRenderTarget();
		mVideoDriver->setRenderTarget(nullptr);

		ICameraNode* viewCamera = getActiveCameraNode();

		mReflectCamera->setPerspectiveProjection(viewCamera->isPerspectiveProjection());
		mReflectCamera->setAspectRatio(viewCamera->getAspectRatio());
		mReflectCamera->setFovAngle(viewCamera->getFovAngle());
		mReflectCamera->setNearZ(viewCamera->getNearZ());
		mReflectCamera->setFarZ(viewCamera->getFarZ());
		mReflectCamera->setViewWidth(viewCamera->getViewWidth());
		mReflectCamera->setViewHeight(viewCamera->getViewHeight());
		

		setActiveCamera(mReflectCamera);

		for (u32 i = 0; i < mReflectionPlanes.size(); i++) {
			IReflectionPlane* plane = mReflectionPlanes[i];
			if (plane) {
				plane->render(viewCamera);
			}
		}
	
		setActiveCamera(viewCamera);
		mVideoDriver->setRenderTarget(preRenderTarget);
		mVideoDriver->clearDepthStencil(1.0f, 0);
	}

	void CSceneManager::drawAll()
	{
		drawPass();
	}

	void CSceneManager::drawPass()
	{
		// adjust the skydome's position
		// update skydome position
		ICameraNode* camera = getActiveCameraNode();
		if (mSkyDomeNode && camera)
		{
			mDefaultOctree->removeSceneNode(mSkyDomeNode);
			XMFLOAT3 pos = camera->getPosition();
			mSkyDomeNode->setPosition(pos.x, pos.y, pos.z);
			mSkyDomeNode->updateAbsoluteTransformation();
			mDefaultOctree->addSceneNodeToOctree(mSkyDomeNode);
		}

		drawShadowMaps();

		if (mCompositors.empty())
		{
			if (mVideoDriver->isDeferredShading())
			{
				drawDeferredShading(mVideoDriver->getRenderTarget());
			}
			else
			{
				draw(mDefaultOctree);
			}
		}
		else
		{
			IRenderTarget* target = ITextureManager::getInstance()->getTempRenderTarget(0, 0, EGF_R8G8B8A8_UNORM);
			target->clear();

			if (mVideoDriver->isDeferredShading())
			{
				drawDeferredShading(target);
			}
			else
			{
				mVideoDriver->setRenderTarget(target);
				draw(mDefaultOctree);
			}
			
			for (u32 i = 0; i < mCompositors.size(); i++)
			{
				bool lastCompositor = false;
				if (i == mCompositors.size() - 1)
					lastCompositor = true;

				ICompositor* compositor = mCompositors[i];
				compositor->render(this, lastCompositor);
			}
		}
	}

	void CSceneManager::drawDeferredShading(IRenderTarget* target)
	{
		IMeshNode* quad = getQuadNode();
		static const u32 gBufferCount = IVideoDriver::MAX_GBUFFER_COUNT;
		static IRenderTarget* gbuffers[gBufferCount];

		mVideoDriver->setPipelineUsage(EPU_DEFERRED_SHADING);
		// set gbuffer as render targets.
		mVideoDriver->getGBuffers(gbuffers);

		for (u32 i = 0; i < gBufferCount; i++)
			gbuffers[i]->clear();

		mVideoDriver->setMultipleRenderTargets(gbuffers, gBufferCount);

		draw(mDefaultOctree);

		mVideoDriver->setPipelineUsage(EPU_FORWARD);

		IDepthStencilSurface* depthStencilSurface = mVideoDriver->getDepthStencilSurface();
		mRenderingDeferredQuad = true;

		E_DEFERRED_SHADING_ALGORITHM deferredShadingAlgorithm = mVideoDriver->getDeferredShadingAlgorithm();
		if (deferredShadingAlgorithm == EDSA_NORMAL_DEFERRED_SHADING)
		{
			IRenderTarget* targets[] = { target };
			mVideoDriver->setMultipleRenderTargets(targets, 1, nullptr);

			// set gBuffers and previous depth buffer as textures input.
			IPipeline* deferredPipeline = getDeferredShadingPipeline();
			SMaterial material(deferredPipeline);

			material.setTexture(0, gbuffers[0]->getTexture());
			material.setTexture(1, gbuffers[1]->getTexture());
			material.setTexture(2, gbuffers[2]->getTexture());
			material.setTexture(3, gbuffers[3]->getTexture());
			material.setTexture(4, depthStencilSurface->getTexture());
			
			quad->setMaterial(&material);

			ICameraNode* camera = getActiveCameraNode();
			if (camera)
			{
				math::SFrustum frustum = camera->getFrustum();
				mDefaultOctree->getLightsInFrustum(frustum, mDeferredShadingLights.PointLights);
			}

			material.setAttribute("TileColRow", XMFLOAT2(0.0f, 0.0f));
			material.setAttribute("TileNum", XMFLOAT2(1.0f, 1.0f));

			draw(quad);
		}
		else if (deferredShadingAlgorithm == EDSA_TILED_BASED_DEFERRED_SHADING)
		{
			IRenderTarget* targets[] = { target };
			mVideoDriver->setMultipleRenderTargets(targets, 1, nullptr);

			// set gBuffers and previous depth buffer as textures input.
			IPipeline* deferredPipeline = getDeferredShadingPipeline();
			SMaterial material(deferredPipeline);

			material.setTexture(0, gbuffers[0]->getTexture());
			material.setTexture(1, gbuffers[1]->getTexture());
			material.setTexture(2, gbuffers[2]->getTexture());
			material.setTexture(3, gbuffers[3]->getTexture());
			material.setTexture(4, depthStencilSurface->getTexture());

			quad->setMaterial(&material);

			const u32 RowTileNum = 16;
			const u32 ColTileNum = 16;

			f32 fRowTileSpace = 2.0f / RowTileNum;
			f32 fColTileSpace = 2.0f / ColTileNum;

			ICameraNode* camera = getActiveCameraNode();

			material.setAttribute("TileNum", XMFLOAT2(static_cast<f32>(ColTileNum), static_cast<f32>(RowTileNum)));

			for (u32 i = 0; i < RowTileNum; i++)
			{
				for (u32 j = 0; j < ColTileNum; j++)
				{
					XMFLOAT2 leftTop, rightDown;
					leftTop.x = j * fColTileSpace - 1.0f;
					leftTop.y = 1.0f - i * fRowTileSpace;

					rightDown.x = leftTop.x + fColTileSpace;
					rightDown.y = leftTop.y - fRowTileSpace;

					if (camera)
					{
						math::SFrustum frustum = camera->getFrustumFromProjSpace(leftTop, rightDown);
						mDefaultOctree->getLightsInFrustum(frustum, mDeferredShadingLights.PointLights);
					}

					material.setAttribute("TileColRow", XMFLOAT2(static_cast<f32>(j), static_cast<f32>(i)));
					draw(quad);
				}
			}
		}
		else if (deferredShadingAlgorithm == EDSA_CS_TILE_BASED_DEFERRED_SHADING)
		{
			const u32 maxPointsLightNum = 3000;
			const u32 maxDirLightNum = 100;
			static SPointLight pointLightsData[maxPointsLightNum];
			static SDirectionalLight dirLightsData[maxDirLightNum];

			u32 screenWidth = IDevice::getInstance()->getClientWidth();
			u32 screenHeight = IDevice::getInstance()->getClientHeight();
			const u32 ColTileNum = ceil(((float)screenWidth / 16));
			const u32 RowTileNum = ceil(((float)screenHeight / 16));

			ITextureManager* textureManager = ITextureManager::getInstance();

			std::string outputTextureName("cs_tile_based_deferred_shading_output");
			std::string pointLightsBufferName("cs_ds_point_lights_buffer");
			std::string dirLightsBufferName("cs_ds_dir_lights_buffer");

			ITexture* outputTexture = textureManager->get(outputTextureName, false);
			if (!outputTexture)
			{
				outputTexture = textureManager->createTexture2D(outputTextureName, screenWidth, screenHeight,
					ETBT_SHADER_RESOURCE | ETBT_UNORDERED_ACCESS, nullptr, 1, EGF_R8G8B8A8_UNORM,
					0, EMU_DEFAULT);
			}
			
			//create point lights buffer in Compute Shader.
			IBuffer* pointLightsBuffer = textureManager->getBuffer(pointLightsBufferName);
			if (!pointLightsBuffer)
			{
				pointLightsBuffer = textureManager->createBuffer(pointLightsBufferName, maxPointsLightNum,
					ETBT_SHADER_RESOURCE | ETBT_CPU_ACCESS_WRITE, EGF_UNKNOWN, sizeof(SPointLight), nullptr);
			}
			
			// create dir lights buffer in Compute Shader.
			IBuffer* dirLightsBuffer = textureManager->getBuffer(dirLightsBufferName);
			if (!dirLightsBuffer)
			{
				dirLightsBuffer = textureManager->createBuffer(dirLightsBufferName, maxDirLightNum,
					ETBT_SHADER_RESOURCE | ETBT_CPU_ACCESS_WRITE, EGF_UNKNOWN, sizeof(SDirectionalLight), nullptr);
			}
			
			IShader* shader = getTileBasedDeferredShadingCS();

			
			// set Render targets to null
			IRenderTarget* nulltargets[] = { 0, 0, 0, 0 };
			mVideoDriver->setMultipleRenderTargets(nulltargets, 4, nullptr);
			
			shader->setTexture("gGBuffer0", gbuffers[0]->getTexture());
			shader->setTexture("gGBuffer1", gbuffers[1]->getTexture());
			shader->setTexture("gGBuffer2", gbuffers[2]->getTexture());
			shader->setTexture("gGBuffer3", gbuffers[3]->getTexture());
			shader->setTexture("gDepthBuffer", depthStencilSurface->getTexture());
			shader->setTexture("gOutputTexture", outputTexture);

			ICameraNode* camera = getActiveCameraNode();
			math::SFrustum frustum = camera->getFrustum();

			ITimer* timer = mDevice->getTimer();
			f64 start = timer->getMilliseconds();
			mDefaultOctree->getLightsInFrustum(frustum, mDeferredShadingLights.PointLights);
			
			/*
			for (auto it = mDirectionalLights.begin(); it != mDirectionalLights.end(); it++) {
				ILightNode* light = *it;
				if (light->isVisible())
					mDeferredShadingLights.DirLights.push_back(light);
			}
			*/

			u32 pointLightCount = mDeferredShadingLights.PointLights.size();
			//u32 dirLightCount = mDeferredShadingLights.DirLights.size();

			if (pointLightCount > 0)
			{
				for (u32 i = 0; i < pointLightCount; i++)
					mDeferredShadingLights.PointLights[i]->getLightData(&pointLightsData[i]);

				STextureData texData;
				pointLightsBuffer->lock(ETLT_WRITE_DISCARD, &texData);
				memcpy(texData.Data, pointLightsData, pointLightCount * sizeof(SPointLight));
				pointLightsBuffer->unlock();
			}

			/*
			if (dirLightCount > 0)
			{
				for (u32 i = 0; i < dirLightCount; i++)
					mDeferredShadingLights.DirLights[i]->getLightData(&dirLightsData[i]);

				STextureData texData;
				dirLightsBuffer->lock(ETLT_WRITE_DISCARD, &texData);
				memcpy(texData.Data, dirLightsData, dirLightCount * sizeof(SDirectionalLight));
				dirLightsBuffer->unlock();
			}
			*/

			shader->setTexture("gPointLights", pointLightsBuffer);
			shader->setUint("gPointLightsNum", pointLightCount);
			//shader->setTexture("gDirLights", dirLightsBuffer);
			//shader->setUint("gDirLightsNum", dirLightCount);
			XMFLOAT4 tileNums;
			tileNums.x = ColTileNum;
			tileNums.y = RowTileNum;
			tileNums.z = 1.0f / ColTileNum;
			tileNums.w = 1.0f / RowTileNum;
			shader->setVector("gTilesNum", tileNums);

			mVideoDriver->resetTextures(EST_PIXEL_SHADER);
			mVideoDriver->runComputeShader(shader, ColTileNum, RowTileNum, 1, this);
			mVideoDriver->resetRWTextures();
			mVideoDriver->resetTextures(EST_COMPUTE_SHADER);
			
			IRenderTarget* targets[] = { target };
			mVideoDriver->setMultipleRenderTargets(targets, 1, nullptr);
			
			if (!mVideoDriver->isDeferredAntiAliasing()) {
				std::string postProcessPipelineName = "gf/default_cs_tile_based_ds_post_process";
				IPipeline* postProcessPipeline = IPipelineManager::getInstance()->get(postProcessPipelineName, true);
				SMaterial material(postProcessPipeline);
				material.setTexture(0, outputTexture);
				quad->setMaterial(&material);
				draw(quad);
				quad->setMaterial(nullptr);
			}
			else {
				std::string postProcessPipelineName = "gf/deferred_anti_aliasing_post_process";
				IPipeline* postProcessPipeline = IPipelineManager::getInstance()->get(postProcessPipelineName, true);
				SMaterial material(postProcessPipeline);
				material.setTexture(0, outputTexture);
				material.setTexture(1, depthStencilSurface->getTexture());
				material.setTexture(2, gbuffers[0]->getTexture());
				quad->setMaterial(&material);
				draw(quad);
				quad->setMaterial(nullptr);
			}

			mVideoDriver->setDepthStencilSurface(depthStencilSurface);

			f64 end = timer->getMilliseconds();
			//std::cout << end - start << std::endl;
		}


		mRenderingDeferredQuad = false;
	}

	ILightNode* CSceneManager::addDirectionalLight(u32 id, ISceneNode* parent, const XMFLOAT3& direction)
	{
		ILightNode* light = getLightNode(id);
		if (light)
		{
			GF_PRINT_CONSOLE_INFO("The light with id %d already existed!\n", id);
			return nullptr;
		}

		if (!parent)
			parent = this;

		light = new CDirectionalLightNode(parent, this, id, direction);
		mLightNodes.insert(std::make_pair(id, light));
		mDirectionalLights.push_back(light);

		return light;
	}

	ILightNode* CSceneManager::addPointLight(u32 id,
		ISceneNode* parent,
		bool bStatic,
		const XMFLOAT3& position,
		f32 range)
	{
		ILightNode* light = getLightNode(id);
		if (light)
		{
			GF_PRINT_CONSOLE_INFO("The light with id %d already existed!\n", id);
			return nullptr;
		}

		light = new CPointLightNode(nullptr, this, bStatic, id, position, range);
		parent = getParentNode(parent, bStatic);
		parent->addChild(light);
		mLightNodes.insert(std::make_pair(id, light));
		return light;
	}

	ILightNode* CSceneManager::addSpotLight(u32 id,
		ISceneNode* parent,
		bool bStatic,
		const XMFLOAT3& position,
		const XMFLOAT3& direction,
		f32 range,
		f32 innerCone,
		f32 outerCone)
	{
		return nullptr;
	}

	ILightNode* CSceneManager::getLightNode(u32 id)
	{
		auto it = mLightNodes.find(id);
		if (it == mLightNodes.end())
			return nullptr;

		return it->second;
	}

	ICameraNode* CSceneManager::addCameraNode(u32 id,
		ISceneNode* parent,
		const XMFLOAT3& position,
		const XMFLOAT3& lookat,
		const XMFLOAT3& up,
		bool bPersectiveProj,
		f32 fov,
		f32 nearZ,
		f32 farZ,
		f32 aspectRatio)
	{
		if (parent == nullptr)
			parent = this;

		if (id <= 0 || id >= MAX_CAMERA_COUNT)
		{
			GF_PRINT_CONSOLE_INFO("The camera id must between 1 and %d.\n", 0, MAX_CAMERA_COUNT - 1);
			return nullptr;
		}

		if (mCameraNodes[id] != nullptr)
		{
			GF_PRINT_CONSOLE_INFO("The camera with id %d already existed!\n", id);
			return nullptr;
		}

		/* if the aspectRatio is the default param. */
		if (aspectRatio < 0)
		{
			f32 width = static_cast<f32>(mDevice->getClientWidth());
			f32 height = static_cast<f32>(mDevice->getClientHeight());
			aspectRatio = width / height;
		}

		ICameraNode* camera = new CCameraNode(parent, this, position,
			lookat, up, aspectRatio, fov, nearZ, farZ, bPersectiveProj);
		mCameraNodes[id] = camera;

		if (mActiveCameraId == EMPTY_CAMERA_ID)
			setActiveCamera(id);

		return camera;
	}

	IFpsCameraNode* CSceneManager::addFpsCameraNode(u32 id,
		ISceneNode* parent,
		const XMFLOAT3& position,
		const XMFLOAT3& lookat,
		const XMFLOAT3& up,
		bool bPersectiveProj,
		f32 maxUpAngle,
		f32 maxDownAngle,
		f32 fov,
		f32 nearZ,
		f32 farZ,
		f32 aspectRatio)
	{
		if (parent == nullptr)
			parent = this;

		if (id <= 0 || id >= MAX_CAMERA_COUNT)
		{
			GF_PRINT_CONSOLE_INFO("The camera id must between 1 and %d.\n", 0, MAX_CAMERA_COUNT - 1);
			return nullptr;
		}

		if (mCameraNodes[id] != nullptr)
		{
			GF_PRINT_CONSOLE_INFO("The camera with id %d already existed!\n", id);
			return nullptr;
		}

		/* if the aspectRatio is the default param. */
		if (aspectRatio < 0)
		{
			f32 width = static_cast<f32>(mVideoDriver->getBackBufferWidth());
			f32 height = static_cast<f32>(mVideoDriver->getBackBufferHeight());
			aspectRatio = width / height;
		}

		IFpsCameraNode* camera = new CFpsCameraNode(parent, this, position, lookat, up, aspectRatio,
			fov, nearZ, farZ, maxUpAngle, maxDownAngle, bPersectiveProj);

		mCameraNodes[id] = camera;
		if (mActiveCameraId == EMPTY_CAMERA_ID)
			setActiveCamera(id);

		return camera;
	}

	ICameraNode* CSceneManager::getActiveCameraNode() const
	{
		return mCameraNodes[0];
	}

	u32 CSceneManager::getActiveCameraId() const
	{
		return mActiveCameraId;
	}

	ICameraNode* CSceneManager::getCameraNode(u32 id)
	{
		if (id >= MAX_CAMERA_COUNT)
			return nullptr;

		return mCameraNodes[id];
	}

	ICameraNode* CSceneManager::setActiveCamera(u32 id)
	{
		if (id >= MAX_CAMERA_COUNT || !mCameraNodes[id])
			return nullptr;

		mActiveCameraId = id;
		mCameraNodes[0] = mCameraNodes[id];
		return mCameraNodes[0];
	}

	bool CSceneManager::setActiveCamera(ICameraNode* camera)
	{
		if (!camera)
			return false;

		for (u32 i = 1; i < MAX_CAMERA_COUNT; i++)
		{
			if (mCameraNodes[i] == camera)
			{
				mActiveCameraId = i;
				mCameraNodes[0] = camera;
				return true;
			}
		}

		return false;
	}

	IOctreeManager* CSceneManager::addOctreeManagerNode(ISceneNode* parent,
		f32 width,
		f32 height,
		f32 depth,
		XMFLOAT3 center,
		u32 maxTreeHeight)
	{
		if (!parent)
			parent = this;

		IOctreeManager* node = new COctreeManager(parent, this, width, height, depth, center, maxTreeHeight);

		return node;
	}

	void CSceneManager::setSkyDome(const std::string& textureName)
	{
		ITextureManager* textureManager = mVideoDriver->getTextureManager();
		ITextureCube* skyTexture = textureManager->getTextureCube(textureName);

		setSkyDome(skyTexture);
	}

	void CSceneManager::setSkyDome(ITextureCube* cubeTexture)
	{
		std::string materialName = "gf/skydome_material";

		if (cubeTexture)
		{
			if (mSkyDomeNode == nullptr)
			{
				IMaterialManager* materialManager = IMaterialManager::getInstance();
				
				IMaterial* material = materialManager->get(materialName, false);
				if (!material)
				{
					IPipeline* pipeline = IPipelineManager::getInstance()->get("gf/skydome");
					material = materialManager->create(materialName, pipeline);
				}
				material->setTexture(0, cubeTexture);

				ISimpleMesh* mesh = IMeshManager::getInstance()->getSimpleMesh(IMeshManager::SKYDOME);
				if (!mesh)
				{
					mesh = IMeshManager::getInstance()->createSphereMesh(IMeshManager::SKYDOME);
				}

				mSkyDomeNode = addMeshNode(mesh, material);
				mSkyDomeNode->setRenderOrder(ERO_SKYDOME);
				mSkyDomeNode->setNeedCulling(false);
				mSkyDomeNode->setTag(EN_TAG_SKYDOME);
			}
			else
			{
				IMaterial* material = mSkyDomeNode->getMaterial();
				material->setTexture(0, cubeTexture);
				this->addChild(mSkyDomeNode);
			}
		}
		else
		{
			if (mSkyDomeNode)
			{
				mSkyDomeNode->remove();
			}
		}
	}

	IMeshNode* CSceneManager::getSkyNode()
	{
		return mSkyDomeNode;
	}

	IMeshNode* CSceneManager::getQuadNode()
	{
		if (!mQuadMeshNode)
		{
			IMeshManager* meshManager = IMeshManager::getInstance();
			ISimpleMesh* mesh = meshManager->getSimpleMesh(IMeshManager::QUAD);
			if (!mesh)
			{
				mesh = meshManager->createQuad(IMeshManager::QUAD);
			}

			mQuadMeshNode = addMeshNode(mesh, nullptr);
			mQuadMeshNode->remove();
			mQuadMeshNode->setNeedCulling(false);
		}

		return mQuadMeshNode;
	}




	ICompositor* CSceneManager::createCompositor(IPipeline* pipeline)
	{
		ICompositor* compositor = new CCompositor(pipeline);
		return compositor;
	}

	ICompositor* CSceneManager::createCompositor(u32 type, const SCompositorCreateParam& param)
	{
		ICompositorFactory* factory = mVideoDriver->getCompositorFactory();
		return factory->createCompositor(type, param);
	}

	void CSceneManager::addCompositor(ICompositor* compositor)
	{
		if (compositor)
			mCompositors.push_back(compositor);
	}

	ISceneNode* CSceneManager::getParentNode(ISceneNode* parent, bool bStatic)
	{
		if (bStatic)
		{
			return mDefaultOctree;
		}

		if (!parent)
		{
			return this;
		}

		return parent;
	}

	void CSceneManager::registerToOctree(ISceneNode* scene)
	{
		if (scene->getNodeType() & ESNT_OCTREE_MANAGER)
			return;
	}

	bool CSceneManager::getNearLights(IMeshNode* node, E_LIGHT_TYPE lightType, std::vector<ILightNode*>& lights)
	{
		if (mRenderingDeferredQuad)
		{
			lights = mDeferredShadingLights.PointLights;
			return true;
		}

		return mDefaultOctree->getNearLights(node, lightType, lights);
	}

	void CSceneManager::setDeferredShadingPipeline(IPipeline* pipeline)
	{
		// if pipeline is null, then set the default deferred shading pipeline.
		if (!pipeline)
		{
			mDeferredShadingPipeline = IPipelineManager::getInstance()->get("gf/default_deferred_pipeline");
		}
		else
		{
			mDeferredShadingPipeline = pipeline;
		}
	}

	void CSceneManager::setDeferredShadingPipeline(const std::string& name)
	{
		IPipeline* pipeline = IPipelineManager::getInstance()->get(name);
		if (pipeline)
		{
			mDeferredShadingPipeline = pipeline;
		}
	}

	IPipeline* CSceneManager::getDeferredShadingPipeline()
	{
		if (!mDeferredShadingPipeline)
		{
			setDeferredShadingPipeline(nullptr);
		}
		return mDeferredShadingPipeline;
	}

	IShader* CSceneManager::getTileBasedDeferredShadingCS()
	{
		if (!mTileBasedDeferredShadingCS)
		{
			setTileBasedDeferredShadingCS(nullptr);
		}
		return mTileBasedDeferredShadingCS;
	}

	bool CSceneManager::setTileBasedDeferredShadingCS(IShader* shader)
	{
		if (!shader)
		{
			IShaderManager* shaderManager = IShaderManager::getInstance();
			shader = shaderManager->get("tile_based_deferred_shading.hlsl", "cs_main", true, EST_COMPUTE_SHADER);
			if (!shader)
				return false;
		}

		if (shader->getType() != EST_COMPUTE_SHADER)
			return false;

		mTileBasedDeferredShadingCS = shader;
		return true;
	}

	math::SRay CSceneManager::getPickingRay(u32 sx, u32 sy) const
	{
		SViewport viewport = mVideoDriver->getViewport();
		
		ICameraNode* camera = getActiveCameraNode();
		XMFLOAT4X4 proj = camera->getProjMatrix();
		XMFLOAT4X4 view = camera->getViewMatrix();

		f32 vx = (2.0f * (sx - viewport.TopLeftX) / viewport.Width - 1.0f) / proj(0, 0);
		f32 vy = (-2.0f * (sy - viewport.TopLeftY) / viewport.Height + 1.0f) / proj(1, 1);
		
		XMVECTOR dir = XMVectorSet(vx, vy, 1.0f, 0);
		XMMATRIX viewMatrix = XMLoadFloat4x4(&view);
		XMMATRIX invViewMatrix = XMMatrixInverse(&XMMatrixDeterminant(viewMatrix), viewMatrix);

		dir = XMVector3TransformNormal(dir, invViewMatrix);
		dir = XMVector3Normalize(dir);

		math::SRay ray;
		XMStoreFloat3(&ray.Direction, dir);
		ray.Origin = XMFLOAT3(invViewMatrix._41, invViewMatrix._42, invViewMatrix._43);
		return ray;
	}

	ISceneNode* CSceneManager::intersectRay(const math::SRay& ray, f32* pDist,
		u32 nodeType /*= ENT_SOLID_NODE | ENT_LIGHT_NODE*/) const
	{
		return mDefaultOctree->intersectRay(ray, pDist, nodeType);
	}

	ISceneNode* CSceneManager::intersectRayWithTag(const math::SRay& ray,
		f32* pDist, u32 tag,
		u32 nodeType) const
	{
		return mDefaultOctree->intersectRayWithTag(ray, pDist, tag, nodeType);
	}

	void CSceneManager::draw2DImage(ITexture* texture,
		const math::Rect<f32>& sourceRect,
		const math::Rect<f32>& destRect,
		XMFLOAT4 color,
		bool useAlphaChannelOfTexture)
	{
		IMaterialManager* materialManager = IMaterialManager::getInstance();
		IMaterial* material = materialManager->get("gf/2d_image_material");
		material->setTexture(0, texture);

		XMFLOAT4 fSourceRect, fDestRect;
		fSourceRect.x = sourceRect.x1;
		fSourceRect.y = sourceRect.y1;
		fSourceRect.z = sourceRect.x2;
		fSourceRect.w = sourceRect.y2;

		fDestRect.x = destRect.x1;
		fDestRect.y = destRect.y1;
		fDestRect.z = destRect.x2;
		fDestRect.w = destRect.y2;

		material->setAttribute("SourceRect", fSourceRect);
		material->setAttribute("DestRect", fDestRect);

		IMeshNode* quadNode = getQuadNode();
		quadNode->setMaterial(material);
		draw(quadNode);
	}

	void CSceneManager::draw2DImage(ITexture* texture,
		const math::Rect<s32>& sourceRect,
		const math::Rect<s32>& destRect,
		XMFLOAT4 color,
		bool useAlphaChannelOfTexture)
	{
		IMaterialManager* materialManager = IMaterialManager::getInstance();
		IMaterial* material = materialManager->get("gf/2d_image_material");
		material->setTexture(0, texture);
		
		f32 w = mVideoDriver->getViewport().Width;
		f32 h = mVideoDriver->getViewport().Height;

		XMFLOAT4 fSourceRect, fDestRect;
		fSourceRect.x = sourceRect.x1 / texture->getWidth();
		fSourceRect.y = sourceRect.y1 / texture->getHeight();
		fSourceRect.z = sourceRect.x2 / texture->getWidth();
		fSourceRect.w = sourceRect.y2 / texture->getHeight();

		fDestRect.x = destRect.x1 / w;
		fDestRect.y = destRect.y1 / h;
		fDestRect.z = destRect.x2 / w;
		fDestRect.w = destRect.y2 / h;

		material->setAttribute("SourceRect", fSourceRect);
		material->setAttribute("DestRect", fDestRect);

		IMeshNode* quadNode = getQuadNode();
		quadNode->setMaterial(material);
		draw(quadNode);
	}

	bool CSceneManager::getHeightOnTerrain(f32 x, f32 z, f32& height) const
	{
		bool found = false;
		for (auto it = mTerrainNodes.begin(); it != mTerrainNodes.end(); it++) {
			ITerrainNode* terrain = *it;
			if (terrain->isInsideTerrainScope(x, z)) {
				if (!found)
				{
					found = true;
					height = terrain->getHeight(x, z);
				}
				else
				{
					height = max(height, terrain->getHeight(x, z));
				}
			}
		}
		return found;
	}

}

