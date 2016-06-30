#include "stdafx.h"
#include "RaceScene.h"
#include "CSceneLoader.h"
#include "InputHandler.h"
#include "PhysicsEngine.h"

RaceScene::RaceScene(IApplication* device, IVideoDriver* driver)
:Scene(device, driver)
{
	mDeferredShading = true;
	mGrassLand = nullptr;
	mFreeCamera = nullptr;
	mFollowCamera = nullptr;
	mSideCamera = nullptr;
	mPlayerVehicle = nullptr;
}

RaceScene::~RaceScene()
{
	delete mPlayerVehicle;
}

void RaceScene::Enter()
{
	mFollowCamera = mSceneManager->addFpsCameraNode(2, nullptr, XMFLOAT3(0, 100.0f, -100.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	mFollowCamera->setNearZ(1.0f);
	mFollowCamera->setFarZ(1000.0f);
	mFollowCamera->setShadowRange(300.0f);

	mSideCamera = mSceneManager->addFpsCameraNode(3, nullptr);
	mSideCamera->setNearZ(1.0f);
	mSideCamera->setFarZ(1000.0f);
	mSideCamera->setShadowRange(300.0f);

	mPlayerVehicle = new Vehicle(mSceneManager, mTerrainNode, "carA");

	mSceneManager->setActiveCamera(2);
	
	XMFLOAT3 camPos = mFreeCamera->getPosition();
	mPlayerVehicle->setPosition(camPos);

	mFreeCamera->update(0);
	XMFLOAT3 look = mFreeCamera->getLookVector();
	XMVECTOR look_v = XMVector3Normalize(XMVectorSet(look.x, 0, look.z, 0));
	XMVECTOR neg_x_v = XMVectorSet(-1.0f, 0, 0, 0);
	XMVECTOR axis = XMVector3Normalize(XMVector3Cross(neg_x_v, look_v));
	
	XMVECTOR angle = XMVector3AngleBetweenNormals(neg_x_v, look_v);
	XMVECTOR quat_v = XMQuaternionRotationAxis(axis, XMVectorGetX(angle));
	XMFLOAT4 quat;
	XMStoreFloat4(&quat, quat_v);
	mPlayerVehicle->setRotation(quat);

	mVideoDriver->setDeferredAntiAliasing(true);

	IMeshNode* skyNode = mSceneManager->getSkyNode();
	if (skyNode)
		skyNode->setMaterialName("skydome_material");

	u32 textureWidth = mVideoDriver->getBackBufferWidth() / 2;
	u32 textureHeight = mVideoDriver->getBackBufferHeight() / 2;

	ITextureManager* textureManager = ITextureManager::getInstance();
	mTireTrailTexture = textureManager->createTexture2D("tire_trail", 
		textureWidth,
		textureHeight, 
		ETBT_SHADER_RESOURCE | ETBT_RENDER_TARGET,
		nullptr, 1, EGF_R16_FLOAT);

	mTireTrailDepthSurface = textureManager->createDepthStencilSurface("tire_trail_depth",
		textureWidth,
		textureHeight,
		16, 0, false, 1, 0, false, false);
}

void RaceScene::Update(float dt)
{	
	if (mSceneManager->getActiveCameraNode() == mFreeCamera)
		updateCamera(mFreeCamera, dt);

	if (InputHandler::keyPressed(E_SWITCH_CAMERA))
	{
		if (mSceneManager->getActiveCameraNode() == mFreeCamera)
			mSceneManager->setActiveCamera(mFollowCamera);
		else {
			mFreeCamera->setPosition(mFollowCamera->getPosition());
			mSceneManager->setActiveCamera(mFreeCamera);
		}
	}

	if (mSceneManager->getActiveCameraNode() == mFreeCamera)
	{
		updateCamera(mFreeCamera, dt);
	}
	else
	{
		mPlayerVehicle->update(dt);
	}

	mGrassLand->update(dt);
	//mPlayerVehicle->update(dt);

	PhysicsEngine* engine = PhysicsEngine::getInstance();
	engine->update(dt);

	mSceneManager->update(dt);
}


void RaceScene::Render()
{
	if (mDeferredShading)
	{
		mVideoDriver->setDeferredShading(false);
		//mVideoDriver->setRenderTarget(mTireTrailTexture->getRenderTarget());
		mVideoDriver->setRenderTargetAndDepthStencil(mTireTrailTexture->getRenderTarget(), mTireTrailDepthSurface);
		static const f32 color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		mVideoDriver->clearRenderTarget(color);
		mVideoDriver->clearDepth(1.0f);

		mPlayerVehicle->renderTireTrail();
		mVideoDriver->setDefaultRenderTargetAndDepthStencil();

		IMaterial* terrainMaterial = mTerrainNode->getMaterial();
		terrainMaterial->setTexture(4, mTireTrailTexture);

		mVideoDriver->setDeferredShading(true);
		mSceneManager->drawAll();
		mVideoDriver->setDeferredShading(false);
		//mSceneManager->draw(mSceneManager->getSkyNode());
		mSceneManager->draw(4);
		
	}
	else
	{
		mVideoDriver->setDeferredShading(false);
		mSceneManager->drawAll();
	}
}

void RaceScene::updateCamera(ICameraNode* camera, f32 delta)
{
	const u32 SCREEN_WIDTH = 1280;
	const u32 SCREEN_HEIGHT = 960;
	const f32 CAMERA_MOVE_UNIT = 5.0f;
	const f32 CAMERA_ROTATE_UNIT = 1.0f;

	if (InputHandler::keyPressed(E_MOVE_FORWARD))
	{
		camera->walk(CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_MOVE_BACK))
	{
		camera->walk(-CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_TURE_LEFT))
	{
		camera->strafe(-CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_TURE_RIGHT))
	{
		camera->strafe(CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_MOVE_UP))
	{
		camera->fly(CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_MOVE_DOWN))
	{
		camera->fly(-CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_LOOK_UP))
	{
		camera->pitch(-CAMERA_ROTATE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_LOOK_DOWN))
	{
		camera->pitch(CAMERA_ROTATE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_LOOK_LEFT))
	{
		camera->yaw(-CAMERA_ROTATE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_LOOK_RIGHT))
	{
		camera->yaw(CAMERA_ROTATE_UNIT * delta);
	}
}

void RaceScene::Exit()
{
	mSceneManager->destroy();
	ITextureManager* textureManager = ITextureManager::getInstance();
	textureManager->destroy(mTireTrailDepthSurface->getTexture());
	textureManager->destroy(mTireTrailTexture);
}

