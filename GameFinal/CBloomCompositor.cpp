#include "stdafx.h"
#include "CBloomCompositor.h"
#include "ITextureManager.h"
#include "IPipelineManager.h"
#include "ISceneManager.h"

namespace gf
{


	CBloomCompositor::CBloomCompositor(const SCompositorCreateParam& param)
	{
		mBlurPassCount = param.Bloom.BlurPassCount;
		mBrightnessThreshold = param.Bloom.BrightnessThreshold;
		mBlurTexelDistance = param.Bloom.BlurTexelDistance;
		mBlurTextureWidth = param.Bloom.BlurTextureWidth;
		mBlurTextureHeight = param.Bloom.BlurTextureHeight;

		mMaterial.setAttribute("BlurDistance", mBlurTexelDistance);
		mMaterial.setAttribute("BrightThreshold", mBrightnessThreshold);
	}

	void CBloomCompositor::render(ISceneManager* smgr, bool lastCompositor)
	{
		// create another RT, then use threshold to retain the high-bright pixels
		IVideoDriver* driver = smgr->getVideoDriver();
		ITextureManager* tmgr = ITextureManager::getInstance();
		IPipelineManager* pmgr = IPipelineManager::getInstance();

		IRenderTarget* input = driver->getRenderTarget();
		IRenderTarget* output1 = tmgr->getTempRenderTarget(mBlurTextureWidth, mBlurTextureHeight, EGF_R8G8B8A8_UNORM);
		f32 blackColor[] = { 0, 0, 0, 0 };
		output1->clear(blackColor);

		IMeshNode* quad = smgr->getQuadNode();
		quad->setMaterial(&mMaterial);

		IPipeline* highBrightPipeline = pmgr->get("gf/bloom_retain_high_brightness");
		mMaterial.setPipeline(highBrightPipeline);
		
		driver->setDepthStencilSurface(nullptr);
		driver->setRenderTarget(output1);

		mMaterial.setTexture(0, input->getTexture());
		smgr->draw(quad);

		// output is now containing the high bright pixels.
		// Now blur this 'output1'
		blur(driver, smgr, quad, output1);

		// now create a new RT(or just render to final back buffer)
		// then add input and output1

		IPipeline* bloomPipeline = pmgr->get("gf/bloom");
		mMaterial.setPipeline(bloomPipeline);
		mMaterial.setTexture(0, input->getTexture());
		mMaterial.setTexture(1, output1->getTexture());
		if (lastCompositor)
		{
			driver->setDefaultRenderTargetAndDepthStencil();
		}
		else
		{
			IRenderTarget* output2 = tmgr->getTempRenderTarget(0, 0, EGF_R8G8B8A8_UNORM);
			driver->setRenderTarget(output2);
			driver->setDefaultDepthStencilSurface();
		}

		smgr->draw(quad);
		tmgr->releaseTempRenderTarget(input);
		tmgr->releaseTempRenderTarget(output1);
	}

	void CBloomCompositor::renderQuad(IVideoDriver* driver, ISceneManager* smgr, IMeshNode* quad, IRenderTarget* input, IRenderTarget* output)
	{
		driver->setRenderTarget(output);
		mMaterial.setTexture(0, input->getTexture());
		smgr->draw(quad);
	}

	void CBloomCompositor::blur(IVideoDriver* driver, ISceneManager* smgr, IMeshNode* quad, IRenderTarget* rt1)
	{
		// rt1 is containing the information.
		// we use rt2 as an temporary render target to do blurring.

		IPipelineManager* pipelineManager = IPipelineManager::getInstance();
		ITextureManager* tmgr = ITextureManager::getInstance();
		IPipeline* vblurPipeline = pipelineManager->get("gf/gauss_vertical_blur");
		IPipeline* hblurPipeline = pipelineManager->get("gf/gauss_horizontal_blur");
		IRenderTarget* rt2 = tmgr->getTempRenderTarget(mBlurTextureWidth / 2, mBlurTextureHeight / 2, EGF_R8G8B8A8_UNORM);
		rt2->clear();

		for (u32 i = 0; i < mBlurPassCount; i++)
		{
			// vertical blur
			mMaterial.setPipeline(vblurPipeline);
			renderQuad(driver, smgr, quad, rt1, rt2);

			// horizontal blur
			mMaterial.setPipeline(hblurPipeline);
			renderQuad(driver, smgr, quad, rt2, rt1);
		}

		// after doing blurring, rt1 contains the valid pixels.
		// so just release rt2.
		tmgr->releaseTempRenderTarget(rt2);
	}

	bool CBloomCompositor::setIntAttribute(u32 type, s32 val)
	{
		switch (type)
		{
		case ECA_PASS_COUNT:
			mBlurPassCount = max(0, val);
			return true;
		case ECA_BLUR_TEXTURE_WIDTH:
			mBlurTextureWidth = max(0, val);
			return true;
		case ECA_BLUR_TEXTURE_HEIGHT:
			mBlurTextureHeight = max(0, val);
			return true;
		}
		return false;
	}

	bool CBloomCompositor::setFloatAttribute(u32 type, f32 val)
	{
		switch (type)
		{
		case ECA_THRESHOLD:
			mBrightnessThreshold = val;
			mMaterial.setAttribute("BrightThreshold", mBrightnessThreshold);
			return true;

		case ECA_BLUR_TEXEL_DISTANCE:
			mBlurTexelDistance = val;
			mMaterial.setAttribute("BrightThreshold", mBrightnessThreshold);
			return true;
		}
		
		return false;
	}

}
