#ifndef __BOUNDING_WINDOW_H__
#define __BOUNDING_WINDOW_H__

#include "SMeshNodeInfo.h"
#include "CSubWindow.h"

class CBoundingWindow : public CSubWindow
{
public:
	virtual bool Update(SBoundingShape* shape) = 0;
	virtual void ShowInfo(SBoundingShape* shape) = 0;
	virtual void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam){}

};



#endif


