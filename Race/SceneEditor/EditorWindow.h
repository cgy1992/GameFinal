#ifndef __EDITOR_WINDOW_CLASS_H__
#define __EDITOR_WINDOW_CLASS_H__

#include "CCreateMeshNodeWindow.h"
#include "CListNodesWindow.h"
#include "CNodeInfoWindow.h"

using namespace gf;

enum E_MOUSE_STATE
{
	EMS_PICKING,
	EMS_ADD_OBJECT,
	EMS_ROTATION
};

enum E_MOUSE_BUTTON
{
	E_LEFT_MOUSE_BUTTON = 0,
	E_MIDDLE_MOUSE_BUTTON = 1,
	E_RIGHT_MOUSE_BUTTON = 2,
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
	void MouseRightButtonUp(int xPos, int yPos);
	void MouseMove(int xPos, int yPos);
	void MouseDoubleClicked(int xPos, int yPos);
	void OnNewMeshNodeButton(HWND hwnd);
	void OnSize(int cxClient, int cyClient);
	void OnCreate(HINSTANCE hInst, HWND hwnd);

	bool CheckMouseInScene(int xPos, int yPos);
	void SetMouseState(E_MOUSE_STATE state) { mMouseState = state; }
	E_MOUSE_STATE GetMouseState() { return mMouseState; }

	void ShowNodeInfo(u32 id);

private:

	static EditorWindow*		_instance;
	u32							mBufferWndWidth;
	u32							mBufferWndHeight;

	HWND						mHwnd;
	HWND						mCreatePanelSelector;
	HWND						mNodeNameTextField;

	E_MOUSE_STATE				mMouseState;
	CCreateMeshNodeWindow		mCreateMeshNodeWindow;
	CListNodesWindow			mListNodesWindow;
	CNodeInfoWindow				mNodeInfoWindow;

	bool						mMousePressed[3];
	POINT						mRightMousePressPoint;
	f32							mOrientationBeforeRotate;

	const static u32 LEFT_SUB_WINDOW_WIDTH;
	const static u32 RIGHT_SUB_WINDOW_WIDTH;
	const static u32 WINDOW_INITIAL_WIDTH;
	const static u32 WINDOW_INITIAL_HEIGHT;
};


#endif

