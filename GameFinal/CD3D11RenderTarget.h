#ifndef __D3D11_RENDER_TARGET_CLASS_H__
#define __D3D11_RENDER_TARGET_CLASS_H__

#include "IRenderTarget.h"
#include "gfGIFormat.h"
#include "D3DUtil.h"

namespace gf
{
	class CD3D11RenderTarget : public IRenderTarget
	{
	public:
		CD3D11RenderTarget(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, const std::string& name, u32 sortcode)
			:IRenderTarget(name, sortcode)
			, md3dDevice(pd3dDevice)
			, md3dDeviceContext(pd3dDeviceContext)
			, md3dTexture(nullptr)
			, md3dShaderResourceView(nullptr)
			, md3dRenderTargetView(nullptr)
			, mTextureWidth(0)
			, mTextureHeight(0)
		{
			
		}

		bool create(u32 width, u32 height, E_GI_FORMAT format, bool multiSampling, u32 multiSamplingCount, u32 multiSamplingQuality);


		ID3D11ShaderResourceView* getShaderResourceView()
		{
			return md3dShaderResourceView;
		}
		
		ID3D11RenderTargetView* getRenderTargetView()
		{
			return md3dRenderTargetView;
		}

		virtual void clear(const f32 color[]);

		virtual ~CD3D11RenderTarget()
		{
			ReleaseCOM(md3dTexture);
			ReleaseCOM(md3dShaderResourceView);
			ReleaseCOM(md3dRenderTargetView);
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
		ID3D11RenderTargetView*			md3dRenderTargetView;

		u32								mTextureWidth;
		u32								mTextureHeight;
	};
}


#endif
