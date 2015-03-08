#ifndef __NODE_INFO_WINDOW_CLASS_H__
#define __NODE_INFO_WINDOW_CLASS_H__
#include "CSubWindow.h"
#include "SNodeInfo.h"

class CNodeInfoWindow : public CSubWindow
{
public:
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	void UpdateShowing(SNodeInfo* info);
	void UpdateNodeInfo(SNodeInfo* info);
	void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);
	void OnClickUpdateTransformButton();
	void OnClickCancelTransformButton();
	void OnClickStaticCheckBox();
	void OnClickPhysicsCheckBox();
	void OnClickShadowCheckBox();

private:
	HWND	mPosTextFields[3];
	HWND	mRotTextFields[3];
	HWND	mScaleTextFields[3];
	HWND	mUpdateTransformBtn;
	HWND	mCancelTransformBtn;
	HWND	mStaticCheckBox;
	HWND	mPhysicsCheckBox;
	HWND	mShadowCheckBox;
};

#endif
