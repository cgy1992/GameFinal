#include "stdafx.h"
#include "gfUtil.h"
#include "D3DUtil.h"
#include "CD3D11Texture.h"
namespace gf
{
	CD3D11Texture::CD3D11Texture(ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pd3dDeviceContext,
		const std::string& name,
		u32 sortcode)
		:ITexture(name, sortcode)
		, md3dDevice(pd3dDevice)
		, md3dDeviceContext(pd3dDeviceContext)
		, m_pd3dTexture(NULL)
		, m_pd3dSRV(NULL)
		, mTextureWidth(0)
		, mTextureHeight(0)
	{

	}

	bool CD3D11Texture::loadFromFile(const std::string& filename)
	{
		HRESULT hr;
		ID3D11Texture2D* pd3dTexture = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;

		/*
		hr = D3DX11CreateShaderResourceViewFromFileA(md3dDevice, filename.c_str(),
		NULL, NULL, &pd3dSRV, NULL);
		if (FAILED(hr))
		{
		GF_PRINT_CONSOLE_INFO("create texture shader view (%s) failed.\n", filename.c_str());
		return false;
		}
		*/



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

		hr = md3dDevice->CreateShaderResourceView(pd3dTexture, &desc, &pd3dSRV);
		if (FAILED(hr))
		{
			GF_PRINT_CONSOLE_INFO("create texture shader view (%s) failed.\n", filename.c_str());
			return false;
		}

		ReleaseCOM(m_pd3dTexture);
		ReleaseCOM(m_pd3dSRV);

		m_pd3dTexture = pd3dTexture;
		m_pd3dSRV = pd3dSRV;

		// update width and height
		mTextureWidth = texDesc.Width;
		mTextureHeight = texDesc.Height;

		return true;
	}

	bool CD3D11Texture::create(u32 width, u32 height, void* rawData, u32 miplevel, E_GI_FORMAT format, u32 pitch)
	{
		HRESULT hr;
		ID3D11Texture2D* pd3dTexture = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = miplevel;
		texDesc.ArraySize = 1;
		texDesc.Format = getDxgiFormat(format);
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA texData;
		texData.pSysMem = rawData;
		if (pitch == 0)
			pitch = getFormatOffset(format) * width;

		texData.SysMemPitch = pitch;
		texData.SysMemSlicePitch = 0;

		hr = md3dDevice->CreateTexture2D(&texDesc, &texData, &pd3dTexture);
		if (FAILED(hr))
		{
			return false;
		}

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

		ReleaseCOM(m_pd3dTexture);
		ReleaseCOM(m_pd3dSRV);

		m_pd3dTexture = pd3dTexture;
		m_pd3dSRV = pd3dSRV;

		// update width and height member
		mTextureWidth = width;
		mTextureHeight = height;

		return true;
	}


	CD3D11Texture::~CD3D11Texture()
	{
		ReleaseCOM(m_pd3dTexture);
		ReleaseCOM(m_pd3dSRV);
	}
}