#ifndef __EDITOR_WINDOW_CLASS_H__
#define __EDITOR_WINDOW_CLASS_H__

#include "CreateMeshNodeWindow.h"

using namespace gf;

enum E_MOUSE_STATE
{
	EMS_PICKING,
	EMS_ADD_OBJECT,
};

class EditorWindow
{
public:
	EditorWindow(u32 clientWidth, u32 clientHeight);
	static EditorWindow* getInstance();
	static void _setInstance(EditorWindow* instance);
	void init();
	void BeginAddObject();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowsProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void MouseLeftButtonDown(int xPos, int yPos);
	void MouseRightButtonDown(int xPos, int yPos);
	void MouseMove(int xPos, int yPos);
	void MouseDoubleClicked(int xPos, int yPos);
	void OnNewMeshNodeButton(HWND hwnd);
	void OnSize(int cxClient, int cyClient);

	void SetMouseState(E_MOUSE_STATE state) { mMouseState = state; }
	E_MOUSE_STATE GetMouseState() { return mMouseState; }
private:

	static EditorWindow*		_instance;
	u32							mBufferWndWidth;
	u32							mBufferWndHeight;

	HWND						mHwnd;
	HWND						mLeftSubWindow;
	HWND						mRightSubWindow;

	E_MOUSE_STATE				mMouseState;

	CreateMeshNodeWindow		mCreateMeshNodeWindow;
	

	const static u32 LEFT_SUB_WINDOW_WIDTH = 200;
	const static u32 RIGHT_SUB_WINDOW_WINDTH = 200;
};


#endif

