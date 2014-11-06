#include "stdafx.h"
#include "gfUtil.h"
#include "CTextureManager.h"
#include "IVideoDriver.h"

namespace gf
{
	CTextureManager::CTextureManager(IDevice* device, IResourceFactory* pResourceFactory)
		:mDevice(device)
		, mResourceFactory(pResourceFactory)
	{
		createStockTextures();
	}

	ITexture* CTextureManager::load(const std::string& name)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			GF_PRINT_CONSOLE_INFO("Texture named %s has already been loaded.\n", name.c_str());
			return it->second;
		}
		
		std::string filepath;
		if (!IResourceGroupManager::getInstance()->getFullPath(name, filepath, ERFT_TEXTURE))
		{
			GF_PRINT_CONSOLE_INFO("Texture file named '%s' has not been found.\n", name.c_str());
			return nullptr;
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

	ITextureCube* CTextureManager::loadCubeTexture(const std::string& name)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			ITexture* pTexture = it->second;
			if (pTexture->getType() == ETT_CUBE_TEXTURE)
			{
				GF_PRINT_CONSOLE_INFO("CubeTexture file named '%s' has already been loaded.\n", name.c_str());
				return dynamic_cast<ITextureCube*>(pTexture);
			}
			else
			{
				GF_PRINT_CONSOLE_INFO("Texture file named '%s' has already been loaded. But it is not a cube texture.\n", name.c_str());
				return nullptr;
			}
		}

		std::string filepath;
		if (!IResourceGroupManager::getInstance()->getFullPath(name, filepath, ERFT_TEXTURE))
		{
			GF_PRINT_CONSOLE_INFO("Texture file named '%s' has not been found.\n", name.c_str());
			return nullptr;
		}

		u32 sortcode = mCodeAllocator.allocate();
		ITextureCube* pTexture = mResourceFactory->loadCubeTextureFromFile(name, filepath, sortcode);
		if (pTexture == nullptr)
		{
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pTexture));
		return pTexture;
	}

	ITexture* CTextureManager::createTexture2D(
		const std::string& name,
		u32 width,
		u32 height,
		u32 bindFlags,
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
		ITexture* pTexture = mResourceFactory->createTexture2D(name, sortcode, width, height, bindFlags, data, mipLevel, format, pitch);
		if (pTexture == nullptr)
		{
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pTexture));
		return pTexture;
	}

	ITexture3D* CTextureManager::createTexture3D(
		const std::string& name,
		u32 width,
		u32 height,
		u32 depth,
		void* data,
		u32 mipLevel,
		E_GI_FORMAT format,
		u32 pitch,
		u32 slicePitch)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			if (it->second->getType() == ETT_TEXTURE_3D)
			{
				GF_PRINT_CONSOLE_INFO("Texture3D named '%s' has already existed.\n", name.c_str());
				return dynamic_cast<ITexture3D*>(it->second);
			}
			else
			{
				GF_PRINT_CONSOLE_INFO("Texture named '%s' has already existed and it is not a kind of Texture3D.\n", name.c_str());
				return nullptr;
			}
		}

		u32 sortcode = mCodeAllocator.allocate();
		ITexture3D* pTexture = mResourceFactory->createTexture3D(name, sortcode, width, height, depth, data, mipLevel, format, pitch, slicePitch);
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
		u32  width,
		u32 height,
		E_GI_FORMAT format)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			GF_PRINT_CONSOLE_INFO("Texture named '%s' has already existed.\n", name.c_str());
			return it->second->getRenderTarget();
		}

		if (width == 0)
			width = mDevice->getClientWidth();
		if (height == 0)
			height = mDevice->getClientHeight();

		u32 sortcode = mCodeAllocator.allocate();
		ITexture* pTexture = mResourceFactory->createTexture2D(name, sortcode, width, height, ETBT_SHADER | ETBT_RENDER_TARGET,
			nullptr, 1, format);

		if (!pTexture)
		{
			GF_PRINT_CONSOLE_INFO("RenderTarget named '%s' has failed to be created. \
								  Because the failure of creating texture.\n", name.c_str());
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pTexture));
		return pTexture->getRenderTarget();
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
			GF_PRINT_CONSOLE_INFO("Texture named '%s' has already existed.\n", name.c_str());
			return it->second->getDepthStencilSurface();
		}

		if (width == 0)
			width = mDevice->getClientWidth();
		if (height == 0)
			height = mDevice->getClientHeight();

		u32 sortcode = mCodeAllocator.allocate();
		ITexture* pTexture = mResourceFactory->createDepthStencilTexture(name, sortcode,
			width, height, depthBitNum, stencilBitNum, 
			multiSampling, multiSamplingCount, multiSamplingQuality,
			bShaderBound, bindDepthToShader);
		
		if (pTexture == nullptr)
		{
			GF_PRINT_CONSOLE_INFO("DepthStencilSurface named '%s' has failed to be created.\n", name.c_str());
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pTexture));
		return pTexture->getDepthStencilSurface();
	}


	ITexture* CTextureManager::get(const std::string& name, bool bLoadIfNotExist)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
			return it->second;

		if (bLoadIfNotExist)
			return load(name);

		return nullptr;
	}

	IRenderTarget* CTextureManager::getRenderTarget(const std::string& name) const
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			return it->second->getRenderTarget();
		}

		return nullptr;
	}

	IDepthStencilSurface* CTextureManager::getDepthStencilSurface(const std::string& name) const
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			return it->second->getDepthStencilSurface();
		}
		return nullptr;
	}

	IRenderTarget* CTextureManager::getTempRenderTarget(u32 width /*= 0*/, u32 height /*= 0*/, E_GI_FORMAT format/*= EGF_R32G32B32A32_FLOAT*/)
	{
		if (width == 0)
			width = (u32)IDevice::getInstance()->getClientWidth();
		if (height == 0)
			height = (u32)IDevice::getInstance()->getClientHeight();

		// find the longest idled time, the header of this list.
		if (!mIdledRenderTargets.empty())
		{
			for (auto it = mIdledRenderTargets.begin(); it != mIdledRenderTargets.end(); it++)
			{
				ITexture* p = it->Texture;
				if (p->getWidth() == width && p->getHeight() == height && p->getFormat() == format)
				{
					mIdledRenderTargets.erase(it);
					return p->getRenderTarget();
				}
			}
		}

		// if no idled render target, then create one.
		u32 sortcode = mCodeAllocator.allocate();
		ITexture* pTexture = mResourceFactory->createTexture2D("temp-render-target", sortcode, width, height, 
			ETBT_SHADER | ETBT_RENDER_TARGET, nullptr, 1, format);

		GF_PRINT_CONSOLE_INFO("[[[create RT]]]\n");

		if (pTexture == nullptr)
		{
			GF_PRINT_CONSOLE_INFO("Temporary RenderTarget has failed to be created.\n");
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		IRenderTarget* pRenderTarget = pTexture->getRenderTarget();
		pRenderTarget->setTemporary(true);
		return pRenderTarget;
	}

	bool CTextureManager::releaseTempRenderTarget(IRenderTarget* pRenderTarget)
	{
		if (pRenderTarget && pRenderTarget->isTemporary())
		{
			SRenderTargetInfo info = { 0, pRenderTarget->getTexture() };
			mIdledRenderTargets.push_back(info);
			return true;
		}

		return false;
	}

	void CTextureManager::updateTemporaryTextures(u32 delta)
	{
		for (auto it = mIdledRenderTargets.begin(); it != mIdledRenderTargets.end();)
		{
			if (it->IdledTime > _RENDER_STATE_IDLE_TIME_)
			{
				mCodeAllocator.release(it->Texture->getSortCode());
				it->Texture->drop();
				it = mIdledRenderTargets.erase(it);
			}
			else
			{
				it->IdledTime += delta;
				it++;
			}
		}
	}

	void CTextureManager::createStockTextures()
	{
		createShadowMapJitterTexture();
	}

	ITexture3D* CTextureManager::createShadowMapJitterTexture()
	{
		const u32 depth = 32;
		const u32 width = 32;
		const u32 height = 32;
		const u32 SAMPLE_GRID_U = 8;
		const u32 SAMPLE_GRID_V = 8;
		const f32 INV_SAMPLE_GRID_U = 1.0f / 8.0f;
		const f32 INV_SAMPLE_GRID_V = 1.0f / 8.0f;
		
		u32 dataSize = width * height * depth * sizeof(u8)* 4;
		u8* data = (u8*)malloc(dataSize);

		u32 index = 0;

		for (u32 i = 0; i < depth; i++)
		{
			// calculate u, v
			// because each pixel contains two values, so it times 2.
			u32 y = i * 2 / SAMPLE_GRID_V;
			u32 x = i * 2 - SAMPLE_GRID_U * y;

			// write one slice.
			for (u32 j = 0; j < width; j++)
			{
				for (u32 k = 0; k < height; k++)
				{
					f32 u1 = math::RandomFloat(x * INV_SAMPLE_GRID_U, (x + 1) * INV_SAMPLE_GRID_U);
					f32 v1 = math::RandomFloat(y * INV_SAMPLE_GRID_V, (y + 1) * INV_SAMPLE_GRID_V);

					f32 u2 = math::RandomFloat((x + 1) * INV_SAMPLE_GRID_U, (x + 2) * INV_SAMPLE_GRID_U);
					f32 v2 = math::RandomFloat(y * INV_SAMPLE_GRID_V, (y + 1) * INV_SAMPLE_GRID_V);

					f32 offset_x1 = sqrtf(v1) * cosf(XM_2PI * u1);
					f32 offset_y1 = sqrtf(v1) * sinf(XM_2PI * u1);

					f32 offset_x2 = sqrtf(v2) * cosf(XM_2PI * u2);
					f32 offset_y2 = sqrtf(v2) * sinf(XM_2PI * u2);

					data[index++] = static_cast<u8>(offset_x1 * 255);
					data[index++] = static_cast<u8>(offset_y1 * 255);
					data[index++] = static_cast<u8>(offset_x2 * 255);
					data[index++] = static_cast<u8>(offset_y2 * 255);
				}
			}
		}
		
		ITexture3D* pTexture = createTexture3D(ITextureManager::SHADOW_MAP_JITTER_TEXTURE,
			width, height, depth, data, 1, EGF_R8G8B8A8_UNORM);

		return pTexture;
	}



}

