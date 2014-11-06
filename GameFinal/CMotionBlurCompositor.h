#ifndef __MOTION_BLUR_CLASS_H__
#define __MOTION_BLUR_CLASS_H__

#include "ICompositor.h"

namespace gf
{
	class CMotionBlurCompositor : public ICompositor
	{
	public:
		CMotionBlurCompositor(const SCompositorCreateParam& param);

		virtual void render(ISceneManager* smgr, bool lastCompositor);

	private:
		SMaterial			mMaterial;
		XMFLOAT4X4			mPrevViewProj;
		bool				mFirstFrame;
		u32					mSampleNum;
	};
}




#endif

