#ifndef __ITEXTURE_H_INCLUDE__
#define __ITEXTURE_H_INCLUDE__

#include "IReferenceCounted.h"

namespace gf
{
	enum E_TEXTURE_TYPE
	{
		ETT_TEXTURE_2D,
		ETT_RENDER_TARGET,
		ETT_DEPTH_STENCIL
	};


	class ITexture : public IReferenceCounted
	{
	public:
		ITexture(const std::string& name, u32 sortcode)
			:mName(name), mSortCode(sortcode){}

		const std::string& getName()
		{
			return mName;
		}

		u32 getSortCode() const
		{
			return mSortCode;
		}

		virtual u32 getWidth() const = 0;

		virtual u32 getHeight() const = 0;

		virtual E_TEXTURE_TYPE getType() const = 0;

		virtual ~ITexture() {}
	protected:
		std::string		mName;
		u32				mSortCode;
		


	};

}

#endif