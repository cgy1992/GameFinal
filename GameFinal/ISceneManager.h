#ifndef __ISCENEMANAGER_H__
#define __ISCENEMANAGER_H__

#include "gfTypes.h"
#include "gfUtil.h"
#include "gfRect.h"
#include "IReferenceCounted.h"
#include "IDevice.h"
#include "IModelMeshNode.h"
#include "IVideoDriver.h"
#include "IGeometryCreator.h"
#include "IMaterial.h"
#include "IResourceGroupManager.h"
#include "IFpsCameraNode.h"
#include "IAnimatedMeshNode.h"
#include "IOctreeManager.h"
#include "ITerrainNode.h"
#include "ICompositor.h"
#include "ILightNode.h"
#include "IInstanceNode.h"
#include "IInstanceCollectionNode.h"
#include "IReflectionPlane.h"

namespace gf
{
	class ISceneManager : public ISceneNode
	{
	public:
		ISceneManager(IDevice* device, const math::SAxisAlignedBox& aabb)
			:ISceneNode(nullptr, this, false)
			, mVideoDriver(nullptr)
			, mMaterialManager(nullptr)
			, mDevice(device)
			, mAabb(aabb)
		{

		}

		virtual void init() = 0;

		virtual void drawAll() = 0;

		virtual void drawPass() = 0;

		virtual void draw(u32 tag) = 0;

		virtual void draw(ISceneNode* node) = 0;

		virtual void drawShadowMap(ILightNode* light) = 0;

		virtual void drawReflectionMaps() = 0;

		IDevice* getDevice()
		{
			return mDevice;
		}


		virtual void registerNodeForRendering(IMeshNode* node, E_NODE_TYPE nodeType = ENT_SOLID_NODE) = 0;

		virtual void registerNodeForRendering(ILightNode* node) = 0;

		virtual ISceneNode* addEmptyNode(
			ISceneNode* parent = nullptr,
			bool bStatic = false,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f)) = 0;

