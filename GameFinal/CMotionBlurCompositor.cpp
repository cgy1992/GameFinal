#include "stdafx.h"
#include "CMotionBlurCompositor.h"
#include "ISceneManager.h"
#include "IPipelineManager.h"
#include "ITextureManager.h"

namespace gf
{


	CMotionBlurCompositor::CMotionBlurCompositor(const SCompositorCreateParam& param)
		: mFirstFrame(true)
	{
		mSampleNum = param.MotionBlur.SampleNum;
		IPipeline* pipeline = IPipelineManager::getInstance()->get("gf/motion_blur");
		mMaterial.setPipeline(pipeline);
		mMaterial.setAttribute("SampleNum", (f32)mSampleNum);
	}

	void CMotionBlurCompositor::render(ISceneManager* smgr, bool lastCompositor)
	{
		IVideoDriver* driver = smgr->getVideoDriver();
		ITextureManager* tmgr = ITextureManager::getInstance();

		IDepthStencilSurface* depthSurface = driver->getDepthStencilSurface();
		IRenderTarget* input = driver->getRenderTarget();
		IMeshNode* quad = smgr->getQuadNode();

		if (mFirstFrame)
		{
			mPrevViewProj = smgr->getActiveCameraNode()->getViewProjMatrix();
			mFirstFrame = false;
		}

		driver->setDepthStencilSurface(nullptr);
		if (lastCompositor)
		{
			driver->setDefaultRenderTarget();
		}
		else
		{
			IRenderTarget* output = tmgr->getTempRenderTarget(0, 0, EGF_R8G8B8A8_UNORM);
			driver->setRenderTarget(output);
		}
		quad->setMaterial(&mMaterial);
		mMaterial.setTexture(0, input->getTexture());
		mMaterial.setTexture(1, depthSurface->getTexture());
		mMaterial.getPipeline(EPU_FORWARD)->setMatrix(EST_PIXEL_SHADER, "gPrevViewProj", mPrevViewProj);

		smgr->draw(quad);

		tmgr->releaseTempRenderTarget(input);

		mPrevViewProj = smgr->getActiveCameraNode()->getViewProjMatrix();
	}



}
