#ifndef __CSCENEMANAGER_H__
#define __CSCENEMANAGER_H__

#include "ISceneManager.h"
namespace gf
{

	class CSceneManager : public ISceneManager
	{
		friend class CDirectionalLightNode;
		friend class CPointLightNode;

	public:

		struct SLightsInDeferredShading
		{
			std::vector<ILightNode*>	PointLights;
			std::vector<ILightNode*>	SpotLights;
			void clear()
			{
				PointLights.clear();
				SpotLights.clear();
			}
		};

	public:
		CSceneManager(IDevice* device, const math::SAxisAlignedBox& aabb);

		virtual ~CSceneManager();

		virtual void init();

		virtual void drawShadowMaps();

		virtual void drawShadowMap(ILightNode* light);

		virtual void drawAll();

		virtual void draw(ISceneNode* node);

		virtual void render(E_PIPELINE_USAGE);

		

		virtual ISceneNode* addEmptyNode(
			ISceneNode* parent = nullptr,
			bool bStatic = false,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f));

		virtual IMeshNode* addMeshNode(
			ISimpleMesh* mesh,
			IMaterial* material,
			ISceneNode* parent = nullptr,
			bool bStatic = false,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f));

		virtual IMeshNode* addModelMeshNode(
			IModelMesh* mesh,
			ISceneNode* parent = nullptr,
			bool bStatic = false,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f));

		virtual IAnimatedMeshNode* addAnimatedMeshNode(
			IAnimatedMesh* mesh,
			ISceneNode* parent = nullptr,
			bool bStatic = false,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f));

		virtual IInstanceCollectionNode* addInstanceCollectionNode(
			IMesh* mesh,
			ISceneNode* parent,
			u32 maxInstanceNum,
			u32 eachInstanceDataSize,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f));

		virtual void registerNodeForRendering(IMeshNode* node, E_NODE_TYPE nodeType = ENT_SOLID_NODE);

		virtual void registerNodeForRendering(ILightNode* node);

		virtual ILightNode* addDirectionalLight(u32 id,
			ISceneNode* parent,
			const XMFLOAT3& direction);

		virtual ILightNode* addPointLight(u32 id,
			ISceneNode* parent,
			bool bStatic,
			const XMFLOAT3& position,
			f32 range);

		virtual ILightNode* addSpotLight(u32 id,
			ISceneNode* parent,
			bool bStatic,
			const XMFLOAT3& position,
			const XMFLOAT3& direction,
			f32 range,
			f32 innerCone,
			f32 outerCone);

		virtual ICameraNode* addCameraNode(u32 id,
			ISceneNode* parent = nullptr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& lookat = XMFLOAT3(0, 0, 5.0f),
			const XMFLOAT3& up = XMFLOAT3(0, 1.0f, 0),
			bool bPersectiveProj = true,
			f32 fov = XM_PIDIV4,
			f32 nearZ = 1.0f,
			f32 farZ = 1000.0f,
			f32 aspectRatio = -1.0f);

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
			f32 aspectRatio = -1.0f);

		virtual IOctreeManager* addOctreeManagerNode(ISceneNode* parent,
			f32 width,
			f32 height,
			f32 depth,
			XMFLOAT3 center = XMFLOAT3(0, 0, 0),
			u32 maxTreeHeight = 8);

		virtual ITerrainNode* addTerrainNode(
			ITerrainMesh* mesh,
			IMaterial* material = nullptr,
			ISceneNode* parent = nullptr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0));

		virtual IOctreeManager*	getDefaultOctreeManager() { return mDefaultOctree; }

		virtual void setSkyDome(ITextureCube* cubeTexture);

		virtual ILightNode* getLightNode(u32 id);

		virtual ICameraNode* getActiveCameraNode();

		virtual u32 getActiveCameraId() const;

		virtual ICameraNode* getCameraNode(u32 id);

		virtual ICameraNode* setActiveCamera(u32 id);

		virtual bool setActiveCamera(ICameraNode* camera);

		virtual u32 getRenderedMeshNum() const
		{
			return mRenderedMeshNum;
		}

		virtual void update(f32 delta = 0);

		bool isCulled(const IMeshNode* node);

		virtual IMeshNode* getQuadNode();

		virtual ICompositor* createCompositor(IPipeline* pipeline);

		virtual ICompositor* createCompositor(u32, const SCompositorCreateParam& param);

		virtual void addCompositor(ICompositor* compositor);

		virtual void setAmbient(const XMFLOAT4& color) { mAmbient = color; }

		virtual XMFLOAT4 getAmbient() { return mAmbient; }

		virtual bool getNearLights(IMeshNode* node,
			E_LIGHT_TYPE lightType, std::vector<ILightNode*>& lights);

		virtual const std::list<ILightNode*>& getDirectionalLights() const { return mDirectionalLights; }

		virtual void setDeferredShadingPipeline(IPipeline* pipeline);

		virtual void setDeferredShadingPipeline(const std::string& name);

		virtual IPipeline* getDeferredShadingPipeline();

	private:

		void collectMeshNodeShaders(IMeshNode* node);

		void registerToOctree(ISceneNode* scene);

		ISceneNode* getParentNode(ISceneNode* parent, bool bStatic);

		void drawDeferredShading(IRenderTarget* target);

		std::vector<ISceneNode*>		mSolidNodes;
		std::map<u32, ILightNode*>		mLightNodes;
		std::array<ICameraNode*, 65>	mCameraNodes;
		std::set<IShader*>				mActiveShaders;

		ICameraNode*					mActiveCamera;
		u32								mActiveCameraId;
		ICameraNode*					mShadowMapCamera;

		u32								mRenderedMeshNum;

		f32								mSecondsDelta;
		//u32								mMillisecondsDelta;

		IMeshNode*						mSkyDomeNode;
		IMeshNode*						mQuadMeshNode;
		IOctreeManager*					mDefaultOctree;

		std::vector<ICompositor*>		mCompositors;

		XMFLOAT4						mAmbient;

		std::list<ILightNode*>			mDirectionalLights;

		u32								mCurrentShadowLightID;
	//	E_PIPELINE_USAGE				mCurrentPipelineUsage;

		IPipeline*						mDeferredShadingPipeline;

		bool							mRenderingDeferredQuad;

		SLightsInDeferredShading		mDeferredShadingLights;
		
	};
}

#endif