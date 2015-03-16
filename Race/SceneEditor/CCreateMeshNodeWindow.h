#ifndef __CREATE_MESH_NODE_WINDOW
#define __CREATE_MESH_NODE_WINDOW

#include "CSubWindow.h"
#include "SCollectionNodeInfo.h"

class CCreateMeshNodeWindow : public CSubWindow
{
public:
	void Init();
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);
	void OnNewMeshNodeButton();
	//void OnSelectMesh();
	void UpdateNodeNameTextField();
	void OnClickMultipleCheckBox();
	void OnClickDeleteButton();

	void AddListItem(u32 id);
	void OnClickListItem();
	void OnDoubleClickListItem();
	void SelectListItem(u32 id);
	int GetSelectedItemId();
	SNodeInfo* GetSelectedItemNodeInfo();
private:
	HWND						mNodeNameTextField;
	HWND						mNewButton;
	HWND						mMeshNamesComboBox;
	HWND						mStaticCheckBox;
	HWND						mMutipleCheckBox;
	HWND						mMutipleNumTextField;
	HWND						mNodesList;
	HWND						mDeleteButton;
};

#endif