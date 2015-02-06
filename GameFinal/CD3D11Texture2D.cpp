#include "stdafx.h"
#include "gfUtil.h"
#include "D3DUtil.h"
#include "CD3D11Texture2D.h"


namespace gf
{
	CD3D11Texture2D::CD3D11Texture2D(ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pd3dDeviceContext,
		const std::string& name,
		u32 sortcode,
		CD3D11Driver* d3d11Driver)
		:ITexture(name, sortcode)
		, md3dDevice(pd3dDevice)
		, md3dDeviceContext(pd3dDeviceContext)
		, md3dTexture(NULL)
		, md3dSRV(NULL)
		, md3dUAV(NULL)
		, mTextureWidth(0)
		, mTextureHeight(0)
		, md3d11Driver(d3d11Driver)
		, mBindFlags(0)
		, mRenderTarget(nullptr)
		, mDepthStencilSurface(nullptr)
	{

	}

	bool CD3D11Texture2D::loadFromFile(const std::string& filename)
	{
		HRESULT hr;
		ID3D11Texture2D* pd3dTexture = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;

		hr = D3DX11CreateTextureFromFileA(md3dDevice, filename.c_str(),
			NULL, NULL, (ID3D11Resource**)&pd3dTexture, NULL);
		if (FAILED(hr))
		{
			GF_PRINT_CONSOLE_INFO("load texture file (%s) failed.\n", filename.c_str());
			return false;
		}

		D3D11_TEXTURE2D_DESC texDesc;
		pd3dTexture->GetDesc(&texDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format = texDesc.Format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = texDesc.MipLevels;
		desc.Texture2D.MostDetailedMip = 0;

		mFormat = getFormatFromDxgi(texDesc.Format);

		hr = md3dDevice->CreateShaderResourceView(pd3dTexture, &desc, &pd3dSRV);
		if (FAILED(hr))
		{
			GF_PRINT_CONSOLE_INFO("create texture shader view (%s) failed.\n", filename.c_str());
			return false;
		}

		ReleaseCOM(md3dTexture);
		ReleaseCOM(md3dSRV);

		md3dTexture = pd3dTexture;
		md3dSRV = pd3dSRV;

		// update width and height
		mTextureWidth = texDesc.Width;
		mTextureHeight = texDesc.Height;
		mBindFlags = ETBT_SHADER_RESOURCE;

		return true;
	}

	

	bool CD3D11Texture2D::create(u32 width, u32 height, u32 bindFlags,
		void* rawData, u32 miplevel, E_GI_FORMAT format, u32 pitch,
		E_MEMORY_USAGE memoryUsage)
	{
		HRESULT hr;
		ID3D11Texture2D* pd3dTexture = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;
		ID3D11UnorderedAccessView* pd3dUAV = NULL;
		CD3D11RenderTarget* pRenderTarget = nullptr;

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = miplevel;
		texDesc.ArraySize = 1;
		texDesc.Format = getDxgiFormat(format);
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.BindFlags = getD3dx11BindFlags(bindFlags);
		texDesc.CPUAccessFlags = getD3dx11CpuAccessFlag(bindFlags);
		texDesc.MiscFlags = 0;
		texDesc.Usage = getD3d11Usage(memoryUsage);

		if (memoryUsage == EMU_UNKNOWN)
		{
			if ((bindFlags & ETBT_CPU_ACCESS_READ)) {
				texDesc.Usage = D3D11_USAGE_STAGING;
				memoryUsage = EMU_STAGING;
			}
			else if (bindFlags & ETBT_CPU_ACCESS_WRITE) {
				texDesc.Usage = D3D11_USAGE_DEFAULT;
				memoryUsage = EMU_DEFAULT;
			}
			else if (rawData) {
				texDesc.Usage = D3D11_USAGE_IMMUTABLE;
				memoryUsage = EMU_STATIC;
			}
			else if (!rawData) {
				texDesc.Usage = D3D11_USAGE_DEFAULT;
				memoryUsage = EMU_DEFAULT;
			}
		}
		else
		{
			if (memoryUsage == EMU_DEFAULT || memoryUsage == EMU_STATIC)
			{
				if ((bindFlags & ETBT_CPU_ACCESS_READ) || (bindFlags & ETBT_CPU_ACCESS_WRITE))
				{
					GF_PRINT_CONSOLE_INFO("Static or Default Buffer cannot be accessed by CPU.\n");
					return false;
				}
			}
			else if (memoryUsage == EMU_DYNAMIC)
			{
				if (bindFlags & ETBT_CPU_ACCESS_READ)
				{
					GF_PRINT_CONSOLE_INFO("Dynamic Buffer cannot be read by CPU.\n");
					return false;
				}
			}
		}

		if (memoryUsage == D3D11_USAGE_STAGING)
		{
			if ((bindFlags & ETBT_SHADER_RESOURCE) || (bindFlags & ETBT_UNORDERED_ACCESS)){
				GF_PRINT_CONSOLE_INFO("Buffer with the memory usage 'STARING' cannot have SHADER_RESOURCE or UNORDERED_ACCESS.");
				return false;
			}
		}

		if (rawData)
		{
			D3D11_SUBRESOURCE_DATA texData;
			texData.pSysMem = rawData;
			if (pitch == 0)
				pitch = getFormatOffset(format) * width;

			texData.SysMemPitch = pitch;
			texData.SysMemSlicePitch = 0;
			hr = md3dDevice->CreateTexture2D(&texDesc, &texData, &pd3dTexture);
		}
		else
		{
			hr = md3dDevice->CreateTexture2D(&texDesc, NULL, &pd3dTexture);
		}
		

		if (FAILED(hr))
		{
			return false;
		}

		if (bindFlags & ETBT_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = texDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;

			hr = md3dDevice->CreateShaderResourceView(pd3dTexture, &srvDesc, &pd3dSRV);
			if (FAILED(hr))
			{
				ReleaseCOM(pd3dTexture);
				return false;
			}
		}

		if (bindFlags & ETBT_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.Format = texDesc.Format;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = 0;

			hr = md3dDevice->CreateUnorderedAccessView(pd3dTexture, &uavDesc, &pd3dUAV);
			if (FAILED(hr))
			{
				ReleaseCOM(pd3dTexture);
				return false;
			}
		}


		if (bindFlags & ETBT_RENDER_TARGET)
		{
			pRenderTarget = new CD3D11RenderTarget(md3dDevice, md3dDeviceContext);
			if (!pRenderTarget->create(this, pd3dTexture, pd3dSRV, width, height, format))
			{
				ReleaseReferenceCounted(pRenderTarget);
				ReleaseCOM(pd3dSRV);
				ReleaseCOM(pd3dTexture);
				return false;
			}
		}

		ReleaseReferenceCounted(mRenderTarget);
		ReleaseReferenceCounted(mDepthStencilSurface);
		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dUAV);
		ReleaseCOM(md3dTexture);
		
		md3dTexture = pd3dTexture;
		md3dSRV = pd3dSRV;
		md3dUAV = pd3dUAV;
		mRenderTarget = pRenderTarget;

		mTextureWidth = width;
		mTextureHeight = height;
		mFormat = format;

		return true;
	}


