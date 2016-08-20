#ifndef __BASE_APP_CLASS_H__
#define __BASE_APP_CLASS_H__

#include "IApplication.h"

namespace gf
{
	class IWindow;
	class IInputDriverFactory;
	class IVideoDriverFactory;

	class CBaseApp : public IApplication
	{
	public:
		CBaseApp(const SAppSettings& params);

		virtual void init();

		virtual ~CBaseApp();

		virtual void run();

		virtual void setWindowTitle(const std::string& caption);

		virtual ISceneManager* createSceneManager(const math::SAxisAlignedBox& aabb);

		virtual ISceneManager* createSceneManager();

		virtual void clientToScreen(int& x, int& y) const;

		virtual void screenToClient(int& x, int& y) const;

		virtual void getMousePos(int& x, int& y) const;

		virtual bool async(AsyncFunction cb);

		virtual bool async(AsyncFunctionList& list,
			AsyncFunction finishCallback);

	protected:
		
		IInputDriverFactory*			mInputDriverFactory;
		IVideoDriverFactory*			mVideoDriverFactory;
	};
}


#endif

