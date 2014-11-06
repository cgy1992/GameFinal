#ifndef __CD3D11_TEXTURE_3D_CLASS_H__
#define __CD3D11_TEXTURE_3D_CLASS_H__

#include "ITexture3D.h"
#include "CD3D11Driver.h"

namespace gf
{
	class CD3D11Texture3D : public ITexture3D
	{
	public:
		CD3D11Texture3D(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext,
			const std::string& name, u32 sortcode, CD3D11Driver* d3d11Driver)
			:ITexture3D(name, sortcode)
			, md3dDevice(pd3dDevice)
			, md3dDeviceContext(pd3dDeviceContext)
			, md3dShaderResourceView(NULL)
			, md3dTexture(NULL)
			, mTextureWidth(0)
			, mTextureHeight(0)
			, md3d11Driver(d3d11Driver)
		{

		}

		virtual ~CD3D11Texture3D();

		bool create(u32 width, u32 height,
			u32 depth, void* rawData,
			u32 miplevel, E_GI_FORMAT format, u32 pitch = 0,
			u32 slicePitch = 0);


		virtual u32 getWidth() const
		{
			return mTextureWidth;
		}

		virtual u32 getHeight() const
		{
			return mTextureWidth;
		}

		virtual IRenderTarget* getRenderTarget(u32 index = 0)
		{
			return nullptr;
		}

		virtual IDepthStencilSurface* getDepthStencilSurface(u32 index = 0)
		{
			return nullptr;
		}

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot);

	private:
		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;
		CD3D11Driver*					md3d11Driver;
		ID3D11Texture3D*				md3dTexture;
		ID3D11ShaderResourceView*		md3dShaderResourceView;
		u32								mTextureWidth;
		u32								mTextureHeight;
		u32								mTextureDepth;
	};

}


#endif

