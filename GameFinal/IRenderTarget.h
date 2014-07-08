#ifndef __RENDER_TARGET_INTERFACE_H__
#define __RENDER_TARGET_INTERFACE_H__

#include "ITexture.h"

namespace gf
{
	class IRenderTarget : public ITexture
	{
	public:
		IRenderTarget(const std::string& name, u32 sortcode)
			:ITexture(name, sortcode)
		{

		}

		virtual E_TEXTURE_TYPE getType() const
		{
			return ETT_RENDER_TARGET;
		}

		virtual void clear(const f32 color[]) = 0;
	};

}


#endif
