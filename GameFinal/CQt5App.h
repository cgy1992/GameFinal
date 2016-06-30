#ifndef __QT5_APP_CLASS_H__
#define __QT5_APP_CLASS_H__

#include "IApplication.h"
#include "IVideoDriver.h"
#include <qapplication.h>

namespace gf
{
	class CQt5RenderWidget;

	class CQt5App : public IApplication
	{
	public:
		CQt5App(const SAppSettings& params);

		~CQt5App();

		bool init();

		//virtual void setUpdateCallback(AppUpdateCallbackFunction cb);

		virtual void run();

		void step();

		virtual void setWindowCaption(const char* caption);

		virtual ISceneManager* createSceneManager(const math::SAxisAlignedBox& aabb);

		virtual ISceneManager* createSceneManager();

		virtual void clientToScreen(u32& x, u32& y) const;

		virtual void screenToClient(u32& x, u32& y) const;

	private:
		CQt5RenderWidget*			mRenderWidget;
		QApplication*				mQApplication;
	};
}


#endif