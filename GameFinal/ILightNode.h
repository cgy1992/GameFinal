#ifndef __ILIGHTNODE_H_INCLUDE__
#define __ILIGHTNODE_H_INCLUDE__

#include "ISceneNode.h"
#include "IOctreeNode.h"
#include "ITexture.h"
#include "ICameraNode.h"

namespace gf
{
	class ISceneManager;
	class IOctreeNode;

	enum E_LIGHT_TYPE
	{
		ELT_POINT,
		ELT_DIRECTIONAL,
		ELT_SPOT
	};
	/*
	D3DLIGHTTYPE  Type;
	D3DCOLORVALUE Diffuse;
	D3DCOLORVALUE Specular;
	D3DCOLORVALUE Ambient;
	D3DVECTOR     Position;
	D3DVECTOR     Direction;
	float         Range;
	float         Falloff;
	float         Attenuation0;
	float         Attenuation1;
	float         Attenuation2;
	float         Theta;
	float         Phi;

	struct SLightData
	{
		XMFLOAT4	Diffuse;
		XMFLOAT4	Specular;
		//XMFLOAT4	Ambient;
		XMFLOAT3	Position;
		f32			Range;
		XMFLOAT3	Direction;
		f32			Falloff;
		XMFLOAT3	Attenuations;
		f32			Theta;
	};
	*/


	struct SDirectionalLight
	{
		XMFLOAT4	Diffuse;
		XMFLOAT4	Specular;
		//XMFLOAT4	Ambient;
		XMFLOAT4	Direction;
	};

	struct SPointLight
	{
		XMFLOAT4	Diffuse;
		XMFLOAT4	Specular;
		//XMFLOAT4	Ambient;
		XMFLOAT3	Position;
		f32			Range;
		XMFLOAT3	Attenuations;
		f32			_pad;
	};

	struct SSpotLight
	{
		XMFLOAT4	Diffuse;
		XMFLOAT4	Specular;
		//XMFLOAT4	Ambient;
		XMFLOAT4	Direction;
		XMFLOAT3	Position;
		f32			Range;
		XMFLOAT3	Attenuations;
		f32			Falloff;
		f32			InnerCone;
		f32			OuterCone;
		f32			_pad[2];
	};

	class ILightNode : public ISceneNode
	{
	
	public:
		ILightNode(ISceneNode* parent,
			ISceneManager* smgr,
			bool bStatic,
			u32 id,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0))
			:ISceneNode(parent, smgr, bStatic, position)
			, mId(id)
			, mOctreeNode(nullptr)
			, mInsideFrustum(false)
			, mCastShadow(false)
		{
			
		}

		virtual u32 getId() const { return mId; }

		virtual E_LIGHT_TYPE getType() const = 0;

		virtual void setDiffuse(const XMFLOAT4& diffuse) = 0;

		virtual XMFLOAT4 getDiffuse() const = 0;

		virtual void setSpecular(const XMFLOAT4& specular) = 0;

		virtual XMFLOAT4 getSpecular() const = 0;

		virtual void setAttenuation(f32 a0, f32 a1, f32 a2) = 0;

		virtual XMFLOAT3 getAttenuation() const = 0;

		virtual void setDirection(const XMFLOAT3& direction) = 0;

		virtual XMFLOAT4 getDirection() const = 0;

		virtual void setRange(f32 range) = 0;

		virtual f32 getRange() const = 0;

		virtual void setFalloff(f32 falloff) = 0;

		virtual f32 getFalloff() const = 0;

		virtual void setTheta(f32 theta) = 0;

		virtual f32 getTheta() const = 0;

		virtual bool isCulled(const math::SFrustum& frustum) const = 0;

		virtual bool intersect(const XNA::OrientedBox& obb) const = 0;

		virtual void getLightData(void* data) const = 0;

		virtual bool castShadow() const { return mCastShadow; }

		virtual void enableShadow(bool enabled) { mCastShadow = enabled; }

		void setShadowMapSize(u32 w, u32 h)
		{
			mShadowMapWidth = w;
			mShadowMapHeight = h;
		}

		void getShadowMapSize(u32& w, u32& h) const
		{
			w = mShadowMapWidth;
			h = mShadowMapHeight;
		}

		virtual void setShadowCameraOrthographicSize(f32 viewWidth, f32 viewHeight) {}

		virtual void generateShadowMap(ICameraNode* viewCamera) {}

		virtual XMFLOAT4X4 getShadowMapTransform(u32 index = 0) = 0;

		virtual void getShadowMapTransforms(XMFLOAT4X4 transforms[]) {}

		virtual ITexture* getShadowMap() = 0;

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_LIGHT;
		}

		virtual IOctreeNode*	getOctreeNode()
		{
			return mOctreeNode;
		}

		virtual void setOctreeNode(IOctreeNode* octreeNode)
		{
			mOctreeNode = octreeNode;
		}

		void setInsideFrustum(bool bInside)
		{
			mInsideFrustum = bInside;
		}

		bool insideFrustum()
		{
			return mInsideFrustum;
		}

		

	protected:
		u32				mId;
		bool			mInsideFrustum;
		bool			mCastShadow;
		IOctreeNode*	mOctreeNode;
		u32				mShadowMapWidth;
		u32				mShadowMapHeight;

	};

}

#endif
