#include "stdafx.h"
#include "CInputLayoutManager.h"
namespace gf
{
	IInputLayout* CInputLayoutManager::create(
		const std::vector<SInputLayoutElement>& elements,
		IShader* shader)
	{
		return nullptr;
		/*
		u32 hashcode = getHashCode(elements);

		auto it = mInputLayoutsCache.find(hashcode);

		if (it != mInputLayoutsCache.end())
		{
			std::list<IInputLayout*>& layoutList = it->second;
			auto layoutIt = layoutList.begin();
			for (; layoutIt != layoutList.end(); layoutIt++)
			{
				IInputLayout* layout = *layoutIt;
				if (layout->equals(elements))
				{
					return layout;
				}
			}

			//if no equal inputlayout in the list
			u32 sortcode = mCodeAllocator.allocate();
			IInputLayout* inputlayout = mResourceFactory->createInputLayout(sortcode, elements, shader, hashcode);
			if (!inputlayout)
				return nullptr;
			layoutList.push_back(inputlayout);
			return inputlayout;
		}

		// if no input layout with the same hashcode
		u32 sortcode = mCodeAllocator.allocate();
		IInputLayout* inputlayout = mResourceFactory->createInputLayout(sortcode, elements, shader, hashcode);
		if (!inputlayout)
			return nullptr;

		std::list<IInputLayout*> layoutList;
		layoutList.push_back(inputlayout);
		mInputLayoutsCache.insert(std::make_pair(hashcode, layoutList));
		return inputlayout;
		*/
	}

	IInputLayout* CInputLayoutManager::create(u32 vertexFormat, IShader* vertexShader)
	{
		auto it = mInputLayoutCache.find(vertexFormat);
		if (it != mInputLayoutCache.end()) {
			return it->second;
		}

		std::vector<SInputLayoutElement> elements;

		u32 slotOffsets[16];
		memset(slotOffsets, 0, sizeof(slotOffsets));
		std::map<u32, SInputLayoutElement> ets;
		ets[EVF_POSITION] = SInputLayoutElement(EGF_R32G32B32_FLOAT, 0, "POSITION");
		ets[EVF_COLOR] = SInputLayoutElement(EGF_R32G32B32_FLOAT, 0, "COLOR");
		ets[EVF_NORMAL] = SInputLayoutElement(EGF_R32G32B32_FLOAT, 0, "NORMAL");
		ets[EVF_TANGENT] = SInputLayoutElement(EGF_R32G32B32_FLOAT, 0, "TANGENT");
		ets[EVF_BINORMAL] = SInputLayoutElement(EGF_R32G32B32_FLOAT, 0, "BINORMAL");
		ets[EVF_TEXCOORD0] = SInputLayoutElement(EGF_R32G32_FLOAT, 0, "TEXCOORD", 0);
		ets[EVF_TEXCOORD1] = SInputLayoutElement(EGF_R32G32_FLOAT, 0, "TEXCOORD", 1);
		ets[EVF_TEXCOORD2] = SInputLayoutElement(EGF_R32G32_FLOAT, 0, "TEXCOORD", 2);
		ets[EVF_TEXCOORD3] = SInputLayoutElement(EGF_R32G32_FLOAT, 0, "TEXCOORD", 3);
		ets[EVF_TEXCOORD4] = SInputLayoutElement(EGF_R32G32_FLOAT, 0, "TEXCOORD", 4);
		ets[EVF_TEXCOORD5] = SInputLayoutElement(EGF_R32G32_FLOAT, 0, "TEXCOORD", 5);
		ets[EVF_TEXCOORD6] = SInputLayoutElement(EGF_R32G32_FLOAT, 0, "TEXCOORD", 6);
		ets[EVF_TEXCOORD7] = SInputLayoutElement(EGF_R32G32_FLOAT, 0, "TEXCOORD", 7);
		ets[EVF_SIZE] = SInputLayoutElement(EGF_R32G32_FLOAT, 0, "SIZE");

		for (u32 i = 0; i < 31; i++) {
			u32 vf = (1 << i);
			if (vertexFormat & vf) {
				auto it = ets.find(vf);
				if (it != ets.end()) {
					SInputLayoutElement e = it->second;
					e.Offset = slotOffsets[e.Slot];
					slotOffsets[e.Slot] += getFormatOffset(e.Format);
					elements.push_back(e);
				}
				else if (vf == EVF_INSTANCED) {
					SInputLayoutElement e(EGF_R32G32B32A32_FLOAT, 0, "WORLD", 0, 1, true, 1);
					for (u32 i = 0; i < 4; i++) {
						e.Offset = getFormatOffset(EGF_R32G32B32A32_FLOAT) * i;
						e.SemanticIndex = i;
						elements.push_back(e);
						slotOffsets[1] += 4 * getFormatOffset(EGF_R32G32B32A32_FLOAT);
					}
				}
			}
		}
		
		// if no input layout with the same hashcode
		u32 sortcode = mCodeAllocator.allocate();
		IInputLayout* inputlayout = mResourceFactory->createInputLayout(sortcode, elements, vertexShader, 0);
		if (!inputlayout)
			return nullptr;

		mInputLayoutCache.insert(std::make_pair(vertexFormat, inputlayout));
		return inputlayout;
	}

	IInputLayout* CInputLayoutManager::get(u32 vertexFormat)
	{
		auto it = mInputLayoutCache.find(vertexFormat);
		if (it != mInputLayoutCache.end())
			return it->second;
		return nullptr;
	}

	IInputLayout* CInputLayoutManager::get(const std::vector<SInputLayoutElement>& elements)
	{
		/*
		int hashcode = getHashCode(elements);
		auto it = mInputLayoutsCache.find(hashcode);

		// if have the input layout with the same hash code
		if (it != mInputLayoutsCache.end())
		{
			auto layoutIt = it->second.begin();
			for (; layoutIt != it->second.end(); layoutIt++)
			{
				IInputLayout* layout = *layoutIt;
				if (layout->equals(elements))
				{
					return layout;
				}
			}
		}
		*/
		return nullptr;
	}

	CInputLayoutManager::~CInputLayoutManager()
	{
		destroyAll();
	}

	void CInputLayoutManager::destroyAll()
	{
		/*
		for (auto it = mInputLayoutsCache.begin(); it != mInputLayoutsCache.end(); it++)
		{
			std::list<IInputLayout*>& inputLayouts = it->second;
			for (auto it2 = inputLayouts.begin(); it2 != inputLayouts.end(); it2++)
			{
				(*it2)->drop();
			}
		}
		*/

		for (auto it = mInputLayoutCache.begin(); it != mInputLayoutCache.end(); it++) {
			(it)->second->drop();
		}
	}



	
}