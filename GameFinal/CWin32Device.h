#ifndef D3D11DEIVCE_H
#define D3D11DEIVCE_H

#include "IDevice.h"
#include "IVideoDriver.h"
namespace gf
{

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	class CWin32Device : public IDevice
	{
	public:
		CWin32Device(SCreationParameters& params);

		HRESULT init();

		bool run();

		LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

		virtual void setWindowCaption(const char* caption);

		virtual ISceneManager* createSceneManager(const math::SAxisAlignedBox& aabb);

		virtual ISceneManager* createSceneManager();

		virtual void clientToScreen(u32& x, u32& y) const;

		virtual void screenToClient(u32& x, u32& y) const;

		virtual ~CWin32Device();
	private:
		HWND			mHwnd;
		HINSTANCE		m_hInstance;
		WNDPROC			mDefinedWndProc;
	};
}
#endif