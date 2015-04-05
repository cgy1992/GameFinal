#pragma once
#include "CBoundingWindow.h"

class CBoxBoundingWindow : public CBoundingWindow
{
public:
	//virtual void Init();
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	//virtual void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam);

	virtual bool Update(SBoundingShape* shape);
	virtual void ShowInfo(SBoundingShape* shape);

private:
	HWND		mSizeTextFields[3];

};

