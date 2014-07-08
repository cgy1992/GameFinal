#ifndef __DEPTH_STENCIL_SURFACE_CLASS_H__
#define __DEPTH_STENCIL_SURFACE_CLASS_H__

#include "IDepthStencilSurface.h"
#include "gfGIFormat.h"
#include "gfUtil.h"
#include "D3DUtil.h"

namespace gf
{
	class CD3D11DepthStencilSurface : public IDepthStencilSurface
	{
	public:
		CD3D11DepthStencilSurface(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, const std::string& name, u32 sortcode)
			:IDepthStencilSurface(name, sortcode)
			, md3dDevice(pd3dDevice)
			, md3dDeviceContext(pd3dDeviceContext)
			, md3dTexture(nullptr)
			, md3dDepthStencilView(nullptr)
			, md3dShaderResourceView(nullptr)
			, mTextureWidth(0)
			, mTextureHeight(0)
		{

		}

		bool create(u32 width, u32 height,
			u32 depthBitNum, u32 stencilBitNum, 
			bool multiSampling, u32 multiSamplingCount, u32 multiSamplingQuality,
			bool bShaderBound, bool bindDepthToShader);


		ID3D11ShaderResourceView* getShaderResourceView()
		{
			return md3dShaderResourceView;
		}

		ID3D11DepthStencilView* getDepthStencilView()
		{
			return md3dDepthStencilView;
		}

		virtual void clear(f32 depth = 1.0f, u8 stencil = 0);

		virtual void clearDepth(f32 depth = 1.0f);

		virtual void clearStencil(u8 stencil = 0);

		virtual ~CD3D11DepthStencilSurface()
		{
			ReleaseCOM(md3dTexture);
			ReleaseCOM(md3dShaderResourceView);
			ReleaseCOM(md3dDepthStencilView);
		}

		virtual u32 getWidth() const
		{
			return mTextureWidth;
		}

		virtual u32 getHeight() const
		{
			return mTextureHeight;
		}

	private:
		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;
		ID3D11Texture2D*				md3dTexture;
		ID3D11ShaderResourceView*		md3dShaderResourceView;
		ID3D11DepthStencilView*			md3dDepthStencilView;
		u32								mTextureWidth;
		u32								mTextureHeight;

	};
}

#endif

