#ifndef __CREATE_MESH_NODE_WINDOW
#define __CREATE_MESH_NODE_WINDOW

class CreateMeshNodeWindow
{
public:
	void Init();
	void OnCreate(HWND frameHwnd, HWND parentHwnd);
	void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);
	void OnNewMeshNodeButton();
	void Enable(int id, BOOL enabled);
private:

	HWND						mFrameHwnd;
	HWND						mParentHwnd;
	HWND						mNewButton;
	HWND						mMeshNamesComboBox;

};

#endif