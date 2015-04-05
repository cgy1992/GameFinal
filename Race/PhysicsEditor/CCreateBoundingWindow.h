#pragma once
#include "CSubWindow.h"
#include "SMeshNodeInfo.h"
#include "CBoundingWindow.h"

class CCreateBoundingWindow : public CSubWindow
{
public:
	virtual void Init();
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	virtual void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);
	void OnClickCreateButton();
	void OnClickDeleteButton();
	void OnClickBoundingListItem();
	void ShowMeshInfo(SMeshNodeInfo* info);
	E_BOUNDING_CATEGORY GetSelectedBoundingCategory();
	void ShowBoundingShapeInfo(SBoundingShape* shape);
	void OnSize(int xPos, int yPos, int cxClient, int cyClient);
	void OnClickUpdateButton();

public:

	HWND						mBoundingCategoryComboBox;
	HWND						mCreateButton;
	HWND						mDeleteButton;
	HWND						mBoundingsList;
	HWND						mUpdateButton;
	HWND						mCenterTextFields[3];
	HWND						mRotTextFields[3];
	HWND						mMassTextField;
	HWND						mFrictionTextField;

	CBoundingWindow*			mBoundingWindows[BOUNDING_CATEGORY_COUNT];
	
};

