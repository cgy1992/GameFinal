#pragma once
class CEditorScene
{
public:
	CEditorScene(IDevice* device);
	void setupInitialScene();
	~CEditorScene();
	static void _setInstance(CEditorScene* scene) { _instance = scene; }
	static CEditorScene* getInstance() { return _instance; }
	void update(f32 dt);
	void drawAll();
private:

	static CEditorScene* _instance;
	IDevice*					mDevice;
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
};

