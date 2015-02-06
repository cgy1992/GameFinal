#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                          \
	HRESULT hr = (x);                                      \
if (FAILED(hr))                                         \
		{                                                      \
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#ifndef ReleaseCOM 
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }
#endif

//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#ifndef SafeDelete
#define SafeDelete(x) { delete x; x = 0; }
#endif

//---------------------------------------------------------------------------------------
// Utility classes.
//---------------------------------------------------------------------------------------

#ifndef __D3DUTIL_FUNCTION_H__
#define __D3DUTIL_FUNCTION_H__

#include "gfGIFormat.h"
#include "EMemoryUsage.h"
#include "gfEnums.h"

namespace gf
{
	DXGI_FORMAT getDxgiFormat(E_GI_FORMAT format);

	D3D11_USAGE getD3d11Usage(E_MEMORY_USAGE usage);

	E_GI_FORMAT getFormatFromDxgi(DXGI_FORMAT format);

	D3D11_COMPARISON_FUNC getD3d11ComparisonFunc(E_COMPARISON_FUNC func);

	UINT getD3dx11BindFlags(u32 flags);

	UINT getD3dx11CpuAccessFlag(u32 flags);

	DXGI_FORMAT getDxgiSRVFormat(E_GI_FORMAT format);

	DXGI_FORMAT getDxgiDSVFormat(E_GI_FORMAT format);

	DXGI_FORMAT getDxgiUAVFormat(E_GI_FORMAT format);

	D3D11_MAP getD3d11MapType(E_TEXTURE_LOCK_TYPE lockType);


}



#endif
