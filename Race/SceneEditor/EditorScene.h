#ifndef __EDITOR_SCENE_CLASS_H__
#define __EDITOR_SCENE_CLASS_H__

#include "SNodeInfo.h"
#include "SCollectionNodeInfo.h"

using namespace gf;

class EditorScene
{
public:
	EditorScene(IDevice* device);
	~EditorScene();

	void setupInitialScene();
	void setupTerrain();

	static void _setInstance(EditorScene* scene) { _instance = scene; }
	static EditorScene* getInstance() { return _instance; }

	void update(f32 dt);
	void drawAll();
	
	void OnKeyBoard(f32 delta);
	void updateSelectedObjectTransform(f32 delta);
	void updateCamera(f32 delta);

	void setRenderRegion(u32 width, u32 height)
	{
		BufferWndWidth = width;
		BufferWndHeight = height;
	}

	bool PrepareAddingObject(const std::string& meshName);
	bool PrepareAddingInstance(u32 collectionId);
	bool PrepareAddingLight();

	int AddCollectionNode(const std::string& meshName, int maxNum);

	u32 AddObject();
	u32 AddInstanceNode();
	u32 AddLightNode();

	void CancelAddingObject();
	bool GetPickingPosition(u32 sx, u32 sy, XMFLOAT3& position);
	void ChangeAddedObjectPosition(u32 sx, u32 sy);
	void ChangeAddingLightPosition(u32 sx, u32 sy);

	void PickingObject(u32 sx, u32 sy);

	void CancelSelectObject();
	void SelectObject(ISceneNode* node);
	int SelectObject(u32 sx, u32 sy);
	bool SelectObject(u32 id);
	SNodeInfo* GetNodeInfoById(u32 id);
	SNodeInfo* GetSelectedNodeInfo();
	void BeginFocusingObject();
	void UpdateNodeInfo(SNodeInfo* info);
	static u32 getNextNodeSequence();
	void MoveSelectedNode();

	int GetIdBySceneNode(ISceneNode* node);
	SNodeInfo* GetNodeInfoBySceneNode(ISceneNode* node);

	std::vector<int> GetInstanceIdsInOneCollection(int collectionId);
	
	bool DeleteNode(u32 id);
	SCollectionNodeInfo* GetCollectionNodeInfoById(u32 id);

private:

	math::SRay computePickingRay(u32 sx, u32 sy);
	void focusOnObject(f32 dt);

	void updateSelectedNodeCube();

	static EditorScene* _instance;
	IDevice*					mDevice;
	IVideoDriver*				mVideoDriver;
	ITextureManager*			mTextureManager;
	IResourceGroupManager*		mResourceGroupManager;
	ISceneManager*				mSceneManager;
	IMeshManager*				mMeshManager;
	IMaterialManager*			mMaterialManager;
	IShader*					mTileBasedDSShader;
	bool						mDeferredShading;

	ITimer*						mTimer;
	ICameraNode*				mCamera;
	ITerrainNode*				mTerrainNode;

	

	const f32					mGroundSize;
	u32							BufferWndWidth;
	u32							BufferWndHeight;

	ISceneNode*					mAddedNode;
	IMeshNode*					mPickingCubeNode;
	IMeshNode*					mSelectedCubeNode;
	ISceneNode*					mSelectedNode;

	bool						mCameraFocusingObject;

	static u32					nodeSequenceNumber;

	std::map<u32, SNodeInfo>	mNodeInfos;
	std::map<ISceneNode*, u32>  mNodeIdMap;
	std::vector<SCollectionNodeInfo>	mCollectionNodeInfos;
	// for lighting panel
	
	SLightNodeInfo*				mAddedLightNodeInfo;

	std::map<u32, SLightNodeInfo>		mLightNodeInfos;

public:
	IInstanceCollectionNode*	mPointLightCollectionNode;
};


#endif

