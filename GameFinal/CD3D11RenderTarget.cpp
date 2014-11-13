#include "stdafx.h"
#include "CD3D11RenderTarget.h"
#include "gfUtil.h"

namespace gf
{

	void CD3D11RenderTarget::clear(const f32 color[])
	{
		if (md3dRenderTargetView)
			md3dDeviceContext->ClearRenderTargetView(md3dRenderTargetView, color);
	}

	void CD3D11RenderTarget::clear()
	{
		if (md3dRenderTargetView)
		{
			f32 color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			md3dDeviceContext->ClearRenderTargetView(md3dRenderTargetView, color);
		}
	}

	bool CD3D11RenderTarget::create(ITexture* texture, ID3D11Texture2D* pTexture2D, 
		ID3D11ShaderResourceView* d3dShaderResourceView,
		u32 width, u32 height, E_GI_FORMAT format)
	{
		HRESULT hr;
		ID3D11RenderTargetView*	pd3dRenderTargetView = nullptr;

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		// Setup the description of the render target view.

		renderTargetViewDesc.Format = getDxgiSRVFormat(format);
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// Create the render target view.
		hr = md3dDevice->CreateRenderTargetView(pTexture2D, &renderTargetViewDesc, &pd3dRenderTargetView);
		if (FAILED(hr))
			return false;

		ReleaseCOM(md3dRenderTargetView);
		md3dRenderTargetView = pd3dRenderTargetView;
		mTexture = texture;
		md3dShaderResourceView = d3dShaderResourceView;
		mWidth = width;
		mHeight = height;

		return true;
	}

	void CD3D11RenderTarget::apply(E_SHADER_TYPE shaderType, u32 slot)
	{
		if (mTexture)
		{
			mTexture->apply(shaderType, slot);
		}
	}

	bool CD3D11RenderTarget::createOneInArray(ITexture* texture, 
		ID3D11Texture2D* pTextureArray, 
		ID3D11ShaderResourceView* d3dShaderResourceView, 
		u32 index, u32 width, u32 height, E_GI_FORMAT format)
	{
		HRESULT hr;
		ID3D11RenderTargetView*	pd3dRenderTargetView;

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = getDxgiSRVFormat(format);
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.MipSlice = 0;
		rtvDesc.Texture2DArray.ArraySize = 1;
		rtvDesc.Texture2DArray.FirstArraySlice = index;

		// Create the render target view.
		hr = md3dDevice->CreateRenderTargetView(pTextureArray, 
			&rtvDesc, &pd3dRenderTargetView);
		if (FAILED(hr))
			return false;

		ReleaseCOM(md3dRenderTargetView);
		md3dRenderTargetView = pd3dRenderTargetView;
		mTexture = texture;
		md3dShaderResourceView = d3dShaderResourceView;
		mWidth = width;
		mHeight = height;

		return true;
	}

}



