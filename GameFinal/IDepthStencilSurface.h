#ifndef __DEPTH_STENCIL_SURFACE_INTERFACE_H__
#define __DEPTH_STENCIL_SURFACE_INTERFACE_H__

#include "ITexture.h"

namespace gf
{
	class IDepthStencilSurface : public IReferenceCounted
	{
	public:
		IDepthStencilSurface()
			:mTexture(nullptr)
			, mTemporary(false)
			, mWidth(0)
			, mHeight(0)
			, mStencilBits(0)
			, mDepthBits(0)
		{

		}

		virtual void clear(f32 depth = 1.0f, u8 stencil = 0) = 0;

		virtual void clearDepth(f32 depth = 1.0f) = 0;

		virtual void clearStencil(u8 stencil = 0) = 0;

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot) = 0;

		u32 getWidth() const
		{
			return mWidth;
		}

		u32 getHeight() const
		{
			return mHeight;
		}

		E_GI_FORMAT getFormat() const
		{
			return mTexture->getFormat();
		}

		ITexture* getTexture()
		{
			return mTexture;
		}

		u32 getDepthBits() const
		{
			return mDepthBits;
		}

		u32 getStencilBits() const
		{
			return mStencilBits;
		}

		bool isTemporary() const
		{
			return mTemporary;
		}

		void setTemporary(bool b)
		{
			mTemporary = b;
		}

	protected:
		ITexture*				mTexture;
		u32						mWidth;
		u32						mHeight;
		u32						mDepthBits;
		u32						mStencilBits;
		bool					mTemporary;
	};
}

#endif
