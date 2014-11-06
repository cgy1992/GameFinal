#ifndef __MATERIAL_MANAGER_CLASS_H__
#define __MATERIAL_MANAGER_CLASS_H__

#include "IMaterialManager.h"
#include "CSortCodeAllocator.h"

namespace gf
{
	class CMaterialManager : public IMaterialManager
	{
	public:
		CMaterialManager();
		virtual ~CMaterialManager();

		virtual bool add(IMaterial& material);

		virtual IMaterial* get(const std::string& name, bool bLoadIfNotExist = true);

		virtual IMaterial* create(const std::string& name,
			IPipeline* pipelines[]);

		virtual IMaterial* create(const std::string& name,
			IPipeline* pipeline);

		virtual bool destroy(const std::string& name);

		virtual bool destroy(IMaterial* material);


	private:
		std::map<std::string, SMaterial>	mMaterialsMap;
		CSortCodeAllocator<255>				mCodeAllocator;
	};
}

#endif
