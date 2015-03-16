#ifndef __INSTANCE_INFO_WINDOW_H__
#define __INSTANCE_INFO_WINDOW_H__

#include "CSubWindow.h"

class CInstanceInfoWindow : public CSubWindow
{
public:
	void Init();
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);
	void OnClickCreateButton();
	void UpdateListBoxItems(u32 collectionId);
	void AddListBoxItem(u32 id);

	void OnClickListItem();
	void OnDoubleClickListItem();
	void SelectListItem(u32 id);
	void OnClickDeleteButton();
	int GetSelectedItemId();
private:

	HWND			mNodeNameTextField;
	HWND			mCreateBtn;
	HWND			mInstancesListBox;
	HWND			mDeleteBtn;
};

#endif