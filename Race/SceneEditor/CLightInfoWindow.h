#ifndef __LIGHT_NODE_WINDOW_H__
#define __LIGHT_NODE_WINDOW_H__

#include "SNodeInfo.h"
#include "CSubWindow.h"

class CLightInfoWindow : public CSubWindow
{
public:
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	void UpdateShowing(SLightNodeInfo* info);
	//void UpdateNodeInfo(SLightNodeInfo* info);
	void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);
	void OnClickUpdateButton();
	void OnClickCancelButton();
	void OnClickStaticCheckBox();

private:
	HWND		mPosTextFields[3];
	HWND		mDiffTextFields[3];
	HWND		mSpecTextFields[3];
	HWND		mAttenTextFields[3];
	HWND		mRangeTextField;
	
	HWND		mUpdateBtn;
	HWND		mCancelBtn;
	HWND		mStaticCheckBox;
};

#endif