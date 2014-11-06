#include "stdafx.h"
#include "CD3D11Texture3D.h"
#include "D3DUtil.h"

namespace gf
{

	bool CD3D11Texture3D::create(u32 width, u32 height,
		u32 depth, void* rawData,
		u32 miplevel, E_GI_FORMAT format, u32 pitch /*= 0*/, u32 slicePitch /*= 0*/)
	{
		HRESULT hr;
		ID3D11Texture3D* pd3dTexture3D = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;

		D3D11_TEXTURE3D_DESC texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.Depth = depth;
		texDesc.MipLevels = miplevel;
		texDesc.Format = getDxgiFormat(format);
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (pitch == 0)
			pitch = getFormatOffset(format) * width;

		if (slicePitch == 0)
			slicePitch = getFormatOffset(format) * width * height;

		D3D11_SUBRESOURCE_DATA texData;
		texData.pSysMem = rawData;
		texData.SysMemPitch = pitch;
		texData.SysMemSlicePitch = slicePitch;

		hr = md3dDevice->CreateTexture3D(&texDesc, &texData, &pd3dTexture3D);
		if (FAILED(hr))
		{
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MostDetailedMip = 0;
		srvDesc.Texture3D.MipLevels = texDesc.MipLevels;

		hr = md3dDevice->CreateShaderResourceView(pd3dTexture3D, &srvDesc, &pd3dSRV);
		if (FAILED(hr))
		{
			return false;
		}

		ReleaseCOM(md3dShaderResourceView);
		ReleaseCOM(md3dTexture);
		

		md3dTexture = pd3dTexture3D;
		md3dShaderResourceView = pd3dSRV;

		mTextureWidth = width;
		mTextureHeight = height;
		mTextureDepth = depth;
		mFormat = format;

		return true;
	}

	CD3D11Texture3D::~CD3D11Texture3D()
	{
		ReleaseCOM(md3dShaderResourceView);
		ReleaseCOM(md3dTexture);
	}

	void CD3D11Texture3D::apply(E_SHADER_TYPE shaderType, u32 slot)
	{
		if (md3dTexture)
			md3d11Driver->setTexture(shaderType, slot, md3dShaderResourceView);
	}


}

