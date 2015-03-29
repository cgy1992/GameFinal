#ifndef __EDITOR_WINDOW_H__
#define __EDITOR_WINDOW_H__

using namespace gf;

class CEditorWindow
{
public:
	CEditorWindow(u32 clientWidth, u32 clientHeight);
	static CEditorWindow* getInstance();
	static void _setInstance(CEditorWindow* instance);
	void init();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowsProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:

	static CEditorWindow*		_instance;

	u32							mBufferWndWidth;
	u32							mBufferWndHeight;

	HWND						mHwnd;
};

#endif