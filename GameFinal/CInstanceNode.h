#ifndef __INSTANCE_NODE_CLASS_H__
#define __INSTANCE_NODE_CLASS_H__

#include "IInstanceNode.h"

namespace gf
{
	class CInstanceNode : public IInstanceNode
	{
	public:
		CInstanceNode(ISceneNode* parent,
			ISceneManager* smgr,
			IInstanceCollectionNode* owner,
			bool bStatic,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f));

		virtual ~CInstanceNode();
		
	//	virtual void addChild(ISceneNode* child) { }

		virtual bool setMaterial(u32 subset, IMaterial* material){ return false; }

		virtual bool setMaterial(IMaterial* material){ return false; }

		virtual bool setMaterialName(const std::string& name){ return false; }

		virtual bool setMaterialName(u32 subset, const std::string& name){ return false; }

		virtual IMaterial* getMaterial(u32 subset = 0);

		virtual void setData(void* data);

		virtual const void* getData() const;

		virtual u32 getInstanceDataSize() const;

		virtual void render(E_PIPELINE_USAGE usage) {}

		virtual void renderInstanced(E_PIPELINE_USAGE usage, u32 instanceCount, IMeshBuffer* instanceBuffer){}

		virtual void updateAbsoluteTransformation();

		virtual u32 getSubsetCount() const;

		virtual void OnRegisterSceneNode(bool bRecursion = true);

		virtual void registerToCollectionForRendering();

	private:
		u8*					mInstanceData;
	};
}


#endif

