#ifndef IVIDEODRIVER_H
#define IVIDEODRIVER_H

#include "gfTypes.h"
#include "gfUtil.h"
#include "IShaderManager.h"
#include "IInputLayoutManager.h"
#include "IPipelineManager.h"
#include "ITextureManager.h"
#include "IResourceFactory.h"
#include "IMesh.h"
#include "IMaterialManager.h"
#include "IRenderStateManager.h"
#include "IMeshManager.h"
#include "IGeometryCreator.h"
#include "IResourceGroupManager.h"
#include "IShaderVariableInjection.h"
#include "ISamplerManager.h"
#include "IDepthStencilSurface.h"
#include "ICompositorFactory.h"

namespace gf
{
	enum E_GBUFFER_TYPE
	{
		EGT_GBUFFER_0,
		EGT_GBUFFER_1,
		EGT_GBUFFER_2,
		EGT_GBUFFER_3,
		EGT_GBUFFER_COUNT,
	};

	enum E_DEFERRED_SHADING_ALGORITHM
	{
		EDSA_NORMAL_DEFERRED_SHADING,
		EDSA_TILED_BASED_DEFERRED_SHADING,
		EDSA_CS_TILE_BASED_DEFERRED_SHADING,
	};

	struct SViewport
	{
		f32 TopLeftX;
		f32 TopLeftY;
		f32 Width;
		f32 Height;
		f32 MinDepth;
		f32 MaxDepth;

		SViewport()
			: TopLeftX(0), TopLeftY(0)
			, Width(0), Height(0)
			, MinDepth(0), MaxDepth(0)
		{

		}

		bool operator==(const SViewport& v) const
		{
			if (TopLeftX == v.TopLeftX && TopLeftY == v.TopLeftY
				&& Width == v.Width && Height == v.Height
				&& MinDepth == v.MinDepth && MaxDepth == v.MaxDepth)
			{
				return true;
			}
			return false;
		}
	};

	class IVideoDriver : public IReferenceCounted
	{
	public:
		IVideoDriver()
			:mResourceFactory(nullptr)
			, mShaderManager(nullptr)
			, mInputLayoutManager(nullptr)
			, mPipeManager(nullptr)
			, mTextureManager(nullptr)
			, mRenderStateManager(nullptr)
			, mMeshManager(nullptr)
			, mDeferredShading(false)
			, mDeferredAntiAliasing(false)
			, mDeferredShadingAlgorithm(EDSA_CS_TILE_BASED_DEFERRED_SHADING)
			, mBackBufferWidth(0)
			, mBackBufferHeight(0)
		{
			ZeroMemory(mGBuffers, sizeof(mGBuffers));
		}

		virtual ~IVideoDriver()
		{

		}

		virtual bool init(SCreationParameters& createParam) = 0;

		virtual void beginScene(
			bool backBuffer,
			bool zBuffer,
			const f32 color[],
			f32 depthValue = 1.0f,
			u8 stencilValue = 0) = 0;

		virtual void endScene() = 0;

		const char* getVideoCardDescription() const
		{
			return mVideoCardDescription;
		}

		virtual void bindPrimitiveType(E_PRIMITIVE_TYPE primitiveType) = 0;

		virtual void clearShader(E_SHADER_TYPE shaderType) = 0;

		virtual void clearDepthStencil(f32 depth, u8 stencil) = 0;

		virtual void clearDepth(f32 val) = 0;

		virtual void clearStencil(u8 val) = 0;

		virtual void clearRenderTarget(const f32 color[]) = 0;

		virtual void setRenderTarget(IRenderTarget* pRenderTarget) = 0;

		//virtual void setRenderTargets(IRenderTarget* pRenderTargets[], u32 count) = 0;

		virtual void setMultipleRenderTargets(IRenderTarget* renderTargets[], u32 count) = 0;

		virtual void setMultipleRenderTargets(IRenderTarget* renderTargets[], u32 count, IDepthStencilSurface* pDepthStencilSurface) = 0;

		virtual IRenderTarget* getRenderTarget(u32 index = 0) = 0;

		virtual void setDefaultRenderTarget() = 0;
		
		virtual void setDepthStencilSurface(IDepthStencilSurface* depthStencilSurface) = 0;

		virtual IDepthStencilSurface* getDepthStencilSurface() = 0;

		virtual void setDefaultDepthStencilSurface() = 0;

		virtual void setRenderTargetAndDepthStencil(IRenderTarget* pRenderTarget, IDepthStencilSurface* pDepthStencilSurface) = 0;

		virtual void setDefaultRenderTargetAndDepthStencil() = 0;

