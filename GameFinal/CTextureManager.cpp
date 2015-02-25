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
			if (pTexture->getType() == ETT_TEXTURE_CUBE)
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

	ITexture* CTextureManager::createTexture1D(
		const std::string& name,
		u32 width,
		u32 bindFlags,
		void* data,
		u32 mipLevel,
		E_GI_FORMAT format,
		E_MEMORY_USAGE memoryUsage)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			ITexture* texture = it->second;
			if (texture->getType() == ETT_TEXTURE_1D)
			{
				GF_PRINT_CONSOLE_INFO("Texture1D named '%s' has already existed.\n", name.c_str());
				return texture;
			}
			else
			{
				GF_PRINT_CONSOLE_INFO("Texture named '%s' has already existed, but it's not a 1D Texture\n", name.c_str());
				return nullptr;
			}
		}

		u32 sortcode = mCodeAllocator.allocate();

		ITexture* pTexture = mResourceFactory->createTexture1D(name, sortcode, width,
			bindFlags, data, mipLevel, format, memoryUsage);

		if (pTexture == nullptr)
		{
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pTexture));
		return pTexture;
	}

	IBuffer* CTextureManager::createBuffer(
		const std::string& name,
		u32 elementNum,
		u32 bindFlags,
		E_GI_FORMAT format,
		u32 elementSize,
		void* rawData,
		E_MEMORY_USAGE memoryUsage)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			ITexture* texture = it->second;
			if (texture->getType() == ETT_BUFFER)
			{
				GF_PRINT_CONSOLE_INFO("Buffer named '%s' has already existed.\n", name.c_str());
				return dynamic_cast<IBuffer*>(texture);
			}
			else
			{
				GF_PRINT_CONSOLE_INFO("Texture named '%s' has already existed, but it's not a Buffer\n", name.c_str());
				return nullptr;
			}
		}


		u32 sortcode = mCodeAllocator.allocate();

		IBuffer* pBuffer = mResourceFactory->createBuffer(name, sortcode,
			elementNum, bindFlags, format, elementSize, rawData, memoryUsage);

		if (pBuffer == nullptr)
		{
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pBuffer));
		return pBuffer;
	}

	ITexture* CTextureManager::createTexture2D(
		const std::string& name,
		u32 width,
		u32 height,
		u32 bindFlags,
		void* data,
		u32 mipLevel,
		E_GI_FORMAT format,
		u32 pitch,
		E_MEMORY_USAGE memoryUsage)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			ITexture* texture = it->second;
			if (texture->getType() == ETT_TEXTURE_2D)
			{
				GF_PRINT_CONSOLE_INFO("Texture2D named '%s' has already existed.\n", name.c_str());
				return texture;
			}
			else
			{
				GF_PRINT_CONSOLE_INFO("Texture named '%s' has already existed, but it's not a 2D Texture\n", name.c_str());
				return nullptr;
			}
		}

		u32 sortcode = mCodeAllocator.allocate();
		ITexture* pTexture = mResourceFactory->createTexture2D(name, sortcode, width, 
			height, bindFlags, data, mipLevel, format, pitch, memoryUsage);
		if (pTexture == nullptr)
		{
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pTexture));
		return pTexture;
	}

	ITexture2DArray* CTextureManager::createTexture2DArray(
		const std::string& name,
		u32 width,
		u32 height,
		u32 arraySize,
		u32 bindFlags,
		void* data,
		u32 mipLevel,
		E_GI_FORMAT format,
		u32 pitch,
		E_MEMORY_USAGE memoryUsage)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			ITexture* pTexture = it->second;
			if (pTexture->getType() == ETT_TEXTURE_2D_ARRAY)
			{
				GF_PRINT_CONSOLE_INFO("Texture named '%s' has already existed.\n", name.c_str());
				return dynamic_cast<ITexture2DArray*>(pTexture);
			}
			else
			{
				GF_PRINT_CONSOLE_INFO("Texture file named '%s' has already been loaded. But it is not a texture2D array texture.\n", name.c_str());
				return nullptr;
			}
		}


		u32 sortcode = mCodeAllocator.allocate();
		ITexture2DArray* pTexture = mResourceFactory->createTexture2DArray(name, sortcode, width, height, 
			arraySize, bindFlags, data, mipLevel, format, pitch, memoryUsage);
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
		u32 bindFlags,
		void* data,
		u32 mipLevel,
		E_GI_FORMAT format,
		u32 pitch,
		u32 slicePitch,
		E_MEMORY_USAGE memoryUsage)
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
		ITexture3D* pTexture = mResourceFactory->createTexture3D(name, sortcode, width, height, depth, bindFlags, 
			data, mipLevel, format, pitch, slicePitch, memoryUsage);
		
		if (pTexture == nullptr)
		{
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		mTextureMap.insert(std::make_pair(name, pTexture));
		return pTexture;
	}

	
	ITextureCube* CTextureManager::createTextureCube(
		const std::string& name,
		u32 size,
		u32 bindFlags,
		void* data,
		u32 miplevel,
		E_GI_FORMAT format,
		u32 pitch,
		E_MEMORY_USAGE memoryUsage)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			if (it->second->getType() == ETT_TEXTURE_CUBE)
			{
				GF_PRINT_CONSOLE_INFO("CreateCube named '%s' has already existed.\n", name.c_str());
				return dynamic_cast<ITextureCube*>(it->second);
			}
			else
			{
				GF_PRINT_CONSOLE_INFO("CreateCube named '%s' has already existed and it is not a kind of CreateCube.\n", name.c_str());
				return nullptr;
			}
		}

		u32 sortcode = mCodeAllocator.allocate();
		ITextureCube* pTexture = mResourceFactory->createTextureCube(name, sortcode, size, bindFlags, 
			data, miplevel, format, pitch, memoryUsage);
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
		ITexture* pTexture = mResourceFactory->createTexture2D(name, sortcode, width, height, ETBT_SHADER_RESOURCE | ETBT_RENDER_TARGET,
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

		IVideoDriver* driver = IVideoDriver::getInstance();

		if (width == 0)
			width = driver->getBackBufferWidth();
		if (height == 0)
			height = driver->getBackBufferHeight();

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

	ITextureCube* CTextureManager::getTextureCube(const std::string& name, bool bLoadIfNotExist)
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			ITexture* texture = it->second;
			if (texture->getType() == ETT_TEXTURE_CUBE)
				return dynamic_cast<ITextureCube*>(texture);

			return nullptr;
		}

		if (bLoadIfNotExist)
			return loadCubeTexture(name);

		return nullptr;
	}

	IBuffer* CTextureManager::getBuffer(const std::string& name) const
	{
		auto it = mTextureMap.find(name);
		if (it != mTextureMap.end())
		{
			ITexture* texture = it->second;
			if (texture->getType() == ETT_BUFFER)
				return dynamic_cast<IBuffer*>(texture);
		}
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
			ETBT_SHADER_RESOURCE | ETBT_RENDER_TARGET, nullptr, 1, format);

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
			STemporaryTextureInfo info = { 0, pRenderTarget->getTexture() };
			mIdledRenderTargets.push_back(info);
			return true;
		}

		return false;
	}

	IDepthStencilSurface* CTextureManager::getTempDepthStencilSurface(u32 width /* = 0 */, u32 height /* = 0 */,
		u32 depthBits /* = 32 */, u32 stencilBits /* = 0 */)
	{
		if (width == 0)
			width = (u32)IDevice::getInstance()->getClientWidth();
		if (height == 0)
			height = (u32)IDevice::getInstance()->getClientHeight();

		// find the longest idled time, the header of this list.
		if (!mIdledDepthStencilTextures.empty())
		{
			for (auto it = mIdledDepthStencilTextures.begin(); it != mIdledDepthStencilTextures.end(); it++)
			{
				ITexture* p = it->Texture;
				IDepthStencilSurface* pDepthStencilSurface = p->getDepthStencilSurface();
				if (pDepthStencilSurface->getWidth() == width
					&& pDepthStencilSurface->getHeight() == height
					&& pDepthStencilSurface->getDepthBits() == depthBits
					&& pDepthStencilSurface->getStencilBits() == stencilBits)
				{
					mIdledDepthStencilTextures.erase(it);
					return pDepthStencilSurface;
				}
			}
		}

		// if no idled render target, then create one.
		u32 sortcode = mCodeAllocator.allocate();
		ITexture* pDepthStencilTexture = mResourceFactory->createDepthStencilTexture("temp-depth-stencil-surface",
			sortcode, width, height, depthBits, stencilBits, false, 1, 0, true, true);

		GF_PRINT_CONSOLE_INFO("[[[create DS]]]\n");

		if (!pDepthStencilTexture)
		{
			GF_PRINT_CONSOLE_INFO("Temporary DepthStencilSurface has failed to be created.\n");
			mCodeAllocator.release(sortcode);
			return nullptr;
		}

		IDepthStencilSurface* pDepthStencilSurface = pDepthStencilTexture->getDepthStencilSurface();
		pDepthStencilSurface->setTemporary(true);
		return pDepthStencilSurface;
	}

	bool CTextureManager::releaseTempDepthStencilSurface(IDepthStencilSurface* pDepthStencilSurface)
	{
		if (pDepthStencilSurface && pDepthStencilSurface->isTemporary())
		{
			STemporaryTextureInfo info = { 0, pDepthStencilSurface->getTexture() };
			mIdledDepthStencilTextures.push_back(info);
			return true;
		}
		return false;
	}

	void CTextureManager::updateTemporaryTextures(f32 delta)
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

		for (auto it = mIdledDepthStencilTextures.begin(); it != mIdledDepthStencilTextures.end();)
		{
			if (it->IdledTime > _RENDER_STATE_IDLE_TIME_)
			{
				mCodeAllocator.release(it->Texture->getSortCode());
				it->Texture->drop();
				it = mIdledDepthStencilTextures.erase(it);
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
		createPointLightShadowMapJitterTexture();
	}

	void CTextureManager::createShadowMapJitterTexture()
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
		
		createTexture3D(ITextureManager::SHADOW_MAP_JITTER_TEXTURE,
			width, height, depth, ETBT_SHADER_RESOURCE, data, 1, EGF_R8G8B8A8_UNORM);
		
		free(data);
	}

	void CTextureManager::createPointLightShadowMapJitterTexture()
	{
		const u32 SAMPLE_GRID_R = 4;
		const u32 SAMPLE_GRID_U = 2;
		const u32 SAMPLE_GRID_V = 2;
		const f32 INV_SAMPLE_GRID_R = 1.0f / SAMPLE_GRID_R;
		const f32 INV_SAMPLE_GRID_U = 1.0f / SAMPLE_GRID_U;
		const f32 INV_SAMPLE_GRID_V = 1.0f / SAMPLE_GRID_V;
		const u32 depth = SAMPLE_GRID_R * SAMPLE_GRID_U * SAMPLE_GRID_V;
		const u32 width = 32;
		const u32 height = 32;

		u32 dataSize = width * height * depth * sizeof(u8)* 4;
		u8* data = (u8*)malloc(dataSize);
		u32 index = 0;

		XMFLOAT3 offsets[depth];
		for (u32 r = 0; r < SAMPLE_GRID_R; r++)
		for (u32 u = 0; u < SAMPLE_GRID_U; u++)
		for (u32 v = 0; v < SAMPLE_GRID_V; v++)
			offsets[index++] = XMFLOAT3(r * INV_SAMPLE_GRID_R, u * INV_SAMPLE_GRID_U, v * INV_SAMPLE_GRID_V);

		index = 0;
		for (u32 i = 0; i < depth; i++)
		{
			// calculate r, u, v
			f32 x = offsets[i].x;
			f32 y = offsets[i].y;
			f32 z = offsets[i].z;

			// write one slice.
			for (u32 j = 0; j < width; j++)
			{
				for (u32 k = 0; k < height; k++)
				{
					f32 r = math::RandomFloat(x, x + INV_SAMPLE_GRID_R);
					f32 u = math::RandomFloat(y, y + INV_SAMPLE_GRID_U);
					f32 v = math::RandomFloat(z, z + INV_SAMPLE_GRID_V);

					// Çò×ø±êÏµ
					r = sqrtf(r);
					f32 phi = XM_2PI * u; // (0, 2PI)
					f32 theta = XM_PI * v; // (0, PI)

					f32 dx = r * sinf(theta) * cosf(phi);
					f32 dy = r * sinf(theta) * sinf(phi);
					f32 dz = r * cosf(theta);

					data[index++] = static_cast<u8>(dx * 255);
					data[index++] = static_cast<u8>(dy * 255);
					data[index++] = static_cast<u8>(dz * 255);
					data[index++] = static_cast<u8>(0 * 255);
				}
			}
		}
		
		createTexture3D(ITextureManager::PL_SHADOW_MAP_JITTER_TEXTURE,
			width, height, depth, ETBT_SHADER_RESOURCE, data, 1, EGF_R8G8B8A8_UNORM);

		free(data);
	}

	CTextureManager::~CTextureManager()
	{
		destroyAll();
		
		for (auto it = mIdledRenderTargets.begin(); it != mIdledRenderTargets.end(); it++)
		{
			ReleaseReferenceCounted(it->Texture);
		}

		for (auto it = mIdledDepthStencilTextures.begin(); it != mIdledDepthStencilTextures.end(); it++)
		{
			ReleaseReferenceCounted(it->Texture);
		}
	}



}

