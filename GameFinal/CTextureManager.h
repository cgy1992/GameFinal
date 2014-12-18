#ifndef __TEXTURE_MANAGER_CLASS_H__
#define __TEXTURE_MANAGER_CLASS_H__

#include "IDevice.h"
#include "ITextureManager.h"
#include "IResourceFactory.h"
#include "CSortCodeAllocator.h"

#define _RENDER_STATE_IDLE_TIME_ 2000 


namespace gf
{
	class CTextureManager : public ITextureManager
	{
	public:
		CTextureManager(IDevice* device, IResourceFactory* pResourceFactory);

		virtual ~CTextureManager();

		virtual ITexture* load(const std::string& name);

		virtual ITextureCube* loadCubeTexture(const std::string& name);

		virtual ITexture* get(const std::string& name, bool bLoadIfNotExist = true);

		virtual ITextureCube* getTextureCube(const std::string& name, bool bLoadIfNotExist = true);

		virtual IRenderTarget* getRenderTarget(const std::string& name) const;

		virtual IDepthStencilSurface* getDepthStencilSurface(const std::string& name) const;

		virtual ITexture* createTexture1D(
			const std::string& name,
			u32 width,
			u32 bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format);

		virtual ITexture* createTexture2D(
			const std::string& name,
			u32 width,
			u32 height,
			u32 bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0);

		virtual ITexture2DArray* createTexture2DArray(
			const std::string& name,
			u32 width,
			u32 height,
			u32 arraySize,
			u32 bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0);

		virtual ITexture3D* createTexture3D(
			const std::string& name,
			u32 width,
			u32 height,
			u32 depth,
			u32 bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0,
			u32 slicePitch = 0);

		virtual ITextureCube* createTextureCube(
			const std::string& name,
			u32 size,
			u32 bindFlags,
			void* data,
			u32 miplevel,
			E_GI_FORMAT format,
			u32 pitch = 0);

		virtual IRenderTarget* createRenderTarget(
			const std::string& name,
			u32  width = 0,
			u32 height = 0,
			E_GI_FORMAT format = EGF_R32G32B32A32_FLOAT);

		virtual IDepthStencilSurface* createDepthStencilSurface(
			const std::string& name,
			u32 width = 0, 
			u32 height = 0,
			u32 depthBitNum = 24, 
			u32 stencilBitNum = 8,
			bool multiSampling = false,
			u32 multiSamplingCount = 1,
			u32 multiSamplingQuality = 0,
			bool bShaderBound = true, 
			bool bindDepthToShader = true);

		virtual IRenderTarget* getTempRenderTarget(
			u32 width = 0,
			u32 height = 0,
			E_GI_FORMAT = EGF_R8G8B8A8_UNORM);

		virtual bool releaseTempRenderTarget(IRenderTarget* pRenderTarget);

		virtual IDepthStencilSurface* getTempDepthStencilSurface(
			u32 width = 0,
			u32 height = 0,
			u32 depthBits = 32,
			u32 stencilBits = 0);

		virtual bool releaseTempDepthStencilSurface(IDepthStencilSurface* pDepthStencilSurface);

		virtual void updateTemporaryTextures(u32 delta);

		_DEFINE_RESOURCE_DESTROY_METHODS(CTextureManager, mTextureMap, ITexture);
		
	private:

		void createStockTextures();

		void createShadowMapJitterTexture();
		void createPointLightShadowMapJitterTexture();


		IDevice*								mDevice;
		IResourceFactory*						mResourceFactory;
		std::map<std::string, ITexture*>		mTextureMap;
		CSortCodeAllocator<255>					mCodeAllocator;

		struct STemporaryTextureInfo
		{
			u32				IdledTime;
			ITexture*		Texture;
		};

		std::list<STemporaryTextureInfo>				mIdledRenderTargets;
		std::list<STemporaryTextureInfo>				mIdledDepthStencilTextures;

	};
}

#endif
