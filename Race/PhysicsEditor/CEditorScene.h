#pragma once

#include "SMeshNodeInfo.h"

class CEditorScene
{
	friend class CXmlFileManager;

public:
	CEditorScene(IApplication* device);
	void setupInitialScene();
	~CEditorScene();
	static void _setInstance(CEditorScene* scene) { _instance = scene; }
	static CEditorScene* getInstance() { return _instance; }
	void update(f32 dt);
	void drawAll();
	void UpdateCamera(f32 delta);

	SMeshNodeInfo* SelectMesh(const std::string meshName);
	SBoxBounding* AddBoxBounding();
	SCylinderBounding* AddCylinderBounding();
	SBoundingShape* SelectBoundingShape(u32 index);
	SBoundingShape* GetBoundingShapeByIndex(u32 index);
	void UpdateWireFrameNode(SBoundingShape* shape);
	void DeleteBounding(u32 index);

private:

	static CEditorScene*		_instance;
	IApplication*					mDevice;
	IVideoDriver*				mVideoDriver;
	ITextureManager*			mTextureManager;
	IResourceGroupManager*		mResourceGroupManager;
	ISceneManager*				mSceneManager;
	IMeshManager*				mMeshManager;
	IMaterialManager*			mMaterialManager;
	IShader*					mTileBasedDSShader;
	ITimer*						mTimer;

	u32							BufferWndWidth;
	u32							BufferWndHeight;
	ICameraNode*				mCamera;

	SMeshNodeInfo*				mCurrentMeshNodeInfo;
	SBoundingShape*				mCurrentBoundingShape;

	std::map<std::string, SMeshNodeInfo>		mMeshNodeInfos;

	ISimpleMesh*				mCubeMesh;
	ISimpleMesh*				mSphereMesh;
	ISimpleMesh*				mCylinderMesh;

	std::vector<std::string>	mMeshNames;

};

