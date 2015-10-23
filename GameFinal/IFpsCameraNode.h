#ifndef __FPS_CAMERA_NODE_INTERFACE_H__
#define __FPS_CAMERA_NODE_INTERFACE_H__

#include "ICameraNode.h"

namespace gf
{
	enum E_FPS_CAMERA_ACTION
	{
		EFCA_MOVE_FORWARD = 0,
		EFCA_MOVE_BACK,
		EFCA_MOVE_LEFT,
		EFCA_MOVE_RIGHT,
		EFCA_JUMP,
		EFCA_COUNT,
	};
	
	class IFpsCameraNode : public ICameraNode
	{
	public:
		IFpsCameraNode(ISceneNode* parent, ISceneManager* smgr,
			const XMFLOAT3& position,
			f32 aspectRatio,
			f32 fov,
			f32 nearZ,
			f32 farZ,
			f32 maxUpAngle,
			f32 maxDownAngle,
			bool bPerspectiveProj)
			:ICameraNode(parent, smgr, position, aspectRatio, fov, nearZ, farZ, bPerspectiveProj)
			, mMaxUpAngle(maxUpAngle)
			, mMaxDownAngle(maxDownAngle)
		{
			
		}

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_FPS_CAMERA;
		}

		void setWalkSpeed(f32 speed) { mWalkSpeed = speed; }

		void setStrafeSpeed(f32 speed) { mStrafeSpeed = speed; }

		f32 getWalkSpeed() const { return mWalkSpeed; }

		f32 getStrafeSpeed() const { return mStrafeSpeed; }

		void setRotateSpeed(f32 speed) { mRotateSpeed = speed; }

		f32 getRotateSpeed() const { return mRotateSpeed; }

		virtual bool setActionMapping(E_FPS_CAMERA_ACTION action, u32 key) = 0;
		
		u32 getActionMapping(E_FPS_CAMERA_ACTION action) const
		{
			return mActionMappings[action];
		}
		
	protected:
		f32								mMaxUpAngle;			/* 最大抬头角度 */
		f32								mMaxDownAngle;			/* 最大低头角度 */
	
		f32								mWalkSpeed;
		f32								mStrafeSpeed;
		f32								mRotateSpeed; // radians per second
	
		u32								mActionMappings[EFCA_COUNT];
	};

}

#endif
