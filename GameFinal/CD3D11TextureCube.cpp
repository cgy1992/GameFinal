#include "stdafx.h"
#include "CD3D11TextureCube.h"
#include "gfUtil.h"
#include "D3DUtil.h"

namespace gf
{
	bool CD3D11TextureCube::loadFromFile(const std::string& filename)
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
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = texDesc.MipLevels;
		desc.TextureCube.MostDetailedMip = 0;

		hr = md3dDevice->CreateShaderResourceView(pd3dTexture, &desc, &pd3dSRV);
		if (FAILED(hr))
		{
			GF_PRINT_CONSOLE_INFO("create texture shader view (%s) failed.\n", filename.c_str());
			return false;
		}

		ReleaseCOM(md3dTexture);
		ReleaseCOM(md3dShaderResourceView);

		md3dTexture = pd3dTexture;
		md3dShaderResourceView = pd3dSRV;
		mFormat = getFormatFromDxgi(texDesc.Format);

		// update width and height
		mTextureWidth = texDesc.Width;

		return true;
	}


	bool CD3D11TextureCube::create(u32 size, u32 bindFlags, 
		void* rawData, u32 miplevel, E_GI_FORMAT format, u32 pitch/*=0*/)
	{
		HRESULT hr;
		ID3D11Texture2D* pd3dTexture = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;
		CD3D11RenderTarget* pRenderTargets[6] = { 0 };

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = size;
		texDesc.Height = size;
		texDesc.MipLevels = miplevel;
		texDesc.ArraySize = 6;
		texDesc.Format = getDxgiFormat(format);
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.BindFlags = getD3dx11BindFlags(bindFlags);
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		if (rawData)
		{
			texDesc.Usage = D3D11_USAGE_IMMUTABLE;
			
			D3D11_SUBRESOURCE_DATA texData;
			texData.pSysMem = rawData;
			if (pitch == 0)
				pitch = getFormatOffset(format) * size;

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
			return false;

		if (bindFlags & ETBT_SHADER)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = texDesc.Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MipLevels = texDesc.MipLevels;
			desc.TextureCube.MostDetailedMip = 0;

			hr = md3dDevice->CreateShaderResourceView(pd3dTexture, &desc, &pd3dSRV);
			if (FAILED(hr))
			{
				GF_PRINT_CONSOLE_INFO("create texture shader view failed.\n");
				ReleaseCOM(pd3dTexture);
				return false;
			}
		}

		if (bindFlags & ETBT_RENDER_TARGET)
		{
			for (u32 i = 0; i < 6; i++)
			{
				pRenderTargets[i] = new CD3D11RenderTarget(md3dDevice, md3dDeviceContext);
				if (!pRenderTargets[i]->createOneInArray(this, pd3dTexture, pd3dSRV, i, size, size, format))
				{
					clearRenderTargets(pRenderTargets);
					ReleaseCOM(pd3dSRV);
					ReleaseCOM(pd3dTexture);
					return false;
				}
			}
		}


		// if all the above is successful, then copy the temporary variables to the members.
		clearRenderTargets(mRenderTargets);
		ReleaseCOM(md3dShaderResourceView);
		ReleaseCOM(md3dTexture);

		md3dTexture = pd3dTexture;
		md3dShaderResourceView = pd3dSRV;
		for (u32 i = 0; i < 6; i++)
			mRenderTargets[i] = pRenderTargets[i];

		mTextureWidth = size;
		mFormat = format;

		return true;
	}



	void CD3D11TextureCube::apply(E_SHADER_TYPE shaderType, u32 slot)
	{
		if (md3dShaderResourceView)
			md3d11Driver->setTexture(shaderType, slot, md3dShaderResourceView);
	}

	void CD3D11TextureCube::clearRenderTargets(CD3D11RenderTarget* pRenderTargets[])
	{
		for (u32 i = 0; i < 6; i++)
		{
			ReleaseReferenceCounted(pRenderTargets[i]);
		}
	}

}

