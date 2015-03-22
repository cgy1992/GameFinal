#ifndef __GLOBAL_SETTING_WINDOW_H__
#define __GLOBAL_SETTING_WINDOW_H__

#include "CSubWindow.h"

class CGlobalSettingWindow : public CSubWindow
{
public:
	void Init();
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	virtual void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);

	void UpdateShowing();
	void OnClickUpdateButton();
	void OnClickDeferredShadingCheckBox();
	void OnClickSaveButton();

private:
	HWND		mSaveBtn;

	HWND		mAmbientTextField;
	HWND		mDiffuseTextField;
	HWND		mSpecularTextField;
	HWND		mUpdateBtn;

	HWND		mDeferShadingCheckBox;

};

#endif
