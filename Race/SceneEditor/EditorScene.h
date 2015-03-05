#ifndef __EDITOR_SCENE_CLASS_H__
#define __EDITOR_SCENE_CLASS_H__

using namespace gf;

class EditorScene
{
public:
	EditorScene(IDevice* device);
	~EditorScene();

	void setupInitialScene();


	static void _setInstance(EditorScene* scene) { _instance = scene; }
	static EditorScene* getInstance() { return _instance; }

	void update(f32 dt);
	void drawAll();
	
	void updateCamera(f32 delta);

	void changeTreePosition();
	void addTreeNode(u32 sx, u32 sy);


	void setRenderRegion(u32 width, u32 height)
	{
		BufferWndWidth = width;
		BufferWndHeight = height;
	}

	void PrepareAddingObject(const std::string& meshName);
	void AddObject();
	void CancelAddingObject();
	void ChangeAddedObjectPosition(u32 sx, u32 sy);
	void PickingObject(u32 sx, u32 sy);
	void SelectObject(u32 sx, u32 sy);

	void BeginFocusingObject();

private:

	math::SRay computePickingRay(u32 sx, u32 sy);
	void focusOnObject(f32 dt);


	static EditorScene* _instance;
	IDevice*					mDevice;
	IVideoDriver*				mVideoDriver;
	ITextureManager*			mTextureManager;
	IResourceGroupManager*		mResourceGroupManager;
	ISceneManager*				mSceneManager;
	IMeshManager*				mMeshManager;
	IMaterialManager*			mMaterialManager;
	
	ITimer*						mTimer;
	ICameraNode*				mCamera;
//	IMeshNode*					mTreeNode;

	const f32					mGroundSize;
	u32							BufferWndWidth;
	u32							BufferWndHeight;

	ISceneNode*					mAddedNode;
	IMeshNode*					mPickingCubeNode;
	IMeshNode*					mSelectedCubeNode;
	ISceneNode*					mSelectedNode;

	bool						mCameraFocusingObject;

};


#endif

