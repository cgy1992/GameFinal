#ifndef __E_GI_FORMAT_H_INCLUDE__
#define __E_GI_FORMAT_H_INCLUDE__

#include "gfTypes.h"

namespace gf
{
	enum E_GI_FORMAT
	{
		EGF_UNKNOWN = 0,
		EGF_R32G32B32A32_FLOAT,
		EGF_R32G32B32_FLOAT,
		EGF_R32G32_FLOAT,
		EGF_R32_FLOAT,
		EGF_R16_FLOAT,
		EGF_R8_UINT,
		EGF_R8G8_UINT,
		EGF_R8G8B8_UINT,
		EGF_R8G8B8A8_UINT,
		EGF_R8G8B8A8_UNORM,
		//EGF_D32_FLOAT,
		//EGF_D24_UNORM_S8_UINT,
		//EGF_D16_UNORM,

	};

	inline u32 getFormatOffset(E_GI_FORMAT format)
	{
		switch (format)
		{
		case EGF_R32G32B32A32_FLOAT:		return 16;
		case EGF_R32G32B32_FLOAT:			return 12;
		case EGF_R32G32_FLOAT:				return 8;
		case EGF_R32_FLOAT:					return 4;
		case EGF_R8_UINT:					return 1;
		case EGF_R8G8_UINT:					return 2;
		case EGF_R8G8B8_UINT:				return 3;
		case EGF_R8G8B8A8_UINT:				return 4;
		case EGF_R16_FLOAT:					return 2;
		case EGF_R8G8B8A8_UNORM:			return 4;
		}

		return 0;
	}
}

#endif

