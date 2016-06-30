#ifndef D3D11DEIVCE_H
#define D3D11DEIVCE_H

#include "IApplication.h"
#include "IVideoDriver.h"
namespace gf
{

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	class CWin32App : public IApplication
	{
	public:
		CWin32App(const SAppSettings& params);

		HRESULT init();

		virtual void run();

		LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

		virtual void setWindowCaption(const char* caption);

		virtual ISceneManager* createSceneManager(const math::SAxisAlignedBox& aabb);

		virtual ISceneManager* createSceneManager();

		virtual void clientToScreen(u32& x, u32& y) const;

		virtual void screenToClient(u32& x, u32& y) const;

		virtual ~CWin32App();
	private:
		//HWND			mHwnd;
		HWND			m_hParentWnd;
		HWND			m_hFrameWnd;
		HWND			m_hBufferWnd;
		HINSTANCE		m_hInstance;
		WNDPROC			mDefinedWndProc;
	};
}
#endif