#ifndef __COMPOSITOR_FACTORY_INTERFACE_H__
#define __COMPOSITOR_FACTORY_INTERFACE_H__

#include "ICompositor.h"
#include <functional>

namespace gf
{
	class ICompositorFactory
	{
	public:

		template<class T>
		bool registerCompositorClass(u32 id)
		{
			return registerFactoryMethod(id, [](const SCompositorCreateParam& param){
				return new T(param);
			});
		}

		virtual bool registerFactoryMethod(u32 id, std::function<ICompositor*(const SCompositorCreateParam&)>) = 0;

		virtual ICompositor* createCompositor(u32 id, const SCompositorCreateParam& param) const = 0;

	};
}






#endif

