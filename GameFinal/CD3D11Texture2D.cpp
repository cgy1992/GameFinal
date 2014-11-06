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
		mBindFlags = ETBT_SHADER;

		return true;
	}

	

	bool CD3D11Texture2D::create(u32 width, u32 height, u32 bindFlags,
		void* rawData, u32 miplevel, E_GI_FORMAT format, u32 pitch)
	{
		HRESULT hr;
		ID3D11Texture2D* pd3dTexture = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;
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
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (rawData)
		{
			texDesc.Usage = D3D11_USAGE_IMMUTABLE;

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
			texDesc.Usage = D3D11_USAGE_DEFAULT;

			hr = md3dDevice->CreateTexture2D(&texDesc, NULL, &pd3dTexture);
		}
		

		if (FAILED(hr))
		{
			return false;
		}

		if (bindFlags & ETBT_SHADER)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = texDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;

			hr = md3dDevice->CreateShaderResourceView(pd3dTexture, &srvDesc, &pd3dSRV);
			if (FAILED(hr))
			{
				return false;
			}
		}

		if (bindFlags & ETBT_RENDER_TARGET)
		{
			pRenderTarget = new CD3D11RenderTarget(md3dDevice, md3dDeviceContext);
			if (!pRenderTarget->create(this, pd3dTexture, pd3dSRV, width, height))
			{
				ReleaseReferenceCounted(pRenderTarget);
				return false;
			}
		}

		ReleaseReferenceCounted(mRenderTarget);
		ReleaseReferenceCounted(mDepthStencilSurface);
		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dTexture);
		
		md3dTexture = pd3dTexture;
		md3dSRV = pd3dSRV;
		mRenderTarget = pRenderTarget;

		mTextureWidth = width;
		mTextureHeight = height;
		mFormat = format;

		return true;
	}


	CD3D11Texture2D::~CD3D11Texture2D()
	{
		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dTexture);
	}

	void CD3D11Texture2D::apply(E_SHADER_TYPE shaderType, u32 slot)
	{
		if (md3dSRV)
			md3d11Driver->setTexture(shaderType, slot, md3dSRV);
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
		}
		else if (depthBitNum == 24 && stencilBitNum == 8)
		{
			d3dTexFormat = DXGI_FORMAT_R24G8_TYPELESS;
			d3dDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			if (bindDepthToShader)
				d3dShaderViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			else
				d3dShaderViewFormat = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		}
		else if (depthBitNum == 16 && stencilBitNum == 0)
		{
			d3dTexFormat = DXGI_FORMAT_R16_TYPELESS;
			d3dDepthStencilFormat = DXGI_FORMAT_D16_UNORM;
			d3dShaderViewFormat = DXGI_FORMAT_R16_FLOAT;
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
		if (!pd3dDepthStencilSurface->create(this, pd3dTexture, d3dDepthStencilFormat, pd3dSRV, multiSampling))
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

}

