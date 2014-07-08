#ifndef __TEXTURE_MANAGER_CLASS_H__
#define __TEXTURE_MANAGER_CLASS_H__

#include "IDevice.h"
#include "ITextureManager.h"
#include "IResourceFactory.h"
#include "CSortCodeAllocator.h"
namespace gf
{
	class CTextureManager : public ITextureManager
	{
	public:
		CTextureManager(IDevice* device, IResourceFactory* pResourceFactory);

		virtual ITexture* load(const std::string& name, const std::string& filepath);
		virtual ITexture* get(const std::string& name) const;

		virtual IRenderTarget* getRenderTarget(const std::string& name) const;

		virtual IDepthStencilSurface* getDepthStencilSurface(const std::string& name) const;

		virtual ITexture* create(
			const std::string& name,
			u32 width,
			u32 height,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0);

		virtual IRenderTarget* createRenderTarget(
			const std::string& name,
			bool multiSampling = true,
			u32  width = 0,
			u32 height = 0,
			E_GI_FORMAT format = EGF_R32G32B32A32_FLOAT,
			u32 multiSamplingCount = 0,
			u32 multiSamplingQuality = 0);

		virtual IDepthStencilSurface* createDepthStencilSurface(
			const std::string& name,
			u32 width = 0, 
			u32 height = 0,
			u32 depthBitNum = 24, 
			u32 stencilBitNum = 8,
			bool multiSampling = false, 
			u32 multiSamplingCount = 0, 
			u32 multiSamplingQuality = 0,
			bool bShaderBound = false, 
			bool bindDepthToShader = true);


		_DEFINE_RESOURCE_DESTROY_METHODS(CTextureManager, mTextureMap, ITexture);


	private:

		IDevice*								mDevice;
		IResourceFactory*						mResourceFactory;
		std::map<std::string, ITexture*>		mTextureMap;
		CSortCodeAllocator<255>					mCodeAllocator;
		

	};
}

#endif
