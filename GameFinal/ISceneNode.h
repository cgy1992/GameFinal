#ifndef __ISCENENODE_H__
#define __ISCENENODE_H__

#include "gfTypes.h"
#include "IReferenceCounted.h"
#include "xnacollision/xnacollision.h"
#include "gfMath.h"
#include <list>

namespace gf
{

	class ISceneManager;

	enum E_NODE_TYPE
	{
		ENT_UNKNOWN,
		ENT_SOLID_NODE,
		ENT_CAMERA_NODE,
		ENT_LIGHT_NODE
	};

	enum E_SCENE_NODE_TYPE
	{
		ESNT_EMPTY = 0x01,			// 1
		ESNT_LIGHT = 0x02,			// 10
		ESNT_SCENE_MANAGER = 0x03,

		ESNT_MESH = 0x10,			// 10000 
		ESNT_MODEL_MESH = 0x11,		// 10001
		ESNT_ANIMATED_MESH = 0x12,	// 10010

		ESNT_CAMERA = 0x20,			// 100000
		ESNT_FPS_CAMERA = 0x21,		// 100001

		ESNT_OCTREE_MANAGER = 0x40	// 100,0000

	};

	class ISceneNode : public IReferenceCounted
	{
	public:
		ISceneNode(ISceneNode* parent, ISceneManager* smgr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:mParent(parent)
			, mSceneManager(smgr)
			, mPosition(position)
			, mScale(scale)
			, mVisible(true)
			, mSortCode(0)

		{
			XMMATRIX rot = XMMatrixIdentity();
			XMStoreFloat4x4(&mOrientation, rot);

			if (parent)
				parent->addChild(this);

			//XMVECTOR r = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
			//XMStoreFloat4(&m_orientation, r);

			updateAbsoluteTransformation();
		}

		//! Adds a child to this scene node.
		/** If the scene node already has a parent it is first removed
		from the other parent.
		\param child A pointer to the new child. */
		virtual void addChild(ISceneNode* child)
		{
			if (child && (child != this))
			{
				// Change scene manager?
				if (mSceneManager != child->mSceneManager)
					child->setSceneManager(mSceneManager);

				child->grab();
				child->remove(); // remove from old parent
				mChildren.push_back(child);
				child->mParent = this;
			}
		}

		virtual void destroy()
		{
			auto it = mChildren.begin();
			for (; it != mChildren.end(); it++)
				(*it)->destroy();

			while (!this->drop());
		}

		virtual bool removeChild(ISceneNode* child)
		{
			auto it = mChildren.begin();
			for (; it != mChildren.end(); ++it)
			if ((*it) == child)
			{
				(*it)->mParent = nullptr;
				(*it)->drop();
				mChildren.erase(it);
				return true;
			}
			return false;
		}

		//! Removes this scene node from the scene
		/** If no other grab exists for this node, it will be deleted.
		*/
		virtual void remove()
		{
			if (mParent)
				mParent->removeChild(this);
		}

		virtual ISceneNode* getParent() const
		{
			return mParent;
		}

		virtual void OnRegisterSceneNode(bool bRecursion = true)
		{
			if (bRecursion)
			{
				if (mVisible)
				{
					auto it = mChildren.begin();
					for (; it != mChildren.end(); ++it)
						(*it)->OnRegisterSceneNode();
				}
			}
		}

		//! Sets the new scene manager for this node and all children.
		//! Called by addChild when moving nodes between scene managers
		void setSceneManager(ISceneManager* newManager)
		{
			mSceneManager = newManager;

			auto it = mChildren.begin();
			for (; it != mChildren.end(); ++it)
				(*it)->setSceneManager(newManager);
		}

		virtual ISceneManager* getSceneManager(void) const { return mSceneManager; }

		virtual void render() = 0;

		virtual void translate(f32 x, f32 y, f32 z)
		{
			mPosition.x += x;
			mPosition.y += y;
			mPosition.z += z;
		}

		virtual void setPosition(f32 x, f32 y, f32 z)
		{
			mPosition.x = x;
			mPosition.y = y;
			mPosition.z = z;
		}


		virtual void setOrientation(CXMMATRIX M)
		{
			XMStoreFloat4x4(&mOrientation, M);
		}

		/*
		void rotate(const XMFLOAT3& axis, f32 angle)
		{
		XMVECTOR axisvector = XMLoadFloat3(&axis);
		XMVECTOR q1 = XMLoadFloat4(&m_orientation);
		XMVECTOR q2 = XMQuaternionRotationAxis(axisvector, angle);
		XMVECTOR q	= XMQuaternionMultiply(q2, q1);
		XMQuaternionNormalize(q);
		XMStoreFloat4(&m_orientation, q);
		}

		void rotate(f32 Pitch, f32 Yaw, f32 Roll)
		{
		XMVECTOR q1 = XMLoadFloat4(&m_orientation);
		XMVECTOR q2 = XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll);
		XMVECTOR q = XMQuaternionMultiply(q2, q1);
		XMQuaternionNormalize(q);
		XMStoreFloat4(&m_orientation, q);
		}

		void setOrientation(const XMFLOAT3& axis, f32 angle)
		{
		XMVECTOR axisvector = XMLoadFloat3(&axis);
		XMVECTOR q = XMQuaternionRotationAxis(axisvector, angle);
		XMQuaternionNormalize(q);
		XMStoreFloat4(&m_orientation, q);
		}

		void setOrientation(const XMFLOAT4& quaternion)
		{
		m_orientation = quaternion;
		}

		void pitch(f32 angle)
		{
		XMVECTOR unit_x = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR q1 = XMLoadFloat4(&m_orientation);
		XMVECTOR q2 = XMQuaternionRotationNormal(unit_x, angle);
		XMVECTOR q = XMQuaternionMultiply(q2, q1);
		XMQuaternionNormalize(q);
		XMStoreFloat4(&m_orientation, q);
		}

		void yaw(f32 angle)
		{
		XMVECTOR unit_y = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR q1 = XMLoadFloat4(&m_orientation);
		XMVECTOR q2 = XMQuaternionRotationNormal(unit_y, angle);
		XMVECTOR q = XMQuaternionMultiply(q2, q1);
		XMQuaternionNormalize(q);
		XMStoreFloat4(&m_orientation, q);
		}

		void roll(f32 angle)
		{
		XMVECTOR unit_z = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMVECTOR q1 = XMLoadFloat4(&m_orientation);
		XMVECTOR q2 = XMQuaternionRotationNormal(unit_z, angle);
		XMVECTOR q = XMQuaternionMultiply(q2, q1);
		XMQuaternionNormalize(q);
		XMStoreFloat4(&m_orientation, q);
		}

		*/

		virtual void pitch(f32 angle)
		{
			XMMATRIX m = XMLoadFloat4x4(&mOrientation);
			XMMATRIX rotMat = XMMatrixRotationX(angle);
			m = m * rotMat;
			XMStoreFloat4x4(&mOrientation, m);
		}

		virtual void yaw(f32 angle)
		{
			XMMATRIX m = XMLoadFloat4x4(&mOrientation);
			XMMATRIX rotMat = XMMatrixRotationY(angle);
			m = m * rotMat;
			XMStoreFloat4x4(&mOrientation, m);
		}

		virtual void roll(f32 angle)
		{
			XMMATRIX m = XMLoadFloat4x4(&mOrientation);
			XMMATRIX rotMat = XMMatrixRotationZ(angle);
			m = m * rotMat;
			XMStoreFloat4x4(&mOrientation, m);
		}

		virtual void setScale(f32 x, f32 y, f32 z)
		{
			mScale.x = x;
			mScale.y = y;
			mScale.z = z;
		}

		virtual void scale(f32 x, f32 y, f32 z)
		{
			mScale.x *= x;
			mScale.y *= y;
			mScale.z *= z;
		}

		virtual XMMATRIX getRelativeTransformation() const
		{
			/*
			XMVECTOR S = XMVectorSet(mScale.x, mScale.y, mScale.z, 0.0f);
			XMVECTOR R = XMLoadFloat4(&m_orientation);
			XMVECTOR T = XMVectorSet(mPosition.x, mPosition.y, mPosition.z, 0.0f);
			XMVECTOR zero = XMVectorZero();

			XMMATRIX m = XMMatrixAffineTransformation(S, zero, R, T);

			return m;
			*/

			XMMATRIX S = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
			//	XMVECTOR quat = XMLoadFloat4(&);
			//	XMMATRIX R = XMMatrixRotationQuaternion(quat);
			XMMATRIX T = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
			XMMATRIX R = XMLoadFloat4x4(&mOrientation);
			//XMMATRIX R = XMMatrixIdentity();

			return S * R * T;
		}

		virtual XMMATRIX getAbsoluteTransformation()
		{
			return XMLoadFloat4x4(&mAbsoluteTransformation);
		}

		virtual void updateAbsoluteTransformation()
		{
			XMMATRIX absoluteMatrix;
			if (mParent)
			{
				absoluteMatrix = XMMatrixMultiply(getRelativeTransformation(), mParent->getAbsoluteTransformation());
				//absoluteMatrix = XMMatrixMultiply(mParent->getAbsoluteTransformation(), getRelativeTransformation());
			}
			else {
				absoluteMatrix = getRelativeTransformation();
			}

			XMStoreFloat4x4(&mAbsoluteTransformation, absoluteMatrix);
		}

		XMFLOAT3 getAbsolutePosition() const
		{
			return XMFLOAT3(mAbsoluteTransformation._41, mAbsoluteTransformation._42, mAbsoluteTransformation._43);
		}

		virtual void OnAnimate(u32 timeMs = 0)
		{
			if (mVisible)
			{
				updateAbsoluteTransformation();

				auto it = mChildren.begin();
				for (; it != mChildren.end(); ++it)
					(*it)->OnAnimate(timeMs);
			}
		}

		virtual u64 getSortCode() const
		{
			return mSortCode;
		}

		virtual math::SAxisAlignedBox getAabb() const
		{
			math::SAxisAlignedBox aabb;
			return aabb;
		}

		virtual math::SOrientedBox getOrientedBox() const
		{
			math::SOrientedBox obb;
			return obb;
		}

		virtual bool isVisible() const
		{
			return mVisible;
		}

		virtual void setVisible(bool visible)
		{
			mVisible = visible;
		}

		virtual void getLocalAxis(XMFLOAT3 axes[]) const
		{
			memcpy(&axes[0], mAbsoluteTransformation.m[0], sizeof(XMFLOAT3));
			memcpy(&axes[1], mAbsoluteTransformation.m[1], sizeof(XMFLOAT3));
			memcpy(&axes[2], mAbsoluteTransformation.m[2], sizeof(XMFLOAT3));
		}

		virtual E_SCENE_NODE_TYPE getNodeType() const = 0;


		class CSceneNodeIterator
		{
			friend class ISceneNode;
		public:
			CSceneNodeIterator(const std::list<ISceneNode*>& nodelist)
				: mNodeList(nodelist)
				, mIt(nodelist.begin())
			{

			}
			bool hasNext()
			{
				return mIt != mNodeList.end();
			}

			ISceneNode* next()
			{
				ISceneNode* node = *mIt;
				mIt++;
				return node;
			}

		private:
			const std::list<ISceneNode*>&			mNodeList;
			std::list<ISceneNode*>::const_iterator	mIt;
		};

		CSceneNodeIterator ISceneNode::getChildrenIterator() const
		{
			return CSceneNodeIterator(mChildren);
		}

	protected:
		ISceneManager* mSceneManager;
		ISceneNode* mParent;
		std::list<ISceneNode*> mChildren;

		//! Absolute transformation of the node.
		XMFLOAT4X4 mAbsoluteTransformation;

		//! Relative translation of the scene node.
		XMFLOAT3 mPosition;

		//! Relative rotation of the scene node.
		//XMFLOAT4 m_orientation;
		XMFLOAT4X4 mOrientation;

		//! Relative scale of the scene node.
		XMFLOAT3 mScale;

		bool mVisible;

		u64 mSortCode;
	};

}


#endif



