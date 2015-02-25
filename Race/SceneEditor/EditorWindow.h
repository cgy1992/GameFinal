#ifndef __EDITOR_WINDOW_CLASS_H__
#define __EDITOR_WINDOW_CLASS_H__

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
	
private:

	static EditorWindow*		_instance;
	u32							mBufferWndWidth;
	u32							mBufferWndHeight;

	HWND						mHwnd;
	HWND						mNewButton;
	HWND						mCancelButton;
	HWND						mMeshNamesComboBox;

	E_MOUSE_STATE				mMouseState;
	
};


#endif

