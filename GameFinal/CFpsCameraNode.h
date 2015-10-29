#ifndef __FPS_CAMERA_NODE_CLASS_H__
#define __FPS_CAMERA_NODE_CLASS_H__

#include "IFpsCameraNode.h"
#include "IInputDriver.h"

namespace gf
{

	class CFpsCameraNode : public IFpsCameraNode
	{
	public:
		CFpsCameraNode(ISceneNode* parent, ISceneManager* smgr,
			const XMFLOAT3& position,
			const XMFLOAT3& lookat,
			const XMFLOAT3& up,
			f32 aspectRatio,
			f32 fov,
			f32 nearZ,
			f32 farZ,
			f32 maxUpAngle,
			f32 maxDownAngle,
			bool bPerspectiveProj);

		virtual void lookAt(const XMFLOAT3& lookat);

		virtual void lookAt(const XMFLOAT3& lookat, const XMFLOAT3& up);

		virtual void look(const XMFLOAT3& dir, const XMFLOAT3& up);

		virtual void look(const XMFLOAT3& d);

		virtual void render(E_PIPELINE_USAGE usage)
		{

		}

		virtual void walk(f32 unit);

		virtual void fly(f32 unit);

		virtual void strafe(f32 unit);

		/* rotate along camera's right vector. */
		virtual void pitch(f32 angle);

		/* rotate along camera's up vector. */
		virtual void yaw(f32 angle);

		/* rotate along camera's look vector. */
		virtual void roll(f32 angle);

		virtual void setOrientation(CXMMATRIX M)
		{

		}

		virtual XMMATRIX getRelativeTransformation() const
		{
			return XMLoadFloat4x4(&mRelativeTransform);
		}

		virtual XMMATRIX getAbsoluteTransformation()
		{
			return XMLoadFloat4x4(&mAbsoluteTransformation);
		}

		virtual void updateAbsoluteTransformation();

		virtual void update(f32 delta = 0);

		bool setActionMapping(E_FPS_CAMERA_ACTION action, u32 key);

		

	private:

		void adjustPitchAngle()
		{
			/* the pitch angle must between -maxUpAngle and maxDownAngle*/

			if (mPitchAngle > mMaxDownAngle)
				mPitchAngle = mMaxDownAngle;
			if (mPitchAngle < -mMaxUpAngle)
				mPitchAngle = -mMaxUpAngle;
		}

		XMFLOAT3					mWorldUp;				/* ����������� */
		XMFLOAT3					mWalkDir;				/* ��������߷��� */
		f32							mPitchAngle;			/* �����Ƕȣ�������ƫ�����߷���ĽǶ� */

		XMFLOAT4X4					mRelativeTransform;		/* �������Եı任������ getRelativeTransformation �����ķ���ֵ. */
	
		IInputDriver*				mInputDriver;
		IMouseDevice*				mMouse;
		IKeyboardDevice*			mKeyboard;

		
	};

}

#endif
