#ifndef __WINDOW_INTERFACE_H__
#define __WINDOW_INTERFACE_H__

#include "IReferenceCounted.h"

namespace gf
{
	struct SAppSettings;
	class IApplication;

	class IWindow : public IReferenceCounted
	{
	public:
		IWindow(IApplication* app) 
			:mApp(app)
		{
			
		}
		virtual bool init(SAppSettings& settings) = 0;
		virtual void loop(std::function<bool()>) = 0;
		virtual void setTitle(const std::string& s) = 0;
		virtual void clientToScreen(int& x, int& y) const = 0;
		virtual void screenToClient(int& x, int& y) const = 0;
		virtual void getMousePos(int& x, int& y) const = 0;

	protected:
		IApplication*			mApp;
	};
}

#endif