		virtual void setViewport(f32 topLeftX, f32 topLeftY, f32 width, f32 height, f32 minDepth = 0.0f, f32 maxDepth = 1.0f) = 0;

		virtual void setViewport(const SViewport& viewport) = 0;

		virtual void setViewports(const std::vector<SViewport>& viewports) = 0;

		virtual bool runComputeShader(IShader* shader, u32 x, u32 y, u32 z, 
			ISceneNode* node = nullptr) = 0;

		virtual void resetTextures(E_SHADER_TYPE shaderType = EST_UNDEFINED_SHADER) = 0;

		virtual void resetRWTextures() = 0;

		const SViewport& getViewport() const { return mViewport; }

		IResourceFactory*		getResourceFactory()
		{
			return mResourceFactory;
		}

		IGeometryCreator* getGeometryCreator()
		{
			return mGeometryCreator;
		}

		IMaterialManager* getMaterialManager()
		{
			return mMaterialManager;
		}

		ITextureManager*		getTextureManager()
		{
			return mTextureManager;
		}

		IRenderStateManager*	getRenderStateManager()
		{
			return mRenderStateManager;
		}

		IMeshManager*			getMeshManager()
		{
			return mMeshManager;
		}

		IShaderManager*			getShaderManager()
		{
			return mShaderManager;
		}

		IInputLayoutManager*	getInputLayoutManager()
		{
			return mInputLayoutManager;
		}

		IPipelineManager*		getPipelineManager()
		{
			return mPipeManager;
		}

		IResourceGroupManager*	getResourceGroupManager()
		{
			return mResourceGroupManager;
		}

		ISamplerManager*		getSamplerManager()
		{
			return mSamplerManager;
		}

		virtual ICompositorFactory*		getCompositorFactory() = 0;

		bool isRenderingShadowMap() const 
		{ 
			return mCurrentPipelineUsage == EPU_DIR_SHADOW_MAP || mCurrentPipelineUsage == EPU_POINT_SHADOW_MAP;
		}

		E_PIPELINE_USAGE getPipelineUsage() const { return mCurrentPipelineUsage; }

		virtual void setPipelineUsage(E_PIPELINE_USAGE usage) = 0;

		bool isDeferredShading() const { return mDeferredShading; }
		void setDeferredShading(bool deferredShading) { mDeferredShading = deferredShading; }
		bool isDeferredAntiAliasing() { return mDeferredAntiAliasing; }
		void setDeferredAntiAliasing(bool aa) { mDeferredAntiAliasing = aa; }

		E_DEFERRED_SHADING_ALGORITHM getDeferredShadingAlgorithm() { return mDeferredShadingAlgorithm; }
		void setDeferredShadingAlgorithm(E_DEFERRED_SHADING_ALGORITHM method) { mDeferredShadingAlgorithm = method; }

		virtual void setGBuffersAsRenderTargets() = 0;

		virtual void getGBuffers(IRenderTarget* renderTargets[]) = 0;

		virtual IRenderTarget* getDefaultRenderTarget() = 0;

		u32 getBackBufferWidth() const { return mBackBufferWidth; }

		u32 getBackBufferHeight() const { return  mBackBufferHeight; }

		_DECLARE_SINGLETON_INSTANCE(IVideoDriver);

	public:

		static const u32 MAX_GBUFFER_COUNT = 4;

	protected:
		int								mVideoCardMemory;
		char							mVideoCardDescription[128];
		IResourceFactory*				mResourceFactory;
		IGeometryCreator*				mGeometryCreator;
		IMaterialManager*				mMaterialManager;
		IShaderManager*					mShaderManager;
		IInputLayoutManager*			mInputLayoutManager;
		IPipelineManager*				mPipeManager;
		ITextureManager*				mTextureManager;
		IRenderStateManager*			mRenderStateManager;
		IMeshManager*					mMeshManager;
		IResourceGroupManager*			mResourceGroupManager;
		ISamplerManager*				mSamplerManager;
		SViewport						mViewport;
		u32								mBackBufferWidth;
		u32								mBackBufferHeight;

		//bool							mRenderingShadowMap;
		E_PIPELINE_USAGE				mCurrentPipelineUsage;
		bool							mDeferredShading;
		bool							mDeferredAntiAliasing;
		E_DEFERRED_SHADING_ALGORITHM	mDeferredShadingAlgorithm;
		f32								mDeferredShadingTileSize;
		IRenderTarget*					mGBuffers[EGT_GBUFFER_COUNT];
		//	IShaderVariableInjection*		mShaderVariableInjector;

		
	};

}

#endif