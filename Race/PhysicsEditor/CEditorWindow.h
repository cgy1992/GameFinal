#ifndef __EDITOR_WINDOW_H__
#define __EDITOR_WINDOW_H__

using namespace gf;

#include "CMeshInfoWindow.h"
#include "CCreateBoundingWindow.h"

class CEditorWindow
{
public:
	CEditorWindow(u32 clientWidth, u32 clientHeight);
	static CEditorWindow* getInstance();
	static void _setInstance(CEditorWindow* instance);
	void init();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowsProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void OnSize(int cxClient, int cyClient);
	void OnCreate(HINSTANCE hInst, HWND hwnd);
	void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);

	void ShowMeshInfo(SMeshNodeInfo* info);
private:

	static CEditorWindow*		_instance;

	u32							mBufferWndWidth;
	u32							mBufferWndHeight;

	HWND						mHwnd;

	static const u32			RIGHT_SUB_WINDOW_WIDTH;

	CMeshInfoWindow				mMeshInfoWindow;
	CCreateBoundingWindow		mCreateBoundingWindow;

	
};

#endif

