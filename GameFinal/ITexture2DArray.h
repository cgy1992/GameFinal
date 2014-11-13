#ifndef __TEXTURE2DARRAY_H_INTERFACE
#define __TEXTURE2DARRAY_H_INTERFACE

#include "ITexture.h"

namespace gf
{

	class ITexture2DArray : public ITexture
	{
	public:

		ITexture2DArray(const std::string& name, u32 sortcode)
			:ITexture(name, sortcode)
		{

		}

		virtual E_TEXTURE_TYPE getType() const
		{
			return ETT_TEXTURE_2D_ARRAY;
		}

		virtual ~ITexture2DArray() {}
	};

}


#endif

