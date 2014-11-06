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

	void CD3D11TextureCube::apply(E_SHADER_TYPE shaderType, u32 slot)
	{
		if (md3dShaderResourceView)
			md3d11Driver->setTexture(shaderType, slot, md3dShaderResourceView);
	}
}

