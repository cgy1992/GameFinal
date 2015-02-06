#include "stdafx.h"
#include "CD3D11Texture3D.h"
#include "D3DUtil.h"

namespace gf
{

	bool CD3D11Texture3D::create(u32 width, u32 height,
		u32 depth, u32 bindFlags, void* rawData,
		u32 miplevel, E_GI_FORMAT format, u32 pitch /*= 0*/, 
		u32 slicePitch /*= 0*/, E_MEMORY_USAGE memoryUsage)
	{
		HRESULT hr;
		ID3D11Texture3D* pd3dTexture3D = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;
		ID3D11UnorderedAccessView* pd3dUAV = NULL;

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
		texDesc.Usage = getD3d11Usage(memoryUsage);

		if (pitch == 0)
			pitch = getFormatOffset(format) * width;

		if (slicePitch == 0)
			slicePitch = getFormatOffset(format) * width * height;

		if (rawData)
		{
			if (memoryUsage == EMU_UNKNOWN)
				texDesc.Usage = D3D11_USAGE_IMMUTABLE;

			D3D11_SUBRESOURCE_DATA texData;
			texData.pSysMem = rawData;
			texData.SysMemPitch = pitch;
			texData.SysMemSlicePitch = slicePitch;

			hr = md3dDevice->CreateTexture3D(&texDesc, &texData, &pd3dTexture3D);
		}
		else
		{
			if (memoryUsage == EMU_UNKNOWN)
				texDesc.Usage = D3D11_USAGE_DEFAULT;

			md3dDevice->CreateTexture3D(&texDesc, NULL, &pd3dTexture3D);

		}

		if (FAILED(hr))
		{
			return false;
		}

		if (bindFlags & ETBT_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = texDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			srvDesc.Texture3D.MostDetailedMip = 0;
			srvDesc.Texture3D.MipLevels = texDesc.MipLevels;

			hr = md3dDevice->CreateShaderResourceView(pd3dTexture3D, &srvDesc, &pd3dSRV);
			if (FAILED(hr))
			{
				ReleaseCOM(pd3dTexture3D);
				return false;
			}
		}

		if (bindFlags & ETBT_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.Format = texDesc.Format;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

			uavDesc.Texture3D.MipSlice = 0;
			uavDesc.Texture3D.FirstWSlice = 0;
			uavDesc.Texture3D.WSize = texDesc.Depth;

			hr = md3dDevice->CreateUnorderedAccessView(pd3dTexture3D, &uavDesc, &pd3dUAV);
			if (FAILED(hr))
			{
				ReleaseCOM(pd3dTexture3D);
				return false;
			}
		}

		ReleaseCOM(md3dShaderResourceView);
		ReleaseCOM(md3dUAV);
		ReleaseCOM(md3dTexture);		

		md3dTexture = pd3dTexture3D;
		md3dShaderResourceView = pd3dSRV;
		md3dUAV = pd3dUAV;

		mTextureWidth = width;
		mTextureHeight = height;
		mTextureDepth = depth;
		mFormat = format;

		return true;
	}

	CD3D11Texture3D::~CD3D11Texture3D()
	{
		ReleaseCOM(md3dShaderResourceView);
		ReleaseCOM(md3dUAV);
		ReleaseCOM(md3dTexture);
	}

	void CD3D11Texture3D::apply(E_SHADER_TYPE shaderType, u32 slot,
		E_TEXTURE_BIND_TYPE bindType)
	{
		if (bindType == ETBT_SHADER_RESOURCE)
		{
			if (md3dShaderResourceView)
				md3d11Driver->setTexture(shaderType, slot, md3dShaderResourceView);
		}
		else if (bindType == ETBT_UNORDERED_ACCESS)
		{
			if (md3dUAV)
				md3d11Driver->setRWTexture(slot, md3dUAV);
		}
	}

	gf::u32 CD3D11Texture3D::getElementSize() const
	{
		return getFormatOffset(mFormat);
	}

	bool CD3D11Texture3D::copyDataToAnotherTexture(ITexture* dest)
	{
		if (!dest)
			return false;

		if (dest->getType() != ETT_TEXTURE_3D) {
			GF_PRINT_CONSOLE_INFO("Texture's data cannot be copied between different types\n");
			return false;
		}

		CD3D11Texture3D* pAnotherTexture = dynamic_cast<CD3D11Texture3D*>(dest);

		if (getElementSize() != pAnotherTexture->getElementSize())
		{
			GF_PRINT_CONSOLE_INFO("Buffers with different element size couldn't copy data with each other.\n");
			return false;
		}

		if (mTextureWidth > pAnotherTexture->getWidth() 
			|| mTextureHeight > pAnotherTexture->getHeight()
			|| mTextureDepth > pAnotherTexture->getDepth())
		{
			GF_PRINT_CONSOLE_INFO("Destination Buffer' size is smaller than Source Buffer.\n");
			return false;
		}

		md3dDeviceContext->CopyResource(pAnotherTexture->md3dTexture, md3dTexture);
		return true;
	}

	bool CD3D11Texture3D::lock(E_TEXTURE_LOCK_TYPE lockType, STextureData* texData, u32 index)
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

	void CD3D11Texture3D::unlock()
	{
		md3dDeviceContext->Unmap(md3dTexture, 0);
	}
}

