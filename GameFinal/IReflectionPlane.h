#ifndef __REFLECTION_PLANE_INTERFACE_H__
#define __REFLECTION_PLANE_INTERFACE_H__

#include "IReferenceCounted.h"
#include "ICameraNode.h"
#include "ITexture.h"

namespace gf
{

	class IReflectionPlane : public IReferenceCounted
	{
	public:

		virtual u32 getId() const = 0;
		
		virtual void render(ICameraNode* viewCamera) = 0;

		virtual ITexture* getReflectionMap() = 0;

		virtual XMFLOAT4X4 getViewMatrix() const = 0;

		virtual XMFLOAT4X4 getProjMatrix() const = 0;

		virtual XMFLOAT4X4 getViewProjMatrix() const = 0;

		virtual XMFLOAT3 getPosition() const = 0;

		virtual XMFLOAT4 getPlaneEquation() const = 0;

		virtual XMFLOAT2 getSize() const = 0;

		virtual XMFLOAT3 getTangent() const = 0;

		virtual math::SOrientedBox getOBB() const = 0;

	};
}

#endif

