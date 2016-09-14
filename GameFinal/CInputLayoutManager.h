#ifndef __INPUT_LAYOUT_MANAGER_CLASS_H__
#define __INPUT_LAYOUT_MANAGER_CLASS_H__

#include "IInputLayoutManager.h"
#include "IResourceFactory.h"
#include "CSortCodeAllocator.h"
namespace gf
{
	const u32 MAX_INPUT_LAYOUT_SORT_CODE = 255;

	class CInputLayoutManager : public IInputLayoutManager
	{
	public:
		CInputLayoutManager(IResourceFactory* pResourceFactory)
			:mResourceFactory(pResourceFactory)
		{

		}

		virtual IInputLayout* create(
			const std::vector<SInputLayoutElement>& elements,
			IShader* shader);

		virtual IInputLayout* get(const std::vector<SInputLayoutElement>& elements);

		virtual IInputLayout* get(u32 vertexFormat);

		virtual IInputLayout* create(u32 vertexFormat, IShader* vertexShader);

		void destroyAll();

		//virtual bool destroy(IInputLayout* inputlayout);

		virtual ~CInputLayoutManager();

	private:
		u32 getHashCode(const std::vector<SInputLayoutElement>& elements) const
		{
			u32 h = 0;
			u32 elementCount = elements.size();
			for (u32 i = 0; i < elementCount; i++)
			{
				h = h * 37 + elements[i].hashCode();
			}
			return h;
		}

	private:
		IResourceFactory*									mResourceFactory;
		CSortCodeAllocator<MAX_INPUT_LAYOUT_SORT_CODE>		mCodeAllocator;
		std::unordered_map<u32, IInputLayout*>				mInputLayoutCache;

		//std::map<u32, std::list<IInputLayout*>>				mInputLayoutsCache;
	};
}
#endif
