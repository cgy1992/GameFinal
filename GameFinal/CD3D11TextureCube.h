#ifndef __D3D11_CUBETEXTURE_CLASS_H__
#define __D3D11_CUBETEXTURE_CLASS_H__

#include "ITextureCube.h"
#include "CD3D11Driver.h"
#include "CD3D11RenderTarget.h"

namespace gf
{
	class CD3D11TextureCube : public ITextureCube
	{
	public:
		CD3D11TextureCube(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, 
			const std::string& name, u32 sortcode, CD3D11Driver* d3d11Driver)
			:ITextureCube(name, sortcode)
			, md3dDevice(pd3dDevice)
			, md3dDeviceContext(pd3dDeviceContext)
			, md3dTexture(NULL)
			, md3dShaderResourceView(NULL)
			, mTextureWidth(0)
			, md3d11Driver(d3d11Driver)
		{
			memset(mRenderTargets, 0, sizeof(mRenderTargets));
		}

		virtual ~CD3D11TextureCube();

		bool loadFromFile(const std::string& filename);

		bool create(u32 size, u32 bindFlags,
			void* rawData, u32 miplevel, E_GI_FORMAT format, u32 pitch = 0,
			E_MEMORY_USAGE memoryUsage = EMU_UNKNOWN);
		
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
			if (index < 6)
				return mRenderTargets[index];

			return nullptr;
		}

		virtual IDepthStencilSurface* getDepthStencilSurface(u32 index = 0)
		{
			return nullptr;
		}

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot,
			E_TEXTURE_BIND_TYPE bindType = ETBT_SHADER_RESOURCE);

		virtual u32 getElementSize() const;

	private:

		void clearRenderTargets(CD3D11RenderTarget* pRenderTargets[]);

		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;
		CD3D11Driver*					md3d11Driver;
		ID3D11Texture2D*				md3dTexture;
		ID3D11ShaderResourceView*		md3dShaderResourceView;
		u32								mTextureWidth;
		CD3D11RenderTarget*				mRenderTargets[6];

	};

}


#endif