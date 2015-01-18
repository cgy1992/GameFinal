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

namespace gf
{
	CSceneManager::CSceneManager(IDevice* device, const math::SAxisAlignedBox& aabb)
		:ISceneManager(device, aabb)
		, mMillisecondsDelta(0)
		, mSecondsDelta(0)
		, mSkyDomeNode(nullptr)
		, mQuadMeshNode(nullptr)
		, mDefaultOctree(nullptr)
		, mShadowMapCamera(nullptr)
		, mAmbient(0.5f, 0.5f, 0.5f, 1.0f)
		, mCurrentShadowLightID(0)
		, mDeferredShadingPipeline(nullptr)
		, mRenderingDeferredQuad(false)
	{
		//get Video Driver object
		mVideoDriver = device->getVideoDriver();

		//get Resource Factory
		IResourceFactory* pResourceFactory = mVideoDriver->getResourceFactory();

		mCameraNodes.fill(nullptr);
		
		mShadowMapCamera = this->addCameraNode(SHADOW_CAMERA_ID);

		// create shadow map camera
		//mShadowMapCamera = this->addCameraNode(SHADOW_CAMERA_ID, nullptr, XMFLOAT3(0, 0, 0),
		//	XMFLOAT3(0, 0, 0.5), XMFLOAT3(0, 1.0f, 0), false, XM_PIDIV4, 0, 1000.0f);
		
		mActiveCameraId = EMPTY_CAMERA_ID;
		
		// create default octree.
		mDefaultOctree = this->addOctreeManagerNode(nullptr, aabb.Extents.x * 2.0f, aabb.Extents.y * 2.0f,
			aabb.Extents.z * 2.0f, aabb.Center, 8);

		// detach the octree from the scene manager.
		mDefaultOctree->remove();
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

		return node;
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

	void CSceneManager::update(u32 delta)
	{
		mMillisecondsDelta = delta;
		mSecondsDelta = static_cast<f32>(delta)* 0.001f;

		ITextureManager::getInstance()->updateTemporaryTextures(delta);

		// update skydome position
		if (mSkyDomeNode)
		{
			ICameraNode* camera = getActiveCameraNode();
			if (camera)
			{
				XMFLOAT3 pos = camera->getPosition();
				mSkyDomeNode->setPosition(pos.x, pos.y, pos.z);
			}
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

	void CSceneManager::drawShadowMaps()
	{
		ICameraNode* activeCamera = getActiveCameraNode();
		const math::SFrustum& frustum = activeCamera->getFrustum();
		setActiveCamera(mShadowMapCamera);
		SViewport preViewport = mVideoDriver->getViewport();

		//IRenderTarget* preRenderTarget = mVideoDriver->getRenderTarget();
		//IDepthStencilSurface* preDepthStencilSurface = mVideoDriver->getDepthStencilSurface();
		
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
		mVideoDriver->setDefaultRenderTargetAndDepthStencil();
		setActiveCamera(activeCamera);
	}

	/* called by light objects */
	void CSceneManager::drawShadowMap(ILightNode* light)
	{
		mCurrentShadowLightID = light->getId();
		draw(mDefaultOctree);
	}

	void CSceneManager::drawAll()
	{
		drawShadowMaps();

		if (mCompositors.empty())
		{
			if (mVideoDriver->isDeferredShading())
			{
				mVideoDriver->setPipelineUsage(EPU_DEFERRED_SHADING);
				// set gbuffer as render targets.
				
				static const u32 gBufferCount = IVideoDriver::MAX_GBUFFER_COUNT;
				static IRenderTarget* gbuffers[gBufferCount];
				
				mVideoDriver->getGBuffers(gbuffers);

				for (u32 i = 0; i < gBufferCount; i++)
					gbuffers[i]->clear();

				mVideoDriver->setMultipleRenderTargets(gbuffers, gBufferCount);
				
				draw(mDefaultOctree);
				mVideoDriver->setPipelineUsage(EPU_FORWARD);

				IRenderTarget* pDefaultTarget = mVideoDriver->getDefaultRenderTarget();
				IRenderTarget* targets[] = { pDefaultTarget };

				IDepthStencilSurface* depthStencilSurface = mVideoDriver->getDepthStencilSurface();
				mVideoDriver->setMultipleRenderTargets(targets, 1, nullptr);

				// set gBuffers and previous depth buffer as textures input.
				IPipeline* deferredPipeline = getDeferredShadingPipeline();
				SMaterial material(deferredPipeline);
				
				material.setTexture(0, gbuffers[0]->getTexture());
				material.setTexture(1, gbuffers[1]->getTexture());
				material.setTexture(2, gbuffers[2]->getTexture());
				material.setTexture(3, gbuffers[3]->getTexture());
				material.setTexture(4, depthStencilSurface->getTexture());

				IMeshNode* quad = getQuadNode();
				quad->setMaterial(&material);

				mDeferredShadingLights.clear();
				ICameraNode* camera = getActiveCameraNode();
				if (camera)
				{
					math::SFrustum frustum = camera->getFrustum();
					mDefaultOctree->getLightsInFrustum(frustum, mDeferredShadingLights.PointLights);
				}

				mRenderingDeferredQuad = true;

				draw(quad);

				mRenderingDeferredQuad = false;
			}
			else
			{
				draw(mDefaultOctree);
			}
		}
		else
		{
			// if multisampling, then create a new depth buffer
			IRenderTarget* target = ITextureManager::getInstance()->getTempRenderTarget(0, 0, EGF_R8G8B8A8_UNORM);
			target->clear();
			mVideoDriver->setRenderTarget(target);
			draw(mDefaultOctree);
			
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
			f32 width = static_cast<f32>(mDevice->getClientWidth());
			f32 height = static_cast<f32>(mDevice->getClientHeight());
			aspectRatio = width / height;
		}

		IFpsCameraNode* camera = new CFpsCameraNode(parent, this, position, lookat, up, aspectRatio,
			fov, nearZ, farZ, maxUpAngle, maxDownAngle, bPersectiveProj);

		mCameraNodes[id] = camera;
		if (mActiveCameraId == EMPTY_CAMERA_ID)
			setActiveCamera(id);

		return camera;
	}

	ICameraNode* CSceneManager::getActiveCameraNode()
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


}

