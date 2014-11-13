#ifndef __ITEXTURE_H_INCLUDE__
#define __ITEXTURE_H_INCLUDE__

#include "IReferenceCounted.h"
#include "gfGIFormat.h"

namespace gf
{
	enum E_SHADER_TYPE;

	enum E_TEXTURE_TYPE
	{
		ETT_TEXTURE_2D,
		ETT_TEXTURE_CUBE,
		ETT_RENDER_TARGET,
		ETT_DEPTH_STENCIL,
		ETT_TEXTURE_3D,
		ETT_TEXTURE_2D_ARRAY,
	};

	class IRenderTarget;
	class IDepthStencilSurface;

	class ITexture : public IReferenceCounted
	{

	public:
		ITexture(const std::string& name, u32 sortcode)
			:mName(name), mSortCode(sortcode), mFormat(EGF_UNKNOWN)
		{}

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

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot) = 0;

		virtual IRenderTarget* getRenderTarget(u32 index = 0) = 0;

		virtual IDepthStencilSurface* getDepthStencilSurface(u32 index = 0) = 0;

		E_GI_FORMAT getFormat() const
		{
			return mFormat;
		}

		virtual ~ITexture() {}
	protected:
		std::string		mName;
		u32				mSortCode;
		E_GI_FORMAT		mFormat;
	};

}

#endif
