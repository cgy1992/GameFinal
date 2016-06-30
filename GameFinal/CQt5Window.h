#ifndef __QT5_WINDOW_CLASS_H__
#define __QT5_WINDOW_CLASS_H__

#include "IWindow.h"
#include <qapplication.h>

namespace gf
{
	class CQt5RenderWidget;

	class CQt5Window : public IWindow
	{
	public:
		CQt5Window(IApplication* app);

		virtual bool init(SAppSettings& settings);

		virtual void loop(std::function<bool()>);

		void step();

		virtual void setTitle(const std::string& s);

		//virtual void setWindowTitle(const std::string& caption) {}

		virtual void clientToScreen(int& x, int& y) const;

		virtual void screenToClient(int& x, int& y) const;

		virtual void getMousePos(int& x, int& y) const;

	private:
		CQt5RenderWidget*			mRenderWidget;
		QApplication*				mQApplication;
		std::function<bool()>		mUpdateCallback;
	};
}

#endif
