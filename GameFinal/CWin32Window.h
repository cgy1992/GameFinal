#ifndef __WIN32_WINDOW_CLASS_H__
#define __WIN32_WINDOW_CLASS_H__

#include "IWindow.h"
#include <windows.h>

namespace gf
{
	class CWin32Window : public IWindow
	{
	public:
		CWin32Window(IApplication* app);

		virtual bool init(SAppSettings& settings);

		LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

		virtual void loop(std::function<bool()>);

		virtual void setTitle(const std::string& s);

		//virtual void setWindowTitle(const std::string& caption) {}

		virtual void clientToScreen(int& x, int& y) const;

		virtual void screenToClient(int& x, int& y) const;

		virtual void getMousePos(int& x, int& y) const;

	private:
		HWND			m_hParentWnd;
		HWND			m_hFrameWnd;
		HWND			m_hBufferWnd;
		HINSTANCE		m_hInstance;
		WNDPROC			mDefinedWndProc;
	};
}




#endif