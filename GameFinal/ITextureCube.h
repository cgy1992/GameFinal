#ifndef __CUBETEXTURE_INTERFACE_H__
#define __CUBETEXTURE_INTERFACE_H__

#include "ITexture.h"

namespace gf
{
	class ITextureCube : public ITexture
	{
	public:
		ITextureCube(const std::string& name, u32 sortcode)
			:ITexture(name, sortcode)
		{

		}

		virtual E_TEXTURE_TYPE getType() const
		{
			return ETT_CUBE_TEXTURE;
		}

		virtual ~ITextureCube() {}
	};

}

#endif