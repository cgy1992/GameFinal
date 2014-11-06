#ifndef __BLACK_WHITE_COMPOSITOR_CLASS_H__
#define __BLACK_WHITE_COMPOSITOR_CLASS_H__

#include "ICompositor.h"

namespace gf
{
	class CCompositor : public ICompositor
	{
	public:
		CCompositor(IPipeline* pipeline)
			:mPipeline(pipeline), mMaterial(pipeline)
		{

		}

		virtual void render(ISceneManager* smgr, bool lastCompositor);

	private:
		IPipeline*			mPipeline;
		SMaterial			mMaterial;
	};

}


#endif
