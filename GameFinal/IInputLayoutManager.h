#ifndef __IINPUTLAYOUTMANAGER_H__
#define __IINPUTLAYOUTMANAGER_H__

#include "gfTypes.h"
#include "IInputLayout.h"
#include "IShader.h"
#include <vector>
#include <map>
namespace gf
{
	class IInputLayoutManager : public IReferenceCounted
	{
	public:
		virtual IInputLayout* create(
			const std::vector<SInputLayoutElement>& elements,
			IShader* shader) = 0;

		virtual IInputLayout* get(const std::vector<SInputLayoutElement>& elements) = 0;

		_DECLARE_SINGLETON_INSTANCE(IInputLayoutManager);
	};
}

#endif