#Camera#

We can set camera's other attributes, including the distance to the near plane or the far plane, field-of-view, etc, by calling the setter method of `ICameraNode` interface. For example:

	camera->setFovAngle(3.1415f / 3);  // set field-of-view to PI/3 (60 degrees) 
	camera->setNearZ(1.0f);  // set the distance to the near clipping plane
	camera->setFarZ(300.0f); // set the distance to the far clipping plane

##Input Device##

In order to let players control a game, we must be able to get access to the computer input devices: mouse and keyboard. The computer input device is abstracted as `IInputDriver` interface, which could be got by calling `IDevice::getInputDriver`:

	IInputDriver* input = device->getInputDriver();

Then we are able to get the states of mouse and keyboard; for example, we check whether we have pressed a specific key. If we pressed ESC key, then the program should exit:
	
	while (device->run()) {
		// if we press ESC, then exit the game loop.
		if (input->keyDown(GVK_ESCAPE))
			break;

		float dt = timer->tick();
		smgr->update(dt);
		driver->beginScene(true, true, color);
		smgr->drawAll();
		driver->endScene();
	}

##Control Camera##

A camera owns three axes (right, up, front) to define its orientations. `ICameraNode` interface provides the methods to rotate around any axis, or move forward or backward along any axis. The following piece of code allows players to move the camera when they press the direction keys on their keyboards:

	while (device->run()) {
		// if we press ESC, then exit the game loop.
		if (input->keyDown(GVK_ESCAPE))
			break;

		float dt = timer->tick();
		
		if (input->isPressed(GVK_UP))
			camera->walk(2.0f * dt); // move forward
		if (input->isPressed(GVK_DOWN))
			camera->walk(-2.0f * dt); // move backward
		if (input->isPressed(GVK_LEFT))
			camera->strafe(-1.0f * dt); // move left
		if (input->isPressed(GVK_RIGHT))
			camera->strafe(1.0f * dt); // move right
		
		smgr->update(dt);
		driver->beginScene(true, true, color);
		smgr->drawAll();
		driver->endScene();
	}
	
##FPS Camera##

GameFinal also provides a FPS camera. FPS Camera is widely used in first person shooting game, and many other first person adventure, acting or RPG games. FPS Camera is represented as a `IFpsCameraNode` interface, instead of using `ISceneManager::addCameraNode` method, we need call `ISceneManager::addFpsCameraNode` to build a fps camera:

	IFpsCameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f),
		XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));

The parameters' meanings are the same with `addCameraNode`; however, this time we create a FPS Camera. You needn't add any extra code to control the camera. Just run the program, and you'll find that you can press 'W','A','S','D' to make movements, move your mouse to change the orientation, and press Left-CTRL to creep and SPACE to jump:

![](https://github.com/woyaofacai/GameFinal/blob/master/Tutorials/img/03-01.png?raw=true)

You can modify the key-action mapping of FPS camera. For example, if you want to use direction keys to make movements instead of using 'W','A','S','D', you can set this before the game loop:

	camera->setActionMapping(EFCA_MOVE_LEFT, GVK_LEFT);
	camera->setActionMapping(EFCA_MOVE_RIGHT, GVK_RIGHT);
	camera->setActionMapping(EFCA_MOVE_FORWARD, GVK_UP);
	camera->setActionMapping(EFCA_MOVE_BACK, GVK_DOWN);


