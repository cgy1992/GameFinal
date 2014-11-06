#ifndef __COMPOSITOR_INTERFACE_H__
#define __COMPOSITOR_INTERFACE_H__

#include "IReferenceCounted.h"
#include "IMeshNode.h"
#include "IRenderTarget.h"

namespace gf
{
	class ISceneManager;
	class IVideoDriver;

	enum E_COMPOSITOR_TYPE
	{
		ECT_BLUR = 1,
		ECT_BLOOM,
		ECT_MOTION_BLUR
	};

	enum E_COMPOSITOR_ATTRIBUTE
	{
		ECA_PASS_COUNT = 1,
		ECA_BLUR_TEXEL_DISTANCE,
		ECA_ALGORITHM,
		ECA_BLUR_TEXTURE_WIDTH,
		ECA_BLUR_TEXTURE_HEIGHT,
		ECA_THRESHOLD
	};

	struct SCompositorCreateParam
	{
		enum E_BLUR_ALGORITHM
		{
			EBLUR_NEAR_POINTS_4,
			EBLUR_NEAR_POINTS_8,
			EBLUR_GAUSSIAN
		};

		union
		{
			struct _tagBlurParam
			{
				u32 PassCount;
				f32	TexelDistance;
				E_BLUR_ALGORITHM Algorithm;
			} Blur;

			struct _tagBloomParam
			{
				u32 BlurPassCount;
				f32 BrightnessThreshold;
				f32 BlurTexelDistance;
				u32	BlurTextureWidth;
				u32 BlurTextureHeight;
			} Bloom;
			
			struct _tagMotionBlurParam
			{
				u32 SampleNum;
			} MotionBlur;
		};
	};

	class ICompositor : public IReferenceCounted
	{

	public:
		ICompositor()
		{

		}

		virtual void render(ISceneManager* smgr, bool lastCompositor) = 0;

		virtual bool setIntAttribute(u32 type, s32 val) { return false; }
		
		virtual bool setFloatAttribute(u32 type, f32 val) { return false; }

		virtual bool setVectorAttribute(u32 type, XMFLOAT4 val) { return false; }
	};

}



#endif

