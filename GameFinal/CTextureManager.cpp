#include "stdafx.h"
#include "gfUtil.h"
#include "CTextureManager.h"

namespace gf
{
	CTextureManager::CTextureManager(IDevice* device, IResourceFactory* pResourceFactory)
		:mDevice(device)
		, mResourceFactory(pResourceFactory)
	{

	}

	ITexture* CTextureManager::load(const std::string& name, const std::string& filepath)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			GF_PRINT_CONSOLE_INFO("Texture named %s has already been loaded.\n", name.c_str());
			return it->second;
		}

		u32 sortcode = mCodeAllocator.allocate();
		ITexture* pTexture = mResourceFactory->loadTextureFromFile(name, filepath, sortcode);
		if (pTexture == nullptr)
		{
			mCodeAllocator.release(sortcode);
			return nullptr;
		}
			

		mTextureMap.insert(std::make_pair(name, pTexture));
		return pTexture;
	}

	ITexture* CTextureManager::create(
		const std::string& name,
		u32 width,
		u32 height,
		void* data,
		u32 mipLevel,
		E_GI_FORMAT format,
		u32 pitch)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			GF_PRINT_CONSOLE_INFO("Texture named '%s' has already existed.\n", name.c_str());
			return it->second;
		}

		u32 sortcode = mCodeAllocator.allocate();
		ITexture* pTexture = mResourceFactory->createTexture(name, sortcode, width, height, data, mipLevel, format, pitch);
		if (pTexture == nullptr)
		{
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pTexture));
		return pTexture;
	}

	IRenderTarget* CTextureManager::createRenderTarget(
		const std::string& name,
		bool multiSampling,
		u32  width,
		u32 height,
		E_GI_FORMAT format,
		u32 multiSamplingCount,
		u32 multiSamplingQuality)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			if (it->second->getType() == ETT_RENDER_TARGET)
			{
				GF_PRINT_CONSOLE_INFO("RenderTarget named '%s' has already existed.\n", name.c_str());
				return dynamic_cast<IRenderTarget*>(it->second);
			}
			else
			{
				GF_PRINT_CONSOLE_INFO("Texture named '%s' has already existed and it is not a kind of RenderTarget.\n", name.c_str());
				return nullptr;
			}
		}

		if (width == 0)
			width = mDevice->getClientWidth();
		if (height == 0)
			height = mDevice->getClientHeight();
		if (!multiSampling)
		{
			multiSamplingCount = 1;
			multiSamplingQuality = 0;
		}
		else if (multiSamplingCount == 0)
		{
			const SCreationParameters& param = mDevice->getCreationParameters();
			multiSamplingCount = param.MultiSamplingCount;
			multiSamplingQuality = param.MultiSamplingQuality;
		}

		u32 sortcode = mCodeAllocator.allocate();
		IRenderTarget* pRenderTarget = mResourceFactory->createRenderTarget(name, sortcode, width, height, format, 
			multiSampling, multiSamplingCount, multiSamplingQuality);
		if (pRenderTarget == nullptr)
		{
			GF_PRINT_CONSOLE_INFO("RenderTarget named '%s' has failed to be created.\n", name.c_str());
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pRenderTarget));
		return pRenderTarget;
	}

	IDepthStencilSurface* CTextureManager::createDepthStencilSurface(
		const std::string& name,
		u32 width,
		u32 height,
		u32 depthBitNum,
		u32 stencilBitNum,
		bool multiSampling,
		u32 multiSamplingCount,
		u32 multiSamplingQuality,
		bool bShaderBound,
		bool bindDepthToShader)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			if (it->second->getType() == ETT_DEPTH_STENCIL)
			{
				GF_PRINT_CONSOLE_INFO("DepthStencilSurface named '%s' has already existed.\n", name.c_str());
				return dynamic_cast<IDepthStencilSurface*>(it->second);
			}
			else
			{
				GF_PRINT_CONSOLE_INFO("DepthStencilSurface named '%s' has already existed and it is not a kind of RenderTarget.\n", name.c_str());
				return nullptr;
			}
		}

		if (width == 0)
			width = mDevice->getClientWidth();
		if (height == 0)
			height = mDevice->getClientHeight();
		if (!multiSampling)
		{
			multiSamplingCount = 1;
			multiSamplingQuality = 0;
		}
		else if (multiSamplingCount == 0)
		{
			const SCreationParameters& param = mDevice->getCreationParameters();
			multiSamplingCount = param.MultiSamplingCount;
			multiSamplingQuality = param.MultiSamplingQuality;
		}

		u32 sortcode = mCodeAllocator.allocate();
		IDepthStencilSurface* pDepthStencilSurface = mResourceFactory->createDepthStencilSurface(name, sortcode, width, height,
			depthBitNum, stencilBitNum, multiSampling, multiSamplingCount, multiSamplingQuality, bShaderBound, bindDepthToShader);
		
		if (pDepthStencilSurface == nullptr)
		{
			GF_PRINT_CONSOLE_INFO("DepthStencilSurface named '%s' has failed to be created.\n", name.c_str());
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pDepthStencilSurface));
		return pDepthStencilSurface;
	}


	ITexture* CTextureManager::get(const std::string& name) const
	{
		auto it = mTextureMap.find(name);
		if (it == mTextureMap.end())
			return nullptr;

		return it->second;
	}

	IRenderTarget* CTextureManager::getRenderTarget(const std::string& name) const
	{
		auto it = mTextureMap.find(name);
		if (it == mTextureMap.end() || it->second->getType() != ETT_RENDER_TARGET)
		{
			return nullptr;
		}
		return dynamic_cast<IRenderTarget*>(it->second);
	}

	IDepthStencilSurface* CTextureManager::getDepthStencilSurface(const std::string& name) const
	{
		auto it = mTextureMap.find(name);
		if (it == mTextureMap.end() || it->second->getType() != ETT_DEPTH_STENCIL)
		{
			return nullptr;
		}
		return dynamic_cast<IDepthStencilSurface*>(it->second);
	}
}

