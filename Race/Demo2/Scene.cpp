#include "stdafx.h"
#include "Scene.h"


Scene::Scene(IDevice* device, IVideoDriver* driver)
:mSceneManager(nullptr)
, mDevice(device)
, mVideoDriver(driver)
{

}


Scene::~Scene()
{

}
