#ifndef __CREATE_MESH_NODE_WINDOW
#define __CREATE_MESH_NODE_WINDOW

#include "CSubWindow.h"

class CCreateMeshNodeWindow : public CSubWindow
{
public:
	void Init();
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);
	void OnNewMeshNodeButton();
	void OnSelectMesh();
	//void getDefaultMeshNodeName(char name[]);


private:
	HWND						mNewButton;
	HWND						mMeshNamesComboBox;
	HWND						mStaticCheckBox;
	HWND						mMutipleCheckBox;
};

#endif