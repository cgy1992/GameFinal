#ifndef __MATERIAL_STRUCT_H__
#define __MATERIAL_STRUCT_H__

#include "ITexture.h"
#include "IPipeline.h"
#include "SReferenceCounted.h"

#define MAX_TEXTURE_COUNT 8
#define MAX_PIPELINE_COUNT 8

namespace gf
{
	struct SMaterial : public SReferenceCounted
	{
	public:

		SMaterial(const std::string& name, u32 sortcode, IPipeline* pipelines[])
			:mName(name), mSortCode(sortcode)
		{
			memset(mTextures, 0, sizeof(mTextures));
			memset(mPipelines, 0, sizeof(mPipelines));
			for (u32 i = 0; i < EPU_COUNT; i++)
			{
				mPipelines[i] = pipelines[i];
				AddReferenceCounted(mPipelines[i]);
			}
		}

		SMaterial(IPipeline* pipelines[])
			:mName(""), mSortCode(0)
		{
			memset(mTextures, 0, sizeof(mTextures));
			memset(mPipelines, 0, sizeof(mPipelines));
			for (u32 i = 0; i < EPU_COUNT; i++)
			{
				mPipelines[i] = pipelines[i];
				AddReferenceCounted(mPipelines[i]);
			}
		}

		SMaterial(IPipeline* pipeline)
			:mName(""), mSortCode(0)
		{
			memset(mTextures, 0, sizeof(mTextures));
			memset(mPipelines, 0, sizeof(mPipelines));
			mPipelines[EPU_FORWARD] = pipeline;
			AddReferenceCounted(mPipelines[EPU_FORWARD]);
		}

		SMaterial()
			:mName(""), mSortCode(0)
		{
			memset(mTextures, 0, sizeof(mTextures));
			memset(mPipelines, 0, sizeof(mPipelines));
		}

		SMaterial(const SMaterial& material)
		{
			mName = material.mName;
			mSortCode = material.mSortCode;
			mAttributes = material.mAttributes;

			for (u32 i = 0; i < EPU_COUNT; i++)
			{
				mPipelines[i] = material.mPipelines[i];
				AddReferenceCounted(mPipelines[i]);
			}

			for (u32 i = 0; i < MAX_TEXTURE_COUNT; i++)
			{
				mTextures[i] = material.mTextures[i];
				AddReferenceCounted(mTextures[i]);
			}
		}

		void setPipeline(IPipeline* pipeline)
		{
			setPipeline(EPU_FORWARD, pipeline);
		}

		void setPipeline(u32 index, IPipeline* pipeline)
		{
			if (mPipelines[index] != pipeline)
			{
				ReleaseReferenceCounted(mPipelines[index]);
				mPipelines[index] = pipeline;
				AddReferenceCounted(mPipelines[index]);
			}
		}

		void setPipeline(E_PIPELINE_USAGE usage, IPipeline* pipeline)
		{
			if (mPipelines[usage] != pipeline)
			{
				ReleaseReferenceCounted(mPipelines[usage]);
				mPipelines[usage] = pipeline;
				AddReferenceCounted(mPipelines[usage]);
			}
		}

		~SMaterial()
		{
			for (u32 i = 0; i < MAX_TEXTURE_COUNT; i++)
				ReleaseReferenceCounted(mTextures[i]);

			for (u32 i = 0; i < EPU_COUNT; i++)
				ReleaseReferenceCounted(mPipelines[i]);
		}


		const std::string& getName() const
		{
			return mName;
		}

		IPipeline* getPipeline(E_PIPELINE_USAGE usage, bool bReturnClosestPipeline = true)
		{
			IPipeline* pipeline = mPipelines[usage];
			if (bReturnClosestPipeline)
			{
				//如果没有指定负责绘制Directional Light Shadow Map的pipeline,
				//则直接使用FORWORD的pipeline.
				if (usage == EPU_DIR_SHADOW_MAP && pipeline == nullptr)
				{
					pipeline = mPipelines[EPU_FORWARD];
				}
			}

			return pipeline;
		}

		IPipeline* getPipeline(u32 index)
		{
			return mPipelines[index];
		}

		bool setTexture(u32 layer, ITexture* texture)
		{
			if (layer >= MAX_TEXTURE_COUNT)
				return false;

			ReleaseReferenceCounted(mTextures[layer]);
			mTextures[layer] = texture;
			if (texture)
				mTextures[layer]->grab();

			return true;
		}

		ITexture* getTexture(u32 layer)
		{
			if (layer >= MAX_TEXTURE_COUNT)
				return nullptr;

			return mTextures[layer];
		}

		u32 getSortCode() const
		{
			return mSortCode;
		}

		void setAttribute(const std::string& name, f32 value)
		{
			mAttributes[name] = XMFLOAT4(value, value, value, value);
		}

		void setAttribute(const std::string& name, XMFLOAT2 value)
		{
			mAttributes[name] = XMFLOAT4(value.x, value.y, 0, 0);
		}

		void setAttribute(const std::string& name, XMFLOAT3 value)
		{
			mAttributes[name] = XMFLOAT4(value.x, value.y, value.z, 0);
		}

		void setAttribute(const std::string& name, XMFLOAT4 value)
		{
			mAttributes[name] = value;
		}

		XMFLOAT4 getAttribute(const std::string& name) const
		{
			auto it = mAttributes.find(name);
			if (it != mAttributes.end())
				return it->second;

			return XMFLOAT4(0, 0, 0, 0);
		}

		bool getAttribute(const std::string& name, XMFLOAT4& val) const
		{
			auto it = mAttributes.find(name);
			if (it != mAttributes.end())
			{
				val = it->second;
				return true;
			}
			return false;
		}

		void setName(const std::string& name)
		{
			mName = name;
		}

	//	const static u32 MAX_TEXTURE_COUNT;
	//	const static u32 MAX_PIPELINE_COUNT;
	private:
		std::string								mName;
		u32										mSortCode;
		IPipeline*								mPipelines[EPU_COUNT];
		ITexture*								mTextures[MAX_TEXTURE_COUNT];
		std::map<std::string, XMFLOAT4>			mAttributes;
	};


	typedef SMaterial IMaterial;

}

#endif