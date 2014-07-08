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

namespace gf
{
	struct SViewport
	{
		f32 TopLeftX;
		f32 TopLeftY;
		f32 Width;
		f32 Height;
		f32 MinDepth;
		f32 MaxDepth;
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
		{

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

		virtual void clearDepthStencil(f32 depth, u8 stencil) = 0;

		virtual void clearDepth(f32 val) = 0;

		virtual void clearStencil(u8 val) = 0;

		virtual void clearRenderTarget(const f32 color[]) = 0;

		virtual void setRenderTarget(IRenderTarget* pRenderTarget) = 0;

		virtual IRenderTarget* getRenderTarget() = 0;

		virtual void setDefaultRenderTarget() = 0;
		
		virtual void setDepthStencilSurface(IDepthStencilSurface* depthStencilSurface) = 0;

		virtual IDepthStencilSurface* getDepthStencilSurface() = 0;

		virtual void setDefaultDepthStencilSurface() = 0;

		virtual void setRenderTargetAndDepthStencil(IRenderTarget* pRenderTarget, IDepthStencilSurface* pDepthStencilSurface) = 0;

		virtual void setDefaultRenderTargetAndDepthStencil() = 0;

		virtual void setViewport(f32 topLeftX, f32 topLeftY, f32 width, f32 height, f32 minDepth = 0.0f, f32 maxDepth = 1.0f) = 0;

		virtual void setViewport(const SViewport& viewport) = 0;

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
		//	IShaderVariableInjection*		mShaderVariableInjector;
	};
}

#endif