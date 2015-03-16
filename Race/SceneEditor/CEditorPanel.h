#ifndef __CEDITOR_PANEL_H__
#define __CEDITOR_PANEL_H__

class CEditorPanel
{
public:
	virtual void Init() {}
	virtual void OnCreate(HWND hwnd) = 0;
	virtual void SetActive(bool bActive) = 0;
	virtual void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnSize(int cxClient, int cyClient) = 0;
	virtual void MouseLeftButtonDown(int xPos, int yPos){}
	virtual void MouseRightButtonDown(int xPos, int yPos){}
	virtual void MouseRightButtonUp(int xPos, int yPos){}
	virtual void MouseMove(int xPos, int yPos){}
	virtual void MouseDoubleClicked(int xPos, int yPos){}
	virtual void ShowNodeInfo(u32 id){};
};

#endif