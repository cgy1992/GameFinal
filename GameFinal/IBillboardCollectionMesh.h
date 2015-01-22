#ifndef __BILLBOARD_COLLECTION_MESH_INTERFACE_H__
#define __BILLBOARD_COLLECTION_MESH_INTERFACE_H__

#include "ISimpleMesh.h"

namespace gf
{
	struct SBillboard
	{
		XMFLOAT3 Position;
		XMFLOAT2 Texcoord;
		XMFLOAT2 Size;
		XMFLOAT4 Color;
	};

	class IBillboardCollectionMesh : public ISimpleMesh
	{
	public:

		class SBillboardIterator;

	public:
		IBillboardCollectionMesh(
			const std::string& name,
			u32 sortcode,
			const math::SAxisAlignedBox& aabb,
			u32 maxNum,
			bool alterable,
			const std::vector<SBillboard>& billboards)
			:ISimpleMesh(name, sortcode, aabb, EVF_POSITION | EVF_TEXCOORD0 | EVF_SIZE | EVF_COLOR)
			, mMaxNum(maxNum)
			, mAlterable(alterable)
			, mBillboards(billboards)
		{

		}

		virtual E_MESH_TYPE getType() const
		{
			return EMT_BILLBOARD_COLLECTION_MESH;
		}

		bool setData(const std::vector<SBillboard>& billboards)
		{
			if (!mAlterable)
				return false;

			if (billboards.size() > mMaxNum)
				return false;

			mBillboards = billboards;
			return true;
		}

		bool setData(const std::vector<SBillboard>&& billboards)
		{
			if (!mAlterable)
				return false;

			if (billboards.size() > mMaxNum)
				return false;

			mBillboards = billboards;
			return true;
		}

		SBillboardIterator getIterator()
		{
			return SBillboardIterator(mBillboards);
		}

		bool addBillboard(const SBillboard& billboard)
		{
			if (!mAlterable)
				return false;

			if (mBillboards.size() == mMaxNum)
				return false;

			mBillboards.push_back(billboard);
			return true;
		}

		bool deleteBillboard(SBillboardIterator& it)
		{
			if (!mAlterable)
				return false;

			mBillboards.erase(it.mVectorIterator);
			return true;
		}

		u32 getBillboardNum() const { return mBillboards.size(); }

		u32	getMaxBillboardNum() const { return  mMaxNum; }

		virtual bool submitUpdate() = 0;


	protected:
		std::vector<SBillboard>		mBillboards;
		u32							mMaxNum;
		bool						mAlterable;

	public:

		class SBillboardIterator
		{
			friend class IBillboardCollectionMesh;
		public:
			SBillboardIterator(std::vector<SBillboard>& billboards)
				:mBillboards(billboards)
			{
				mVectorIterator = mBillboards.begin();
			}

			bool next()
			{
				mVectorIterator++;
				return mVectorIterator != mBillboards.end();
			}

			operator bool() const
			{
				return mVectorIterator != mBillboards.end();
			}

			operator SBillboard&()
			{
				return *mVectorIterator;
			}

		private:
			std::vector<SBillboard>::iterator		mVectorIterator;
			std::vector<SBillboard>&				mBillboards;
		};
	};
}



#endif

