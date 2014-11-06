#include "stdafx.h"
#include "CBlurCompositor.h"
#include "IPipelineManager.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"


namespace gf
{
	CBlurCompositor::CBlurCompositor(const SCompositorCreateParam& param)
	{
		mPassCount = param.Blur.PassCount;
		mTexelDistance = param.Blur.TexelDistance;
		mBlurAlgorithm = param.Blur.Algorithm;
		//mOutputFormat = param.OutputFormat;

		IPipeline* pipeline = nullptr;
		IPipelineManager* pipelineManager = IPipelineManager::getInstance();

		switch (mBlurAlgorithm)
		{
		case SCompositorCreateParam::EBLUR_NEAR_POINTS_4:
			pipeline = pipelineManager->get("gf/blur4");
			break;
		case SCompositorCreateParam::EBLUR_NEAR_POINTS_8:
			pipeline = pipelineManager->get("gf/blur8");
			break;
		case SCompositorCreateParam::EBLUR_GAUSSIAN:
			pipeline = pipelineManager->get("gf/gauss_vertical_blur");
			break;
		}

		mMaterial.setPipeline(pipeline);
		mMaterial.setAttribute("BlurDistance", mTexelDistance);
	}

	void CBlurCompositor::renderQuad(IVideoDriver* driver, ISceneManager* smgr, IMeshNode* quad, IRenderTarget* input, IRenderTarget* output)
	{
		if (output)
			driver->setRenderTarget(output);
		else
			driver->setDefaultRenderTarget();
	
		mMaterial.setTexture(0, input->getTexture());
		smgr->draw(quad);
	}


	void CBlurCompositor::render(ISceneManager* smgr, bool lastCompositor)
	{
		switch (mBlurAlgorithm)
		{
		case SCompositorCreateParam::EBLUR_NEAR_POINTS_4:
		case SCompositorCreateParam::EBLUR_NEAR_POINTS_8:
			squareBlur(smgr, lastCompositor);
			break;
		case SCompositorCreateParam::EBLUR_GAUSSIAN:
			gaussBlur(smgr, lastCompositor);
			break;
		default:
			break;
		}
	}

	void CBlurCompositor::gaussBlur(ISceneManager* smgr, bool lastCompositor)
	{
		IVideoDriver* driver = smgr->getVideoDriver();
		ITextureManager* tmgr = ITextureManager::getInstance();

		IRenderTarget* input = driver->getRenderTarget();
		IRenderTarget* output = tmgr->getTempRenderTarget(input->getWidth(), input->getHeight(), input->getFormat());

		IMeshNode* quad = smgr->getQuadNode();
		quad->setMaterial(&mMaterial);

		IPipelineManager* pipelineManager = IPipelineManager::getInstance();
		IPipeline* vblurPipeline = pipelineManager->get("gf/gauss_vertical_blur");
		IPipeline* hblurPipeline = pipelineManager->get("gf/gauss_horizontal_blur");

		// render pass - 1
		for (u32 i = 0; i < mPassCount - 1; i++)
		{
			// vertical blur
			mMaterial.setPipeline(vblurPipeline);
			renderQuad(driver, smgr, quad, input, output);

			// horizontal blur
			mMaterial.setPipeline(hblurPipeline);
			renderQuad(driver, smgr, quad, output, input);
		}

		// render last pass
		mMaterial.setPipeline(vblurPipeline);
		renderQuad(driver, smgr, quad, input, output);

		mMaterial.setPipeline(hblurPipeline);
		if (lastCompositor)
		{
			renderQuad(driver, smgr, quad, output, nullptr);
			tmgr->releaseTempRenderTarget(output);
		}
		else
		{
			renderQuad(driver, smgr, quad, output, input);
		}

		tmgr->releaseTempRenderTarget(input);
	}

	void CBlurCompositor::squareBlur(ISceneManager* smgr, bool lastCompositor)
	{
		IVideoDriver* driver = smgr->getVideoDriver();
		ITextureManager* tmgr = ITextureManager::getInstance();

		IRenderTarget* input = driver->getRenderTarget();
		IRenderTarget* output = nullptr;

		IMeshNode* quad = smgr->getQuadNode();
		quad->setMaterial(&mMaterial);

		if (mPassCount == 1)
		{
			if (!lastCompositor)
				output = tmgr->getTempRenderTarget(input->getWidth(), input->getHeight(), input->getFormat());

			renderQuad(driver, smgr, quad, input, output);
			tmgr->releaseTempRenderTarget(input);
		}
		else
		{
			output = tmgr->getTempRenderTarget(input->getWidth(), input->getHeight(), input->getFormat());

			// render pass - 1 
			for (u32 i = 0; i < mPassCount - 1; i++)
			{
				renderQuad(driver, smgr, quad, input, output);
				std::swap(input, output);
			}

			// render the last pass
			if (!lastCompositor)
			{
				renderQuad(driver, smgr, quad, input, output);
			}
			else
			{
				renderQuad(driver, smgr, quad, input, nullptr);
				tmgr->releaseTempRenderTarget(output);
			}

			tmgr->releaseTempRenderTarget(input);
		}
	}

	bool CBlurCompositor::setIntAttribute(u32 type, s32 val)
	{
		if (type == (s32)ECA_PASS_COUNT)
		{
			if (val <= 0)
				return false;

			mPassCount = val;
			return true;
		}

		IPipeline* pipeline = nullptr;
		if (type == (s32)ECA_ALGORITHM)
		{
			switch (val)
			{
			case SCompositorCreateParam::EBLUR_NEAR_POINTS_4:
				mBlurAlgorithm = SCompositorCreateParam::EBLUR_NEAR_POINTS_4;
				pipeline = IPipelineManager::getInstance()->get("gf/blur4");
				mMaterial.setPipeline(pipeline);
				return true;
			case SCompositorCreateParam::EBLUR_NEAR_POINTS_8:
				mBlurAlgorithm = SCompositorCreateParam::EBLUR_NEAR_POINTS_8;
				pipeline = IPipelineManager::getInstance()->get("gf/blur8");
				mMaterial.setPipeline(pipeline);
				return true;
			case SCompositorCreateParam::EBLUR_GAUSSIAN:
				mBlurAlgorithm = SCompositorCreateParam::EBLUR_GAUSSIAN;
				mMaterial.setPipeline(nullptr);
				return true;
			default:
				return false;
			}
		}

		return false;
	}

	bool CBlurCompositor::setFloatAttribute(u32 type, f32 val)
	{
		if (type == (u32)ECA_BLUR_TEXEL_DISTANCE)
		{
			mTexelDistance = val;
			mMaterial.setAttribute("BlurDistance", mTexelDistance);
			return true;
		}

		return false;
	}

}

