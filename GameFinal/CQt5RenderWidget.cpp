#include "stdafx.h"
#include "CQt5RenderWidget.h"
#include "CQt5Window.h"

namespace gf
{
	void CQt5RenderWidget::paintEvent(QPaintEvent *event)
	{
		mWindow->step();
	}

}
