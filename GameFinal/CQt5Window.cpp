#include "stdafx.h"
#include "CQt5Window.h"
#include "CQt5RenderWidget.h"

namespace gf
{
	CQt5Window::CQt5Window(IApplication* app)
		:IWindow(app)
		, mQApplication(nullptr)
		, mRenderWidget(nullptr)
		, mUpdateCallback(nullptr)
	{

	}

	bool CQt5Window::init(SAppSettings& settings)
	{
		if (settings.Window.FrameWindowPtr == nullptr) {
			char* argv = NULL;
			int argc = 0;
			mQApplication = new QApplication(argc, &argv);
			mRenderWidget = new CQt5RenderWidget(0, this);
			mRenderWidget->setFixedSize(settings.Window.Width, settings.Window.Height);

			settings.Window.FrameWindowHandle = mRenderWidget->winId();
			settings.Window.FrameWindowPtr = mRenderWidget;

		}
		else {
			QWidget* parent = (QWidget*)settings.Window.ParentWindowPtr;
			QWidget* frame = (QWidget*)settings.Window.FrameWindowPtr;
			if (parent == nullptr) {
				parent = frame;
			}

			mRenderWidget = new CQt5RenderWidget(parent, this);
			mRenderWidget->setFixedSize(settings.Window.Width, settings.Window.Height);

			settings.Window.FrameWindowHandle = frame->winId();
			settings.Window.ParentWindowPtr = parent;
			settings.Window.ParentHandle = parent->winId();
		}

		settings.Window.BackBufferWindowHandle = mRenderWidget->winId();
		settings.Window.BackBufferWindowPtr = mRenderWidget;
		return true;
	}

	void CQt5Window::loop(std::function<bool()> func)
	{
		mUpdateCallback = func;
		mRenderWidget->show();
		if (mQApplication) {
			mQApplication->exec();
		}
	}

	void CQt5Window::step()
	{
		if (mUpdateCallback) {
			if (!mUpdateCallback()) {
				mQApplication->quit();
			}
		}
	}

	void CQt5Window::setTitle(const std::string& s)
	{
		if (mQApplication) {
			mRenderWidget->setWindowTitle(QString(s.c_str()));
		}
	}

	void CQt5Window::clientToScreen(int& x, int& y) const
	{
		QPoint p(x, y);
		QPoint p2 = mRenderWidget->mapToGlobal(p);
		x = p2.x();
		y = p2.y();
	}

	void CQt5Window::screenToClient(int& x, int& y) const
	{
		QPoint p(x, y);
		QPoint p2 = mRenderWidget->mapFromGlobal(p);
		x = p2.x();
		y = p2.y();
	}

	void CQt5Window::getMousePos(int& x, int& y) const 
	{
		QPoint p = QCursor::pos();
		p = mRenderWidget->mapFromGlobal(p);
		x = p.x();
		y = p.y();
	}
}

