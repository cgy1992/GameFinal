#ifndef __LIST_NODES_WINDOW_CLASS_H__
#define __LIST_NODES_WINDOW_CLASS_H__

#include "CSubWindow.h"
class CListNodesWindow : public CSubWindow
{
public:

	void Init();
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);
	void AddListItem(u32 id);
	void OnClickListItem();
	void OnDoubleClickListItem();
	void SelectListItem(u32 id);
private:
	HWND				mNodesList;

};

#endif