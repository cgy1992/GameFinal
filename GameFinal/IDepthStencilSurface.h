#ifndef __DEPTH_STENCIL_SURFACE_INTERFACE_H__
#define __DEPTH_STENCIL_SURFACE_INTERFACE_H__

#include "ITexture.h"

namespace gf
{
	class IDepthStencilSurface : public ITexture
	{
	public:
		
		IDepthStencilSurface(const std::string& name, u32 sortcode)
			:ITexture(name, sortcode)
		{

		}

		virtual E_TEXTURE_TYPE getType() const
		{
			return ETT_DEPTH_STENCIL;
		}

		virtual void clear(f32 depth = 1.0f, u8 stencil = 0) = 0;

		virtual void clearDepth(f32 depth = 1.0f) = 0;

		virtual void clearStencil(u8 stencil = 0) = 0;

	};
}

#endif
