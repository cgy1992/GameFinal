#ifndef __EDITOR_SCENE_CLASS_H__
#define __EDITOR_SCENE_CLASS_H__

#include "SNodeInfo.h"
#include "SCollectionNodeInfo.h"

using namespace gf;

class EditorScene
{
public:
	friend class CGlobalSettingWindow;
	friend class CFileParser;
	friend class CCreateMeshNodeWindow;
	friend class CCreateLightNodeWindow;

public:
	EditorScene(IApplication* device);
	~EditorScene();

	void setupInitialScene();
	void setupTerrain();

	static void _setInstance(EditorScene* scene) { _instance = scene; }
	static EditorScene* getInstance() { return _instance; }

	void update(f32 dt);
	void drawAll();
	
	//void OnKeyBoard(f32 delta);
	void UpdateSelectedObjectTransform(f32 delta);
	void UpdateCamera(f32 delta);

	void setRenderRegion(u32 width, u32 height)
	{
		BufferWndWidth = width;
		BufferWndHeight = height;
	}

	bool PrepareAddingObject(const std::string& meshName);
	bool PrepareAddingInstance(u32 collectionId);
	bool PrepareAddingLight();

	int AddCollectionNode(const char* nodeName, 
		const std::string& meshName, int maxNum);
	IInstanceCollectionNode* CreateCollectionNode(IModelMesh* mesh,int maxNum);

	u32 AddObject(const char* nodeName);
	u32 AddInstanceNode();
	u32 AddLightNode(const char* nodeName);

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
	void UpdateNodeInfo(SNodeInfo* info);
	static u32 getNextNodeSequence();
	void MoveSelectedNode();

	int GetIdBySceneNode(ISceneNode* node);
	SNodeInfo* GetNodeInfoBySceneNode(ISceneNode* node);

	std::vector<int> GetInstanceIdsInOneCollection(int collectionId);
	
	bool DeleteNode(u32 id);
	bool DeleteLight(u32 id);

	SCollectionNodeInfo* GetCollectionNodeInfoById(u32 id);
	SLightNodeInfo*	GetLightNodeInfoById(u32 id);
	void PickingLight(u32 sx, u32 sy);
	//SLightNodeInfo* GetSelectedLightNodeInfo(u32 id);
	SLightNodeInfo* GetLightNodeInfoBySceneNode(ISceneNode* node);
	SLightNodeInfo* GetSelectedLightNodeInfo();
	bool SelectLight(u32 id);
	int SelectLight(u32 sx, u32 sy);
	bool SelectLight(SLightNodeInfo* lightInfo);
	void UpdateLightNodeInfo(SLightNodeInfo* info);
	void UpdateGlobalLighting();
	
	ICameraNode* GetCamera() { return mCamera; }
	
	void FocusOnMeshNode(IMeshNode* node, f32 distance);
	void FocusSelectedObject();
	void FocusSelectedLight();
private:

	math::SRay computePickingRay(u32 sx, u32 sy);

	void updateSelectedNodeCube();
	void setInstanceNodeColor(IInstanceNode* node, XMFLOAT4 color);
	SLightNodeInfo* getIntersectPointLightInfo(u32 sx, u32 sy);

	static EditorScene* _instance;
	IApplication*					mDevice;
	IVideoDriver*				mVideoDriver;
	ITextureManager*			mTextureManager;
	IResourceGroupManager*		mResourceGroupManager;
	ISceneManager*				mSceneManager;
	IMeshManager*				mMeshManager;
	IMaterialManager*			mMaterialManager;
	IShader*					mTileBasedDSShader;
	bool						mDeferredShading;

	ITimer*						mTimer;
	IFpsCameraNode*				mCamera;
	ITerrainNode*				mTerrainNode;
	ILightNode*					mDirectionalLightNode;
	

	const f32					mGroundSize;
	u32							BufferWndWidth;
	u32							BufferWndHeight;

	ISceneNode*					mAddedNode;
	IMeshNode*					mPickingCubeNode;
	IMeshNode*					mSelectedCubeNode;
	ISceneNode*					mSelectedNode;

	IInstanceNode*				mPickingPointLightInstance;

	bool						mCameraFocusingObject;

	static u32					nodeSequenceNumber;

	std::map<u32, SNodeInfo>	mNodeInfos;
	std::map<ISceneNode*, u32>  mNodeIdMap;
	std::vector<SCollectionNodeInfo>	mCollectionNodeInfos;
	// for lighting panel
	
	SLightNodeInfo*				mAddedLightNodeInfo;
	SLightNodeInfo*				mSelectedLightNodeInfo;

	std::map<u32, SLightNodeInfo>		mLightNodeInfos;

public:
	IInstanceCollectionNode*	mPointLightCollectionNode;
};


#endif

