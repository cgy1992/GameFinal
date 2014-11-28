#ifndef __ISCENENODE_H__
#define __ISCENENODE_H__

#include "gfTypes.h"
#include "IReferenceCounted.h"
#include "xnacollision/xnacollision.h"
#include "gfMath.h"
#include "gfEnums.h"
#include <list>

namespace gf
{

	class ISceneManager;

	enum E_RENDER_ORDER
	{
		ERO_SKYDOME = 240,
		ERO_MESH = 128,
	};

	enum E_NODE_TYPE
	{
		ENT_UNKNOWN,
		ENT_SOLID_NODE,
		ENT_INSTANCE_NODE,
		ENT_CAMERA_NODE,
		ENT_LIGHT_NODE
	};

	/* the lowest 4 bits are used for different kinds in the same types.
	
	*/
	enum E_SCENE_NODE_TYPE
	{
		ESNT_MESH = 0x10,			// 10000 
		ESNT_MODEL_MESH = 0x11,		// 10001
		ESNT_ANIMATED_MESH = 0x12,	// 10010
		ESNT_TERRAIN_MESH = 0x13,   // 10011
		ESNT_INSTANCE_MESH = 0x14,	// 10100
		ESNT_INSTANCE_COLLECTION_MESH = 0x15, // 10101

		ESNT_CAMERA = 0x20,			// 100000
		ESNT_FPS_CAMERA = 0x21,		// 100001

		ESNT_OCTREE_MANAGER = 0x40,	// 100,0000
		
		ESNT_EMPTY = 0x80,			// 1000,0000

		ESNT_LIGHT = 0x100,			// 10000,0000

		ESNT_SCENE_MANAGER = 0x200,	// 100000,0000
	};

	class ISceneNode : public IReferenceCounted
	{
	public:
		ISceneNode(ISceneNode* parent, ISceneManager* smgr, bool bStatic, 
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:mParent(parent)
			, mSceneManager(smgr)
			, mPosition(position)
			//, mScale(scale)
			, mVisible(true)
			, mSortCode(0)
			, mNeedCulling(true)
			, mRenderOrder(0)
			, mStatic(bStatic)
		{
			//XMMATRIX rot = XMMatrixIdentity();
			//XMStoreFloat4x4(&mOrientation, rot);

			if (parent)
				parent->addChild(this);

			//XMVECTOR r = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
			//XMStoreFloat4(&mOrientation, r);

			XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
			XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
			XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);
			
			
			XMStoreFloat4x4(&mTransformation, S * R * T);

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

				updateAbsoluteTransformation();
			}
		}

		void destoryRecursion()
		{
			auto it = mChildren.begin();
			for (; it != mChildren.end(); it++)
			{
				(*it)->destoryRecursion();
			}
			while (!this->drop());
		}