		virtual IMeshNode* addMeshNode(
			ISimpleMesh* mesh,
			IMaterial* material,
			ISceneNode* parent = nullptr,
			bool bStatic = false,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f)) = 0;

		virtual IMeshNode* addMeshNode(
			ISimpleMesh* mesh,
			const std::string& materialName,
			ISceneNode* parent = nullptr,
			bool bStatic = false,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f)) = 0;

		virtual IOctreeManager* addOctreeManagerNode(ISceneNode* parent,
			f32 width,
			f32 height,
			f32 depth,
			XMFLOAT3 center = XMFLOAT3(0, 0, 0),
			u32 maxTreeHeight = 8) = 0;

		virtual IMeshNode* addModelMeshNode(
			IModelMesh* mesh,
			ISceneNode* parent = nullptr,
			bool bStatic = false,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f)) = 0;

		virtual IAnimatedMeshNode* addAnimatedMeshNode(
			IAnimatedMesh* mesh,
			ISceneNode* parent = nullptr,
			bool bStatic = false,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f)) = 0;

		virtual IInstanceCollectionNode* addInstanceCollectionNode(
			IMesh* mesh,
			ISceneNode* parent,
			u32 maxInstanceNum,
			u32 eachInstanceDataSize,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f)) = 0;


		virtual ITerrainNode* addTerrainNode(
			ITerrainMesh* mesh,
			IMaterial* material = nullptr,
			ISceneNode* parent = nullptr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0)) = 0;


		virtual ITerrainNode* addTerrainNode(
			ITerrainMesh* mesh,
			const std::string& materialName,
			ISceneNode* parent = nullptr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0)) = 0;
		

		virtual ILightNode* addDirectionalLight(u32 id,
			ISceneNode* parent,
			const XMFLOAT3& direction) = 0;

		virtual ILightNode* addPointLight(u32 id,
			ISceneNode* parent,
			bool bStatic,
			const XMFLOAT3& position,
			f32 range) = 0;

		virtual ILightNode* addSpotLight(u32 id,
			ISceneNode* parent,
			bool bStatic,
			const XMFLOAT3& position,
			const XMFLOAT3& direction,
			f32 range,
			f32 innerCone,
			f32 outerCone) = 0;

		virtual ICameraNode* addCameraNode(u32 id,
			ISceneNode* parent = nullptr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, -5.0f),
			const XMFLOAT3& lookat = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& up = XMFLOAT3(0, 1.0f, 0),
			bool bPersectiveProj = true,
			f32 fov = XM_PIDIV4,
			f32 nearZ = 1.0f,
			f32 farZ = 1000.0f,
			f32 aspectRatio = -1.0f) = 0;

		virtual IFpsCameraNode* addFpsCameraNode(u32 id,
			ISceneNode* parent = nullptr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, -5.0f),
			const XMFLOAT3& lookat = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& up = XMFLOAT3(0, 1.0f, 0),
			bool bPersectiveProj = true,
			f32 maxUpAngle = XM_PI * 0.33f,
			f32 maxDownAngle = XM_PI * 0.33f,
			f32 fov = XM_PIDIV4,
			f32 nearZ = 1.0f,
			f32 farZ = 1000.0f,
			f32 aspectRatio = -1.0f) = 0;

		virtual IReflectionPlane* addReflectionPlane(u32 id,
			XMFLOAT4 plane, f32 planeSizeX, f32 planeSizeZ, 
			u32 mapWidth = 0, u32 mapHeight = 0) = 0;

		virtual IReflectionPlane* getReflectionPlane(u32 id) = 0;

		virtual void setSkyDome(ITextureCube* cubeTexture) = 0;

		virtual void setSkyDome(const std::string& textureName) = 0;

		virtual ILightNode* getLightNode(u32 id) = 0;

		IVideoDriver* getVideoDriver()
		{
			return mVideoDriver;
		}

		

		virtual IOctreeManager*	getDefaultOctreeManager() = 0;

		virtual ICameraNode* getActiveCameraNode() const = 0;

		virtual u32 getActiveCameraId() const = 0;

		virtual ICameraNode* getCameraNode(u32 id) = 0;

		virtual ICameraNode* setActiveCamera(u32 id) = 0;

		virtual bool setActiveCamera(ICameraNode* camera) = 0;

		virtual u32 getRenderedMeshNum() const = 0;

		virtual IMeshNode* getQuadNode() = 0;

		virtual IMeshNode* getSkyNode() = 0;

		virtual ICompositor* createCompositor(IPipeline* pipeline) = 0;

		virtual ICompositor* createCompositor(u32, const SCompositorCreateParam& param) = 0;

		virtual void addCompositor(ICompositor* compositor) = 0;

		virtual void setAmbient(const XMFLOAT4& color) = 0;

		virtual XMFLOAT4 getAmbient() = 0;

		virtual bool getNearLights(IMeshNode* node, 
			E_LIGHT_TYPE lightType, 
			std::vector<ILightNode*>& lights) = 0;

		virtual const std::list<ILightNode*>& getDirectionalLights() const = 0;

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_SCENE_MANAGER;
		}

		virtual math::SAxisAlignedBox getAabb() const
		{
			return mAabb;
		}
		
		virtual void setDeferredShadingPipeline(IPipeline* pipeline) = 0;

		virtual void setDeferredShadingPipeline(const std::string& name) = 0;

		virtual IPipeline* getDeferredShadingPipeline() = 0;

		virtual IShader* getTileBasedDeferredShadingCS() = 0;

		virtual bool setTileBasedDeferredShadingCS(IShader* shader) = 0;

		virtual math::SRay getPickingRay(u32 sx, u32 sy) const = 0;

		virtual ISceneNode* intersectRay(const math::SRay& ray, f32* pDist,
			u32 nodeType = ENT_SOLID_NODE | ENT_LIGHT_NODE) const = 0;

		virtual ISceneNode* intersectRayWithTag(const math::SRay& ray, f32* pDist, u32 tag,
			u32 nodeType = ENT_SOLID_NODE | ENT_LIGHT_NODE) const = 0;

		virtual void draw2DImage(ITexture* texture,
			const math::Rect<s32>& sourceRect,
			const math::Rect<s32>& clipRect,
			XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			bool 	useAlphaChannelOfTexture = false) = 0;

		virtual void draw2DImage(ITexture* texture,
			const math::Rect<f32>& sourceRect,
			const math::Rect<f32>& clipRect,
			XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			bool 	useAlphaChannelOfTexture = false) = 0;

		virtual bool getHeightOnTerrain(f32 x, f32 z, f32& height) const = 0;

	public:
		const static int MAX_LIGHT_COUNT = 1024;
		const static int MAX_CAMERA_COUNT = 64;
		const static int EMPTY_CAMERA_ID = 64;
		const static int SHADOW_CAMERA_ID = 63;
		const static int REFLECT_CAMERA_ID = 62;

	protected:
		IDevice*				mDevice;
		IVideoDriver*			mVideoDriver;
		IMaterialManager*		mMaterialManager;
		math::SAxisAlignedBox	mAabb;


	};

}

#endif
