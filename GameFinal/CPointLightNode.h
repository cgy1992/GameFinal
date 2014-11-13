#ifndef __POINT_LIGHT_NODE_CLASS_H__
#define __POINT_LIGHT_NODE_CLASS_H__

#include "ILightNode.h"
#include "ITextureCube.h"
#include "IDevice.h"

namespace gf
{
	class CPointLightNode : public ILightNode
	{
	public:
		CPointLightNode(ISceneNode* parent, ISceneManager* smgr,
			bool bStatic, u32 id,
			const XMFLOAT3& position, f32 range)
			:ILightNode(parent, smgr, bStatic, id, position)
			, mShadowMapTexture(nullptr)
		{
			ZeroMemory(&mLightData, sizeof(mLightData));
			mLightData.Position = getAbsolutePosition();
			mLightData.Range = range;
			mLightData.Attenuations = XMFLOAT3(1.0f, 0, 0);

			mShadowMapWidth = IDevice::getInstance()->getClientWidth();
			mShadowMapHeight = mShadowMapWidth;
		}

		void render(E_PIPELINE_USAGE usage){}

		virtual E_LIGHT_TYPE getType() const
		{
			return ELT_POINT;
		}

		virtual void setDiffuse(const XMFLOAT4& diffuse)
		{
			mLightData.Diffuse = diffuse;
		}

		virtual XMFLOAT4 getDiffuse() const
		{
			return mLightData.Diffuse;
		}

		virtual void setSpecular(const XMFLOAT4& specular)
		{
			mLightData.Specular = specular;
		}

		virtual XMFLOAT4 getSpecular() const
		{
			return mLightData.Specular;
		}

		virtual void setRange(f32 range)
		{
			mLightData.Range = range;
		}

		virtual f32 getRange() const
		{
			return mLightData.Range;
		}


		virtual void setAttenuation(f32 a0, f32 a1, f32 a2)
		{
			mLightData.Attenuations = XMFLOAT3(a0, a1, a2);
		}

		virtual XMFLOAT3 getAttenuation() const
		{
			return mLightData.Attenuations;
		}

		virtual void setDirection(const XMFLOAT3& direction) {}

		virtual XMFLOAT4 getDirection() const { return XMFLOAT4(0, 0, 0, 0); }

		virtual void setFalloff(f32 falloff) {}

		virtual f32 getFalloff() const { return 0; }

		virtual void setTheta(f32 theta) {}

		virtual f32 getTheta() const { return 0; }

		virtual bool isCulled(const math::SFrustum& frustum) const;

		virtual bool intersect(const XNA::OrientedBox& obb) const;
		
		virtual math::SAxisAlignedBox getAabb() const
		{
			return mAabb;
		}

		virtual void OnRegisterSceneNode(bool bRecursion);

		virtual void getLightData(void* data) const;

		virtual void update(u32 delta = 0);

		virtual ITexture* getShadowMap();

		virtual void generateShadowMap(ICameraNode* viewCamera);

	private:

		void computeBoundingVolumn();

		SPointLight				mLightData;
		math::SSphere			mSphere;
		math::SAxisAlignedBox	mAabb;

		ITextureCube*			mShadowMapTexture;

	};
}

#endif