		virtual void destroy()
		{
			remove();
			destoryRecursion();
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

		virtual void render(E_PIPELINE_USAGE usage) = 0;

		virtual void translate(f32 x, f32 y, f32 z)
		{
			mPosition.x += x;
			mPosition.y += y;
			mPosition.z += z;

			XMMATRIX m = XMLoadFloat4x4(&mTransformation);
			XMMATRIX transMatrix = XMMatrixTranslation(x, y, z);
			m = m * transMatrix;
			XMStoreFloat4x4(&mTransformation, m);
		}

		virtual void translate(const XMFLOAT3& v)
		{
			translate(v.x, v.y, v.z);
		}

		virtual void setPosition(f32 x, f32 y, f32 z)
		{
			mPosition.x = x;
			mPosition.y = y;
			mPosition.z = z;

			mTransformation._41 = x;
			mTransformation._42 = y;
			mTransformation._43 = z;
		}

		virtual void setPosition(const XMFLOAT3& pos)
		{
			setPosition(pos.x, pos.y, pos.z);
		}

		virtual void resetTransform()
		{
			XMMATRIX I = XMMatrixIdentity();
			XMStoreFloat4x4(&mTransformation, I);
		}

		virtual void setTransform(CXMMATRIX M)
		{
			XMStoreFloat4x4(&mTransformation, M);
		}

		virtual void setTransform(const XMFLOAT4X4& M)
		{
			mTransformation = M;
		}
		
		virtual void transform(CXMMATRIX M)
		{
			XMMATRIX trans = XMLoadFloat4x4(&mTransformation);
			trans = XMMatrixMultiply(trans, M);
			XMStoreFloat4x4(&mTransformation, trans);
		}

		virtual void pitch(f32 angle)
		{
			XMMATRIX m = XMLoadFloat4x4(&mTransformation);
			XMMATRIX rotMat = XMMatrixRotationX(angle);
			m = m * rotMat;
			XMStoreFloat4x4(&mTransformation, m);
		}

		virtual void yaw(f32 angle)
		{
			XMMATRIX m = XMLoadFloat4x4(&mTransformation);
			XMMATRIX rotMat = XMMatrixRotationY(angle);
			m = m * rotMat;
			XMStoreFloat4x4(&mTransformation, m);
		}

		virtual void roll(f32 angle)
		{
			XMMATRIX m = XMLoadFloat4x4(&mTransformation);
			XMMATRIX rotMat = XMMatrixRotationZ(angle);
			m = m * rotMat;
			XMStoreFloat4x4(&mTransformation, m);
		}

		virtual void rotate(f32 x, f32 y, f32 z)
		{
			XMMATRIX m = XMLoadFloat4x4(&mTransformation);
			XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(x, y, z);
			m = m * rotMat;
			XMStoreFloat4x4(&mTransformation, m);
		}

		virtual void scale(f32 x, f32 y, f32 z)
		{
			XMMATRIX m = XMLoadFloat4x4(&mTransformation);
			XMMATRIX scaleMatrix = XMMatrixScaling(x, y, z);
			m = m * scaleMatrix;
			XMStoreFloat4x4(&mTransformation, m);
		}
		
		virtual void scale(f32 s)
		{
			XMMATRIX m = XMLoadFloat4x4(&mTransformation);
			XMMATRIX scaleMatrix = XMMatrixScaling(s, s, s);
			m = m * scaleMatrix;
			XMStoreFloat4x4(&mTransformation, m);
		}


		virtual XMMATRIX getRelativeTransformation() const
		{
			return XMLoadFloat4x4(&mTransformation);
		}

		virtual XMMATRIX getAbsoluteTransformation() const
		{
			return XMLoadFloat4x4(&mAbsoluteTransformation);
		}

		virtual XMFLOAT4X4 getWorldTransformation() const
		{
			return mAbsoluteTransformation;
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

		XMFLOAT3 getPosition() const
		{
			//return mPosition;
			return XMFLOAT3(mTransformation._41, mTransformation._42, mTransformation._43);
		}

		virtual void update(u32 delta = 0)
		{
			if (mVisible)
			{
				updateAbsoluteTransformation();

				auto it = mChildren.begin();
				for (; it != mChildren.end(); ++it)
					(*it)->update(delta);
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

			/*
			XMVECTOR v = XMLoadFloat3(&axes[0]);
			v = XMVector3Normalize(v);
			XMStoreFloat3(&axes[0], v);

			v = XMLoadFloat3(&axes[1]);
			v = XMVector3Normalize(v);
			XMStoreFloat3(&axes[1], v);

			v = XMLoadFloat3(&axes[2]);
			v = XMVector3Normalize(v);
			XMStoreFloat3(&axes[2], v);
			*/
		}

		virtual E_SCENE_NODE_TYPE getNodeType() const = 0;

		virtual bool needCulling()
		{
			return mNeedCulling;
		}


		virtual void setNeedCulling(bool cull)
		{
			mNeedCulling = cull;
		}

		virtual void setRenderOrder(u8 order)
		{
			mRenderOrder = order;
		}

		virtual u8 getRenderOrder() const
		{
			return mRenderOrder;
		}

		virtual bool isStatic() const
		{
			return mStatic;
		}

		

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

		XMFLOAT4X4 mTransformation;

		//! Relative translation of the scene node.
		XMFLOAT3 mPosition;

		//! Relative rotation of the scene node.
		//XMFLOAT4 mOrientation;
		//XMFLOAT4X4 mOrientation;

		//! Relative scale of the scene node.
		//XMFLOAT3 mScale;

		bool mVisible;

		bool mNeedCulling;

		u64 mSortCode;

		u8  mRenderOrder;

		// if the node is static in the scene.
		bool mStatic;

	};

}


#endif



