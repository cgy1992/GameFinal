#ifndef __D3D11_BUFFER_CLASS_H__
#define __D3D11_BUFFER_CLASS_H__

#include "IBuffer.h"
#include "CD3D11Driver.h"

namespace gf
{
	class CD3D11Buffer : public IBuffer
	{
	public:
		CD3D11Buffer(ID3D11Device* pd3dDevice,
			ID3D11DeviceContext* pd3dDeviceContext,
			const std::string& name, u32 sortCode,
			CD3D11Driver* d3d11Driver);

		bool create(u32 elementNum, 
			u32 bindFlags,
			E_GI_FORMAT format,
			u32 elementStride,
			void* rawData,
			E_MEMORY_USAGE memoryUsage = EMU_UNKNOWN);

		virtual u32 getElementSize() const;

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot,
			E_TEXTURE_BIND_TYPE bindType = ETBT_SHADER_RESOURCE);

		virtual bool getData(void* data);

		virtual bool setData(void* data, u32 elementCount);

		virtual bool copyDataToAnotherBuffer(IBuffer* anotherBuffer);

		~CD3D11Buffer();

	protected:
		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;
		CD3D11Driver*					md3d11Driver;

		ID3D11Buffer*					md3dBuffer;
		ID3D11ShaderResourceView*		md3dSRV;
		ID3D11UnorderedAccessView*		md3dUAV;

		u32								mTextureWidth;
		u32								mBindFlags;
	};
}


#endif