#ifndef __CD3D11TEXTURE_H_INCLUDE__
#define __CD3D11TEXTURE_H_INCLUDE__

#include "ITexture.h"
namespace gf
{
	class CD3D11Texture : public ITexture
	{
	public:
		CD3D11Texture(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, const std::string& name, u32 sortCode);

		bool loadFromFile(const std::string& filename);

		/*
		const std::string& name,
			u32 width,
			u32 height,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch
			*/

		bool create(u32 width, u32 height, void* rawData, u32 miplevel, E_GI_FORMAT format, u32 pitch = 0);

		ID3D11ShaderResourceView* getShaderResourceView()
		{
			return m_pd3dSRV;
		}

		virtual E_TEXTURE_TYPE getType() const
		{
			return ETT_TEXTURE_2D;
		}

		virtual u32 getWidth() const
		{
			return mTextureWidth;
		}

		virtual u32 getHeight() const
		{
			return mTextureHeight;
		}


		virtual ~CD3D11Texture();
	private:
		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;
		ID3D11Texture2D*				m_pd3dTexture;
		ID3D11ShaderResourceView*		m_pd3dSRV;

		u32								mTextureWidth;
		u32								mTextureHeight;

	};
}
#endif
