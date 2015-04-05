#pragma once

#include "CSubWindow.h"

class CMeshInfoWindow : public CSubWindow
{
public:
	virtual void Init();
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	virtual void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);

	void OnSelectMeshNamesComboBox();
	void OnClickSaveButton();

public:
	HWND						mMeshNamesComboBox;
	HWND						mMeshAabbCenterLabel;
	HWND						mMeshAabbExtendLabel;
	HWND						mSaveButton;

};

