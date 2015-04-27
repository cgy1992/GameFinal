#pragma once
#include "CBoundingWindow.h"

class CCylinderBoundingWindow : public CBoundingWindow
{
public:
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	virtual bool Update(SBoundingShape* shape);
	virtual void ShowInfo(SBoundingShape* shape);

private:
	HWND		 mHeightTextField;
	HWND		 mRadiusTextField;
};

