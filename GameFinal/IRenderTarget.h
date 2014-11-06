#ifndef __RENDER_TARGET_INTERFACE_H__
#define __RENDER_TARGET_INTERFACE_H__

#include "ITexture.h"

namespace gf
{
	class IRenderTarget : public IReferenceCounted
	{
	public:
		IRenderTarget()
			:mTexture(nullptr)
			, mTemporary(false)
			, mWidth(0)
			, mHeight(0)
		{

		}

		virtual void clear(const f32 color[]) = 0;

		virtual void clear() = 0;

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot) = 0;

		u32 getWidth() const
		{
			return mWidth;
		}

		u32 getHeight() const
		{
			return mHeight;
		}

		bool isTemporary() const
		{
			return mTemporary;
		}

		void setTemporary(bool b)
		{
			mTemporary = b;
		}

		ITexture* getTexture()
		{
			return mTexture;
		}

		E_GI_FORMAT getFormat() const
		{
			return mTexture->getFormat();
		}

	protected:
		ITexture*				mTexture;
		u32						mWidth;
		u32						mHeight;
		bool					mTemporary;
	};

}


#endif
