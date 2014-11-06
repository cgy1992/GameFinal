#ifndef __BLOOM_COMPOSITOR_CLASS_H__
#define __BLOOM_COMPOSITOR_CLASS_H__

#include "ICompositor.h"

namespace gf
{
	class CBloomCompositor : public ICompositor
	{
	public:
		CBloomCompositor(const SCompositorCreateParam& param);

		virtual void render(ISceneManager* smgr, bool lastCompositor);
		
		void renderQuad(IVideoDriver* driver, ISceneManager* smgr, IMeshNode* quad, IRenderTarget* input, IRenderTarget* output);


		virtual void blur(IVideoDriver* driver, ISceneManager* smgr, IMeshNode* quad, IRenderTarget* rt1);

		virtual bool setIntAttribute(u32 type, s32 val);

		virtual bool setFloatAttribute(u32 type, f32 val);

	private:
		SMaterial					mMaterial;

		u32							mBlurPassCount;
		f32							mBrightnessThreshold;
		f32							mBlurTexelDistance;

		u32							mBlurTextureWidth;
		u32							mBlurTextureHeight;
		

	};
}




#endif // !
