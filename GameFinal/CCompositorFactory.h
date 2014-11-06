#ifndef __COMPOSITOR_FACTORY_CLASS_H__
#define __COMPOSITOR_FACTORY_CLASS_H__

#include "ICompositorFactory.h"

namespace gf
{
	class CCompositorFactory : public ICompositorFactory
	{
	public:
		CCompositorFactory();

		virtual bool registerFactoryMethod(u32 id, std::function<ICompositor*(const SCompositorCreateParam&)>);

		virtual ICompositor* createCompositor(u32 id, const SCompositorCreateParam& param) const;

	private:

		std::map<u32, std::function<ICompositor*(const SCompositorCreateParam&)>> mFactoryMethods;

	};

}



#endif

