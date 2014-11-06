#include "stdafx.h"
#include "CCompositorFactory.h"
#include "CBlurCompositor.h"
#include "CBloomCompositor.h"
#include "CMotionBlurCompositor.h"

namespace gf
{
	bool CCompositorFactory::registerFactoryMethod(u32 id, std::function<ICompositor*(const SCompositorCreateParam&)> method)
	{
		auto it = mFactoryMethods.find(id);
		if (it != mFactoryMethods.end())
			return false;
			
		mFactoryMethods.insert(std::make_pair(id, method));

		return true;
	}

	ICompositor* CCompositorFactory::createCompositor(u32 id, const SCompositorCreateParam& param) const
	{
		auto it = mFactoryMethods.find(id);
		if (it == mFactoryMethods.end())
			return nullptr;

		return (it->second)(param);
	}

	CCompositorFactory::CCompositorFactory()
	{
		registerCompositorClass<CBlurCompositor>(ECT_BLUR);
		registerCompositorClass<CBloomCompositor>(ECT_BLOOM);
		registerCompositorClass<CMotionBlurCompositor>(ECT_MOTION_BLUR);

	}

}