	CD3D11Texture2D::~CD3D11Texture2D()
	{
		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dUAV);
		ReleaseReferenceCounted(mDepthStencilSurface);
		ReleaseReferenceCounted(mRenderTarget);
		ReleaseCOM(md3dTexture);
	}

	void CD3D11Texture2D::apply(E_SHADER_TYPE shaderType, u32 slot, E_TEXTURE_BIND_TYPE bindType)
	{
		if (bindType == ETBT_SHADER_RESOURCE)
		{
			if (md3dSRV)
				md3d11Driver->setTexture(shaderType, slot, md3dSRV);
		}
		else if (bindType == ETBT_UNORDERED_ACCESS)
		{
			if (md3dUAV)
				md3d11Driver->setRWTexture(slot, md3dUAV);
		}
	}

	bool CD3D11Texture2D::createDepthStencilTexture(u32 width, u32 height, 
		u32 depthBitNum, u32 stencilBitNum, 
		bool multiSampling, u32 multiSamplingCount, u32 multiSamplingQuality,
		bool bShaderBound /*= true*/, bool bindDepthToShader /*= true*/)
	{
		DXGI_FORMAT d3dTexFormat;
		DXGI_FORMAT d3dDepthStencilFormat;
		DXGI_FORMAT d3dShaderViewFormat;
		ID3D11Texture2D* pd3dTexture = nullptr;
		CD3D11DepthStencilSurface* pd3dDepthStencilSurface = nullptr;
		ID3D11ShaderResourceView* pd3dSRV = nullptr;
		HRESULT hr;

		if (!multiSampling)
		{
			multiSamplingCount = 1;
			multiSamplingQuality = 0;
		}

		if (depthBitNum == 32 && stencilBitNum == 0)
		{
			d3dTexFormat = DXGI_FORMAT_R32_TYPELESS;
			d3dDepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
			d3dShaderViewFormat = DXGI_FORMAT_R32_FLOAT;
			mFormat = EGF_D32;
		}
		else if (depthBitNum == 24 && stencilBitNum == 8)
		{
			d3dTexFormat = DXGI_FORMAT_R24G8_TYPELESS;
			d3dDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			if (bindDepthToShader)
				d3dShaderViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			else
				d3dShaderViewFormat = DXGI_FORMAT_X24_TYPELESS_G8_UINT;

			mFormat = EGF_D24S8;
		}
		else if (depthBitNum == 16 && stencilBitNum == 0)
		{
			d3dTexFormat = DXGI_FORMAT_R16_TYPELESS;
			d3dDepthStencilFormat = DXGI_FORMAT_D16_UNORM;
			d3dShaderViewFormat = DXGI_FORMAT_R16_FLOAT;
			mFormat = EGF_D16;
		}
		else {
			GF_PRINT_CONSOLE_INFO("The format of depth-stencil-surface ('%s') is invalid.\n", mName.c_str());
			return false;
		}

		// create texture
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));

		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = d3dTexFormat;
		texDesc.SampleDesc.Count = multiSamplingCount;
		texDesc.SampleDesc.Quality = multiSamplingQuality;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		if (bShaderBound)
			texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		hr = md3dDevice->CreateTexture2D(&texDesc, NULL, &pd3dTexture);
		if (FAILED(hr))
		{
			GF_PRINT_CONSOLE_INFO("The Depth-stencil-surface('%s') has failed to be created. Because of the failure of texture creation.\n", mName.c_str());
			return false;
		}

		// create shader-resource-view, if needed.
		if (bShaderBound)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = d3dShaderViewFormat;
			if (multiSampling)
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			else{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;
			}

			hr = md3dDevice->CreateShaderResourceView(pd3dTexture, &srvDesc, &pd3dSRV);
			if (FAILED(hr))
			{
				GF_PRINT_CONSOLE_INFO("The Depth-stencil-surface('%s') has failed to be created.\
						Because of the failure of ShaderResourceView creation.\n", mName.c_str());
				ReleaseCOM(pd3dTexture);
				return false;
			}
		}
		else
		{
			pd3dSRV = nullptr;
		}


		// create depth-stencil
		pd3dDepthStencilSurface = new CD3D11DepthStencilSurface(md3dDevice, md3dDeviceContext);
		if (!pd3dDepthStencilSurface->create(this, pd3dTexture, d3dDepthStencilFormat, pd3dSRV, multiSampling, 
			width, height, depthBitNum, stencilBitNum))
		{
			ReleaseReferenceCounted(pd3dDepthStencilSurface);
			ReleaseCOM(pd3dTexture);
			return false;
		}

		ReleaseReferenceCounted(mRenderTarget);
		ReleaseReferenceCounted(mDepthStencilSurface);
		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dTexture);

		md3dTexture = pd3dTexture;
		md3dSRV = pd3dSRV;
		mDepthStencilSurface = pd3dDepthStencilSurface;
		mRenderTarget = nullptr;

		mTextureWidth = width;
		mTextureHeight = height;

		return true;
	}

	u32 CD3D11Texture2D::getElementSize() const
	{
		return getFormatOffset(mFormat);
	}

	bool CD3D11Texture2D::copyDataToAnotherTexture(ITexture* dest)
	{
		if (!dest)
			return false;

		if (dest->getType() != ETT_TEXTURE_2D) {
			GF_PRINT_CONSOLE_INFO("Texture's data cannot be copied between different types\n");
			return false;
		}

		CD3D11Texture2D* pAnotherTexture = dynamic_cast<CD3D11Texture2D*>(dest);

		if (getElementSize() != pAnotherTexture->getElementSize())
		{
			GF_PRINT_CONSOLE_INFO("Buffers with different element size couldn't copy data with each other.\n");
			return false;
		}

		if (mTextureWidth > pAnotherTexture->getWidth() || mTextureHeight > pAnotherTexture->getHeight())
		{
			GF_PRINT_CONSOLE_INFO("Destination Buffer' size is smaller than Source Buffer.\n");
			return false;
		}

		md3dDeviceContext->CopyResource(pAnotherTexture->md3dTexture, md3dTexture);
		return true;
	}

	bool CD3D11Texture2D::lock(E_TEXTURE_LOCK_TYPE lockType, STextureData* texData, u32 index)
	{
		D3D11_MAP MapType = getD3d11MapType(lockType);
		D3D11_MAPPED_SUBRESOURCE mappedData;
		HRESULT hr = md3dDeviceContext->Map(md3dTexture, index, MapType, 0, &mappedData);
		if (FAILED(hr))
			return false;
		if (texData)
		{
			texData->Data = mappedData.pData;
			texData->RowPitch = mappedData.RowPitch;
			texData->DepthPitch = mappedData.DepthPitch;
		}
		return true;
	}

	void CD3D11Texture2D::unlock()
	{
		md3dDeviceContext->Unmap(md3dTexture, 0);
	}
}

