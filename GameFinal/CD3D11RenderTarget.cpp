#include "stdafx.h"
#include "CD3D11RenderTarget.h"
#include "gfUtil.h"

namespace gf
{
	bool CD3D11RenderTarget::create(u32 width, u32 height, E_GI_FORMAT format, 
		bool multiSampling, u32 multiSamplingCount, u32 multiSamplingQuality)
	{
		ID3D11Texture2D* pd3dTexture = nullptr;
		ID3D11ShaderResourceView* pd3dShaderResourceView = nullptr;
		ID3D11RenderTargetView* pd3dRenderTargetView = nullptr;

		D3D11_TEXTURE2D_DESC textureDesc;
		HRESULT hr;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


		// Initialize the render target texture description.
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		if (multiSamplingCount > 1)
			multiSampling = true;
		
		if (!multiSampling)
		{
			multiSamplingCount = 1;
			multiSamplingQuality = 0;
		}

		// Setup the render target texture description.
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = getDxgiFormat(format);
		textureDesc.SampleDesc.Count = multiSamplingCount;
		textureDesc.SampleDesc.Quality = multiSamplingQuality;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// Create the render target texture.
		hr = md3dDevice->CreateTexture2D(&textureDesc, NULL, &pd3dTexture);
		if (FAILED(hr))
		{
			return false;
		}

		// Setup the description of the render target view.
		renderTargetViewDesc.Format = textureDesc.Format;

		//renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		//renderTargetViewDesc.Texture2D.MipSlice = 0;
		
		if (!multiSampling)
		{
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;
		}
		else
		{
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		

//		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
//		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// Create the render target view.
		hr = md3dDevice->CreateRenderTargetView(pd3dTexture, &renderTargetViewDesc, &pd3dRenderTargetView);
		if (FAILED(hr))
		{
			ReleaseCOM(pd3dTexture);
			return false;
		}

		// Setup the description of the shader resource view.

		shaderResourceViewDesc.Format = textureDesc.Format;

		//shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		//shaderResourceViewDesc.Texture2D.MipLevels = 1;

		
		if (!multiSampling)
		{
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
		}
		else
		{
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
		
		
	//	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	//	shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// Create the shader resource view.
		hr = md3dDevice->CreateShaderResourceView(pd3dTexture, &shaderResourceViewDesc, &pd3dShaderResourceView);
		if (FAILED(hr))
		{
			ReleaseCOM(pd3dTexture);
			ReleaseCOM(pd3dRenderTargetView);
			return false;
		}

		ReleaseCOM(md3dTexture);
		ReleaseCOM(md3dShaderResourceView);
		ReleaseCOM(md3dRenderTargetView);

		md3dTexture = pd3dTexture;
		md3dShaderResourceView = pd3dShaderResourceView;
		md3dRenderTargetView = pd3dRenderTargetView;

		// update width and height member
		mTextureWidth = width;
		mTextureHeight = height;

		return true;
	}

	void CD3D11RenderTarget::clear(const f32 color[])
	{
		if (md3dRenderTargetView)
			md3dDeviceContext->ClearRenderTargetView(md3dRenderTargetView, color);
	}

}



