#ifndef __CREATE_LIGHT_NODE_WINDOW_H__
#define __CREATE_LIGHT_NODE_WINDOW_H__

#include "CSubWindow.h"

class CCreateLightNodeWindow : public CSubWindow
{
public:
	void Init();
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);

	void OnClickCreateButton();
	void AddListItem(u32 id);
	void OnClickListItem();
	void OnDoubleClickListItem();
	void SelectListItem(u32 id);
	void GetNodeName(char text[]);
	void OnClickDeleteButton();
	int GetSelectedItemId();

private:
	HWND				mNameTextField;
	HWND				mCreateBtn;
	HWND				mLightNodesList;
	HWND				mDeleteBtn;
};

#endif
