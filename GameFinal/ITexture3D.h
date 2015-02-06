#ifndef __TEXTRUE_3D_INTERFACE_H__
#define __TEXTRUE_3D_INTERFACE_H__

#include "ITexture.h"

namespace gf
{
	class ITexture3D : public ITexture
	{
	public:
		ITexture3D(const std::string& name, u32 sortcode)
			:ITexture(name, sortcode)
		{

		}

		virtual E_TEXTURE_TYPE getType() const
		{
			return ETT_TEXTURE_3D;
		}

		virtual ~ITexture3D() {}

		virtual u32 getDepth() const = 0;

	};
}




#endif
