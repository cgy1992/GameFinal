#include "stdafx.h"
#include "CD3D11Texture2DArray.h"

namespace gf
{
	CD3D11Texture2DArray::CD3D11Texture2DArray(ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pd3dDeviceContext,
		const std::string& name, u32 sortcode,
		CD3D11Driver* d3d11Driver)
		:ITexture2DArray(name, sortcode)
		, md3dDevice(pd3dDevice)
		, md3dDeviceContext(pd3dDeviceContext)
		, md3dTexture(NULL)
		, md3dSRV(NULL)
		, md3dUAV(NULL)
		, mTextureWidth(0)
		, mTextureHeight(0)
		, md3d11Driver(d3d11Driver)
		, mBindFlags(0)
		, mArraySize(0)
	{

	}

	CD3D11Texture2DArray::~CD3D11Texture2DArray()
	{
		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dUAV);

		for (u32 i = 0; i < mRenderTargets.size(); i++)
			ReleaseReferenceCounted(mRenderTargets[i]);

		for (u32 i = 0; i < mDepthStencilSurfaces.size(); i++)
			ReleaseReferenceCounted(mDepthStencilSurfaces[i]);

		ReleaseCOM(md3dTexture);
	}

	void CD3D11Texture2DArray::apply(E_SHADER_TYPE shaderType, u32 slot,
		E_TEXTURE_BIND_TYPE bindType)
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

	bool CD3D11Texture2DArray::create(u32 width, u32 height, u32 arraySize, u32 bindFlags, 
		void* rawData, u32 miplevel, E_GI_FORMAT format, u32 pitch /*= 0*/,
		E_MEMORY_USAGE memoryUsage)
	{
		HRESULT hr;
		ID3D11Texture2D* pd3dTexture = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;
		ID3D11UnorderedAccessView* pd3dUAV = NULL;
		CD3D11RenderTarget* pRenderTarget = nullptr;
		std::vector<CD3D11RenderTarget*> renderTargets;
		std::vector<CD3D11DepthStencilSurface*> depthStencilSurfaces;

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = miplevel;
		texDesc.ArraySize = arraySize;
		texDesc.Format = getDxgiFormat(format);
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.BindFlags = getD3dx11BindFlags(bindFlags);
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.Usage = getD3d11Usage(memoryUsage);

		if (rawData)
		{
			if (memoryUsage == EMU_UNKNOWN)
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
			if (memoryUsage == EMU_UNKNOWN)
				texDesc.Usage = D3D11_USAGE_DEFAULT;
			hr = md3dDevice->CreateTexture2D(&texDesc, NULL, &pd3dTexture);
		}


		if (FAILED(hr))
			return false;

		if (bindFlags & ETBT_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = getDxgiSRVFormat(format);
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = miplevel;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.ArraySize = arraySize;

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
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.MipSlice = 0;
			uavDesc.Texture2DArray.ArraySize = arraySize;
			uavDesc.Texture2DArray.FirstArraySlice = 0;

			hr = md3dDevice->CreateUnorderedAccessView(pd3dTexture, &uavDesc, &pd3dUAV);
			if (FAILED(hr))
			{
				ReleaseCOM(pd3dTexture);
				return false;
			}
		}

		
		if (bindFlags & ETBT_RENDER_TARGET)
		{
			for (u32 i = 0; i < arraySize; i++)
			{
				CD3D11RenderTarget* pRenderTarget = new CD3D11RenderTarget(md3dDevice, md3dDeviceContext);
				renderTargets.push_back(pRenderTarget);
				if (!pRenderTarget->createOneInArray(this, pd3dTexture, pd3dSRV, i, width, height, format))
				{
					clearRenderTargets(renderTargets);
					ReleaseCOM(pd3dSRV);
					ReleaseCOM(pd3dTexture);
					return false;
				}
			}
		}

		if (bindFlags & ETBT_DEPTH_STENCIL)
		{
			for (u32 i = 0; i < arraySize; i++)
			{
				CD3D11DepthStencilSurface* pDS = new CD3D11DepthStencilSurface(md3dDevice, md3dDeviceContext);
				depthStencilSurfaces.push_back(pDS);
				if (!pDS->createOneInArray(this, pd3dTexture, pd3dSRV, i, width, height, format))
				{
					clearDepthStencilSurfaces(depthStencilSurfaces);
					clearRenderTargets(renderTargets);
					ReleaseCOM(pd3dSRV);
					ReleaseCOM(pd3dTexture);
					return false;
				}
			}
		}

		clearRenderTargets(mRenderTargets);
		clearDepthStencilSurfaces(mDepthStencilSurfaces);
		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dUAV);
		ReleaseCOM(md3dTexture);

		md3dTexture = pd3dTexture;
		md3dSRV = pd3dSRV;
		md3dUAV = pd3dUAV;
		mRenderTargets = renderTargets;
		mDepthStencilSurfaces = depthStencilSurfaces;

		mTextureWidth = width;
		mTextureHeight = height;
		mFormat = format;

		return true;
	}

	void CD3D11Texture2DArray::clearRenderTargets(std::vector<CD3D11RenderTarget*> renderTargets)
	{
		for (u32 i = 0; i < renderTargets.size(); i++)
		{
			ReleaseReferenceCounted(renderTargets[i]);
		}
	}

	void CD3D11Texture2DArray::clearDepthStencilSurfaces(std::vector<CD3D11DepthStencilSurface*> depthStencilSurfaces)
	{
		for (u32 i = 0; i < depthStencilSurfaces.size(); i++)
		{
			ReleaseReferenceCounted(depthStencilSurfaces[i]);
		}
	}

	u32 CD3D11Texture2DArray::getElementSize() const
	{
		return getFormatOffset(mFormat);
	}


	

}

