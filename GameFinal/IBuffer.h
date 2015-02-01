#ifndef __BUFFER_INTERFACE_H__
#define __BUFFER_INTERFACE_H__

#include "ITexture.h"

namespace gf
{
	class IBuffer : public ITexture
	{
	public:
		IBuffer(const std::string& name, u32 sortcode)
			:ITexture(name, sortcode)
			, mElementStride(0)
			, mElementNum(0)
		{
			
		}

		virtual E_TEXTURE_TYPE getType() const
		{
			return ETT_BUFFER;
		}

		virtual u32 getWidth() const
		{
			return mElementNum;
		}

		u32 getElementNum() const
		{
			return mElementNum;
		}

		virtual u32 getHeight() const
		{
			return 1;
		}

		virtual IRenderTarget* getRenderTarget(u32 index = 0) { return nullptr; }

		virtual IDepthStencilSurface* getDepthStencilSurface(u32 index = 0) { return nullptr; }

		virtual bool getData(void* data) = 0;

		virtual bool setData(void* data, u32 elementCount) = 0;

		virtual bool copyDataToAnotherBuffer(IBuffer* anotherBuffer) = 0;

		virtual ~IBuffer() {}

	protected:

		u32			mElementStride;
		u32			mElementNum;

	};


}


#endif