#ifndef __IMESH_H__
#define __IMESH_H__

#include "IReferenceCounted.h"
#include "EMemoryUsage.h"
#include "IMeshBuffer.h"
#include "gfMath.h"

namespace gf
{

	enum E_MESH_TYPE
	{
		EMT_SIMPLE_MESH = 0,
		EMT_MODEL_MESH,
		EMT_ANIMATE_MODEL_MESH,
		EMT_TERRAIN_MESH
	};

	class IMesh : public IReferenceCounted
	{
	public:
		IMesh(const std::string& name,
			u32 sortcode,
			const math::SAxisAlignedBox& aabb)
			:mName(name)
			, mSortCode(sortcode)
			, mAabb(aabb)
		{

		}

		virtual E_MESH_TYPE getType() const = 0;

		virtual ~IMesh(){}

		virtual const std::string& getName() const
		{
			return mName;
		}

		u32 getSortCode() const
		{
			return mSortCode;
		}

		const math::SAxisAlignedBox& getAabb() const
		{
			return mAabb;
		}

	protected:
		u32					mSortCode;
		const std::string	mName;
		math::SAxisAlignedBox		mAabb;

	};

}


#endif
