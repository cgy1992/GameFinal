#ifndef __MESH_NODE_PANEL_H__
#define __MESH_NODE_PANEL_H__

#include "CEditorPanel.h"
#include "CCreateMeshNodeWindow.h"
#include "CNodeInfoWindow.h"
#include "CInstanceInfoWindow.h"

class CMeshNodePanel : public CEditorPanel
{
public:
	virtual void Init();
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
	void AddCollectionNode(const char* meshName, int maxNum);
	void ShowNodeInfo(SNodeInfo* info);
public:
	CNodeInfoWindow				mNodeInfoWindow;
	CInstanceInfoWindow			mInstanceNodeWindow;
	CCreateMeshNodeWindow		mCreateMeshNodeWindow;

private:
	POINT						mRightMousePressPoint;
	f32							mOrientationBeforeRotate;
};

#endif