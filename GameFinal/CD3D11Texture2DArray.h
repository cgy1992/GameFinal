#ifndef __CD3D11TEXTURE_2D_ARRAY_H_CLASS__
#define __CD3D11TEXTURE_2D_ARRAY_H_CLASS__

#include "ITexture2DArray.h"
#include "CD3D11Driver.h"
#include "CD3D11RenderTarget.h"
#include "CD3D11DepthStencilSurface.h"

namespace gf
{
	class CD3D11Texture2DArray : public ITexture2DArray
	{
	public:
		CD3D11Texture2DArray(ID3D11Device* pd3dDevice,
			ID3D11DeviceContext* pd3dDeviceContext,
			const std::string& name, u32 sortCode,
			CD3D11Driver* d3d11Driver);

		bool loadFromFile(const std::string& filename);

		bool create(u32 width, u32 height, u32 arraySize, u32 bindFlags,
			void* rawData, u32 miplevel, 
			E_GI_FORMAT format, u32 pitch = 0,
			E_MEMORY_USAGE memoryUsage = EMU_UNKNOWN);

		/*
		bool createDepthStencilTexture(u32 width, u32 height,
			u32 depthBitNum, u32 stencilBitNum,
			bool multiSampling, u32 multiSamplingCount, u32 multiSamplingQuality,
			bool bShaderBound = true, bool bindDepthToShader = true);
			*/

		ID3D11ShaderResourceView* getShaderResourceView()
		{
			return md3dSRV;
		}

		virtual u32 getWidth() const
		{
			return mTextureWidth;
		}

		virtual u32 getHeight() const
		{
			return mTextureHeight;
		}

		virtual u32 getArraySize() const
		{
			return mArraySize;
		}

		virtual IRenderTarget* getRenderTarget(u32 index = 0)
		{
			if (index >= mRenderTargets.size())
				return nullptr;

			return mRenderTargets[index];
		}

		virtual IDepthStencilSurface* getDepthStencilSurface(u32 index = 0)
		{
			if (index >= mDepthStencilSurfaces.size())
				return nullptr;

			return mDepthStencilSurfaces[index];
		}

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot,
			E_TEXTURE_BIND_TYPE bindType = ETBT_SHADER_RESOURCE);

		void clearRenderTargets(std::vector<CD3D11RenderTarget*> renderTargets);

		void clearDepthStencilSurfaces(std::vector<CD3D11DepthStencilSurface*> depthStencilSurfaces);

		virtual u32 getElementSize() const;

		virtual bool copyDataToAnotherTexture(ITexture* dest);

		virtual bool lock(E_TEXTURE_LOCK_TYPE lockType, STextureData* texData, u32 index = 0);

		virtual void unlock();

		virtual ~CD3D11Texture2DArray();

	private:

		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;
		CD3D11Driver*					md3d11Driver;
		ID3D11Texture2D*				md3dTexture;
		ID3D11ShaderResourceView*		md3dSRV;
		ID3D11UnorderedAccessView*		md3dUAV;

		u32								mTextureWidth;
		u32								mTextureHeight;
		u32								mBindFlags;
		u32								mArraySize;

		std::vector<CD3D11RenderTarget*>				mRenderTargets;
		std::vector<CD3D11DepthStencilSurface*>			mDepthStencilSurfaces;


	};

}



#endif

