#include "stdafx.h"
#include "CD3D11DepthStencilSurface.h"

namespace gf
{
	/*
	bool CD3D11DepthStencilSurface::create(u32 width, u32 height,
		u32 depthBitNum, u32 stencilBitNum,
		bool multiSampling, u32 multiSamplingCount, u32 multiSamplingQuality,
		bool bShaderBound, bool bindDepthToShader)
	{
		DXGI_FORMAT d3dTexFormat;
		DXGI_FORMAT d3dDepthStencilFormat;
		DXGI_FORMAT d3dShaderViewFormat;
		ID3D11Texture2D* pd3dTexture = nullptr;
		ID3D11ShaderResourceView* pd3dShaderResourceView = nullptr;
		ID3D11DepthStencilView* pd3dDepthStencilView = nullptr;
		HRESULT hr;

		// if the multiSamplingCount surplus 1, then must use SamplingTexture
		if (multiSamplingCount > 1)
			multiSampling = true;

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

		//create depth-stencil-view
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = 0;
		dsvDesc.Format = d3dDepthStencilFormat;
		if (multiSampling)
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}	

		hr = md3dDevice->CreateDepthStencilView(pd3dTexture, &dsvDesc, &pd3dDepthStencilView);
		if (FAILED(hr))
		{
			GF_PRINT_CONSOLE_INFO("The Depth-stencil-surface('%s') has failed to be created.Because of the failure of DepthStencilView creation.\n", mName.c_str());
			ReleaseCOM(pd3dTexture);
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

			hr = md3dDevice->CreateShaderResourceView(pd3dTexture, &srvDesc, &pd3dShaderResourceView);
			if (FAILED(hr))
			{
				GF_PRINT_CONSOLE_INFO("The Depth-stencil-surface('%s') has failed to be created.Because of the failure of ShaderResourceView creation.\n", mName.c_str());
				ReleaseCOM(pd3dTexture);
				ReleaseCOM(pd3dDepthStencilView);
				return false;
			}
		}
		else
		{
			pd3dShaderResourceView = nullptr;
		}

		ReleaseCOM(md3dTexture);
		ReleaseCOM(md3dShaderResourceView);
		ReleaseCOM(md3dDepthStencilView);

		md3dTexture = pd3dTexture;
		md3dShaderResourceView = pd3dShaderResourceView;
		md3dDepthStencilView = pd3dDepthStencilView;

		// update width and height member
		mTextureWidth = width;
		mTextureHeight = height;

		return true;
	}
	*/

	bool CD3D11DepthStencilSurface::create(ITexture* texture, 
		ID3D11Texture2D* pTexture2D,
		DXGI_FORMAT depthStencilFormat, 
		ID3D11ShaderResourceView* d3dShaderResourceView, 
		bool multiSampling)
	{
		HRESULT hr;
		ID3D11DepthStencilView* pd3dDepthStencilView;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = 0;
		dsvDesc.Format = depthStencilFormat;
		if (multiSampling)
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}

		hr = md3dDevice->CreateDepthStencilView(pTexture2D, &dsvDesc, &pd3dDepthStencilView);
		if (FAILED(hr))
		{
			return false;
		}

		ReleaseCOM(md3dDepthStencilView);
		md3dDepthStencilView = pd3dDepthStencilView;
		mTexture = texture;
		md3dShaderResourceView = d3dShaderResourceView;
		return true;
	}

	void CD3D11DepthStencilSurface::clear(f32 depth, u8 stencil)
	{
		if (md3dDepthStencilView)
			md3dDeviceContext->ClearDepthStencilView(md3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
	}

	void CD3D11DepthStencilSurface::clearDepth(f32 depth)
	{
		if (md3dDepthStencilView)
			md3dDeviceContext->ClearDepthStencilView(md3dDepthStencilView, D3D11_CLEAR_DEPTH, depth, 0);
	}

	void CD3D11DepthStencilSurface::clearStencil(u8 stencil)
	{
		if (md3dDepthStencilView)
			md3dDeviceContext->ClearDepthStencilView(md3dDepthStencilView, D3D11_CLEAR_STENCIL, 1.0f, stencil);
	}

	void CD3D11DepthStencilSurface::apply(E_SHADER_TYPE shaderType, u32 slot)
	{
		if (mTexture)
			mTexture->apply(shaderType, slot);
	}
}

