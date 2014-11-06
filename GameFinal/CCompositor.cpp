#include "stdafx.h"
#include "CCompositor.h"
#include "IVideoDriver.h"

namespace gf
{
	void CCompositor::render(ISceneManager* smgr, bool lastCompositor)
	{
		ITextureManager* tmgr = ITextureManager::getInstance();
		IVideoDriver* driver = smgr->getVideoDriver();

		IRenderTarget* preOutput = driver->getRenderTarget();
		const SViewport& viewport = driver->getViewport();

		if (!lastCompositor)
		{
			IRenderTarget* target = tmgr->getTempRenderTarget(0, 0, EGF_R8G8B8A8_UNORM);
			driver->setRenderTarget(target);
		}
		else
		{
			driver->setDefaultRenderTarget();
		}

		mMaterial.setTexture(0, preOutput->getTexture());
		IMeshNode* quadNode = smgr->getQuadNode();
		quadNode->setMaterial(&mMaterial);
		
		smgr->draw(quadNode);
		
		// preOutput can be released now.
		tmgr->releaseTempRenderTarget(preOutput);
	}

}
