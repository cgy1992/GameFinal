#ifndef __BLUR_COMPOSITOR_CLASS_H__
#define __BLUR_COMPOSITOR_CLASS_H__

#include "ICompositor.h"

namespace gf
{
	class CBlurCompositor : public ICompositor
	{
	public:

		CBlurCompositor(const SCompositorCreateParam& param);

		virtual void renderQuad(IVideoDriver* driver, ISceneManager* smgr, 
			IMeshNode* quad, IRenderTarget* input, IRenderTarget* output);

		virtual void render(ISceneManager* smgr, bool lastCompositor);

		virtual bool setIntAttribute(u32 type, s32 val);

		virtual bool setFloatAttribute(u32 type, f32 val);

	private:

		virtual void gaussBlur(ISceneManager* smgr, bool lastCompositor);

		virtual void squareBlur(ISceneManager* smgr, bool lastCompositor);

		E_GI_FORMAT			mOutputFormat;
		u32					mPassCount;
		f32					mTexelDistance;
		SMaterial			mMaterial;

		SCompositorCreateParam::E_BLUR_ALGORITHM mBlurAlgorithm;

	};

}




#endif

