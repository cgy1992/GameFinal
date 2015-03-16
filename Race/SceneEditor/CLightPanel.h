#ifndef __LIGHT_NODE_PANEL_H__
#define __LIGHT_NODE_PANEL_H__

#include "CEditorPanel.h"
#include "CCreateLightNodeWindow.h"

class CEditorPanel;

class CLightPanel : public CEditorPanel
{
public:
	virtual void OnCreate(HWND hwnd);
	virtual void SetActive(bool bActive);
	virtual void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);
	virtual void OnSize(int cxClient, int cyClient);

	virtual void MouseLeftButtonDown(int xPos, int yPos);
	virtual void MouseRightButtonDown(int xPos, int yPos);
	virtual void MouseRightButtonUp(int xPos, int yPos);
	virtual void MouseMove(int xPos, int yPos);
	virtual void MouseDoubleClicked(int xPos, int yPos);
	virtual void ShowNodeInfo(u32 id);
public:
	CCreateLightNodeWindow		mCreateLightNodeWindow;
};

#endif