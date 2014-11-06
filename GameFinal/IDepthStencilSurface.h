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
		{

		}

		virtual void clear(f32 depth = 1.0f, u8 stencil = 0) = 0;

		virtual void clearDepth(f32 depth = 1.0f) = 0;

		virtual void clearStencil(u8 stencil = 0) = 0;

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot) = 0;

		ITexture* getTexture()
		{
			return mTexture;
		}

	protected:
		ITexture*			mTexture;
	};
}

#endif
