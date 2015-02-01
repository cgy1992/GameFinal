#include "stdafx.h"
#include "gfUtil.h"
#include "D3DUtil.h"
#include "CD3D11Texture1D.h"

namespace gf
{
	CD3D11Texture1D::CD3D11Texture1D(ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pd3dDeviceContext, 
		const std::string& name, 
		u32 sortCode, 
		CD3D11Driver* d3d11Driver)
		:ITexture(name, sortCode)
		, md3dDevice(pd3dDevice)
		, md3dDeviceContext(pd3dDeviceContext)
		, md3dTexture(NULL)
		, md3dSRV(NULL)
		, md3dUAV(NULL)
		, mTextureWidth(0)
		, md3d11Driver(d3d11Driver)
		, mBindFlags(0)
	{

	}

	bool CD3D11Texture1D::create(u32 width, u32 bindFlags,
		void* rawData, u32 miplevel, E_GI_FORMAT format,
		E_MEMORY_USAGE memoryUsage)
	{
		HRESULT hr;
		ID3D11Texture1D* pd3dTexture = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;
		ID3D11UnorderedAccessView* pd3dUAV = NULL;

		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = width;
		texDesc.MipLevels = miplevel;
		texDesc.ArraySize = 1;
		texDesc.Format = getDxgiFormat(format);
		texDesc.BindFlags = getD3dx11BindFlags(bindFlags);
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.Usage = getD3d11Usage(memoryUsage);

		UINT support;
		md3dDevice->CheckFormatSupport(DXGI_FORMAT_R32_FLOAT, &support);

		assert(support & D3D11_FORMAT_SUPPORT_TEXTURE1D);

		//D3D11_FORMAT_SUPPORT 

		if (rawData)
		{
			if (memoryUsage == EMU_UNKNOWN)
				texDesc.Usage = D3D11_USAGE_IMMUTABLE;
			
			D3D11_SUBRESOURCE_DATA texData;
			texData.pSysMem = rawData;

			u32 pitch = getFormatOffset(format) * width;
			texData.SysMemPitch = pitch;
			texData.SysMemSlicePitch = 0;
			hr = md3dDevice->CreateTexture1D(&texDesc, &texData, &pd3dTexture);
		}
		else
		{
			if (memoryUsage == EMU_UNKNOWN)
				texDesc.Usage = D3D11_USAGE_DEFAULT;
			
			hr = md3dDevice->CreateTexture1D(&texDesc, NULL, &pd3dTexture);
		}

		if (FAILED(hr))
		{
			return false;
		}

		if (bindFlags & ETBT_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = texDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
			srvDesc.Texture1D.MostDetailedMip = 0;
			srvDesc.Texture1D.MipLevels = -1;

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
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
			uavDesc.Texture1D.MipSlice = 0;
			hr = md3dDevice->CreateUnorderedAccessView(pd3dTexture, &uavDesc, &pd3dUAV);
			if (FAILED(hr))
			{
				ReleaseCOM(pd3dTexture);
				return false;
			}
		}

		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dUAV);
		ReleaseCOM(md3dTexture);


		md3dTexture = pd3dTexture;
		md3dSRV = pd3dSRV;
		md3dUAV = pd3dUAV;
		mTextureWidth = width;
		mFormat = format;

		return true;
	}

	CD3D11Texture1D::~CD3D11Texture1D()
	{
		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dUAV);
		ReleaseCOM(md3dTexture);
	}

	void CD3D11Texture1D::apply(E_SHADER_TYPE shaderType, u32 slot,E_TEXTURE_BIND_TYPE bindType)
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

	u32 CD3D11Texture1D::getElementSize() const
	{
		return getFormatOffset(mFormat);
	}

}
