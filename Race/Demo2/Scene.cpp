#include "stdafx.h"
#include "Scene.h"


Scene::Scene(IApplication* device, IVideoDriver* driver)
:mSceneManager(nullptr)
, mDevice(device)
, mVideoDriver(driver)
{

}


Scene::~Scene()
{

}
