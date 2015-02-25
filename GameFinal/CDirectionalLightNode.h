#ifndef __DIRECTIONAL_LIGHT_NODE_CLASS_H__
#define __DIRECTIONAL_LIGHT_NODE_CLASS_H__

#include "ILightNode.h"
#include "ITextureManager.h"

namespace gf
{
	class CDirectionalLightNode : public ILightNode
	{
	public:
		CDirectionalLightNode(ISceneNode* parent, 
			ISceneManager* smgr, u32 id,
			const XMFLOAT3& direction);

		void render(E_PIPELINE_USAGE usage){}

		virtual E_LIGHT_TYPE getType() const
		{
			return ELT_DIRECTIONAL;
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

		virtual void setDirection(const XMFLOAT3& direction)
		{
			XMVECTOR dir = XMLoadFloat3(&direction);
			dir = XMVector3Normalize(dir);

			XMFLOAT3 normdir;
			XMStoreFloat3(&normdir, dir);

			mLightData.Direction = XMFLOAT4(normdir.x, normdir.y, normdir.z, 0);
		}

		virtual XMFLOAT4 getDirection() const
		{
			return mLightData.Direction;
		}

		virtual void setAttenuation(f32 a0, f32 a1, f32 a2) {}

		virtual XMFLOAT3 getAttenuation() const { return XMFLOAT3(0, 0, 0); }

		virtual void setRange(f32 range) {}

		virtual f32 getRange() const { return 0; }

		virtual void setFalloff(f32 falloff) {}

		virtual f32 getFalloff() const { return 0; }

		virtual void setTheta(f32 theta) {}

		virtual f32 getTheta() const { return 0; }

		virtual bool isCulled(const math::SFrustum& frustum) const { return false; }

		virtual bool intersect(const XNA::OrientedBox& obb) const { return true; }

		virtual void OnRegisterSceneNode(bool bRecursion);

		virtual void getLightData(void* data) const;

		virtual math::SAxisAlignedBox getAabb() const;

		virtual void setShadowCameraOrthographicSize(f32 viewWidth, f32 viewHeight)
		{
			mShadowCameraViewWidth = viewWidth;
			mShadowCameraViewHeight = viewHeight;
		}

		virtual void generateShadowMap(ICameraNode* viewCamera);

		void setCameraAttributes(ICameraNode* shadowCamera, ICameraNode* viewCamera);

		virtual ITexture* getShadowMap()
		{
			return mShadowMap;
		}

		virtual XMFLOAT4X4 getShadowMapTransform(u32 index = 0)
		{
			return mShadowTransforms[index];
		}

		virtual void getShadowMapTransforms(XMFLOAT4X4 transforms[])
		{
			memcpy(transforms, mShadowTransforms, sizeof(mShadowTransforms));
		}

		virtual void setVisible(bool visible);

		virtual bool intersectRay(FXMVECTOR Origin, FXMVECTOR Direction, f32* pDist) const { return false; }

		virtual bool intersectRay(const math::SRay& ray, f32* pDist) const { return false; }

		virtual ~CDirectionalLightNode();

	protected:

		void disableLightInScene(ILightNode* light);

		void enableLightInScene(ILightNode* light);

		SDirectionalLight		mLightData;
		f32						mShadowCameraViewWidth;
		f32						mShadowCameraViewHeight;
		ITexture2DArray*		mShadowMap;
		
		XMFLOAT4X4				mShadowTransforms[ICameraNode::CASCADE_SHADOW_LEVEL];
		XMFLOAT4X4				mShadowCropTransforms[ICameraNode::CASCADE_SHADOW_LEVEL];
		
	};

}


#endif // !
