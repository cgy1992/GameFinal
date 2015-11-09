#HelloWorld#

Let's look at what we were writing in the previous tutorial.

	int main()
	{
		IDevice* device = createDevice(EDT_DIRECT3D11, 800, 600);
		IVideoDriver* driver = device->getVideoDriver();
		ISceneManager* smgr = device->createSceneManager();
	
		ITimer* timer = device->getTimer();
		timer->reset();
	
		const f32 color[] = { 0, 0, 0, 1.0f };
		while (device->run()) {
			driver->beginScene(true, true, color);
	
			float dt = timer->tick();
			smgr->update(dt);
			smgr->drawAll();
	
			driver->endScene();
		}
	
		smgr->destroy();
		device->drop();
		return 0;
	}

The first function we called is `createDevice` under `gf::` namespace. This method returns a `IDevice` interface. Every GameFinal Application must have a device interface, which maintains the state of the window and other components in the game. The first parameter of this function represents which renderer you want to use. Here we set this to `EDT_DIRECT3D11`, which means we would like DirectX11 renderer. The second and third parameters represent the width and height of the window respectively. 

This function contains other parameters that have default values. For instance, if you're planning to support anti-aliasing, you might want to use multi-sampling in your game like this:

	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;
	IDevice* device = createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
 
The second line in the main function gets a `IVideoDriver` interface, which you can regard as an abstraction of your display device:

	IVideoDriver* driver = device->getVideoDriver();

The third line creates a scene manager, which is represented as a `ISceneManager` interface:
	
	IVideoDriver* driver = device->getVideoDriver();

However, in this program, we put nothing into the scene, so we didn't add anything to this scene manager.

The we get a timer from the `IDevice` interface and reset its value. Remember to reset a timer before the actual game loop begins:

	ITimer* timer = device->getTimer();
    timer->reset();

After all these preparations, the program enters the game loop:

	while (device->run()) {
		float dt = timer->tick();
		smgr->update(dt);
        driver->beginScene(true, true, color);        
        smgr->drawAll();
        driver->endScene();
    }

Inside the game loop, we first call `timer->tick()`. This method would return the elapsed seconds between this calling and previous calling. Then we call `smgr->update(dt)` to update all the objects in the scene. Then we begin  to do the rendering: all the rendering work should be put between `beginScene` and `endScene`. We just call `smgr->drawAll()` to render all the objects in the scene, though in this program there is nothing to render in the scene.

Let's try to modify the value of the third parameter of `beginScene` method:

	const f32 color[] = { 1.0f, 0, 0, 1.0f };

Then re-run the program, the engine now uses red to flush the screen's buffer.
 
##Manage Resources##

Every game has to manager lots of resources. In GameFinal, we don't need to give the full path for every resource file every time you need to load a resource.  

Firstly, we create a new file called `Resources.cfg` under the project's folder. Then copy the `built-in-resources` folder to project's folder. Open this `Resources.cfg` in any text editor and add the following text:

	[BUILT-IN]
 	FileSystem=./built-in-resources

The word inside the bracket gives the name of a `resource group`. All the lines following this word belong to the same group. Here the group is called `BUILT-IN`. Then we add a path to the group(`./built-in-resources`). During the game's running, all the files under this folder could be detected automatically. For example, we can write `teapot.mesh` for short instead of giving the full path `./built-in-resources` now in program.

In code, we use `IResourceGroupManager` interface to manage all the resources:

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

Here we call `init` method to initialize all the resources defined in the `Resources.cgf`. The engine indexes and  maintains those resources. 

##Draw Something##

Let's begin to draw something on the screen. We're going to set up a scene containing a sphere object on the ground.

After creating the scene manager, we add a mesh node representing the ground to the scene manager:

	// create ground's material
	SMaterial groundMaterial;
	groundMaterial.setAttribute("ambient", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	groundMaterial.setAttribute("diffuse", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	groundMaterial.setAttribute("specular", XMFLOAT4(0.05f, 0.05f, 0.05f, 32.0f));
	groundMaterial.setAttribute("emissive", XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	groundMaterial.setPipeline("gf/geometry_dirlight");

	// create ground's mesh
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10);

	// add a mesh node to the scene manager
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, &groundMaterial, nullptr, true);

Firstly, we need to define the ground's material by a `SMaterial` structure. We call the `setAttribute` function to set its four attributes: ambient, diffuse, specular, emissive. These values define how much light would be reflected by the surface. Then we call `setPipeline` to set the pipeline this material would like to use. A pipeline defines the whole process of how the engine would render an object on the screen. Here we set its value as `gf/geometry_dirlight`, which is a built-in pipeline provided by the engine. 

Then we call the `meshManager->createPlaneMesh` to create a plane mesh:

	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10);

The first parameter is the mesh's name. You can name it whatever you like here. The second and third parameters are the length and width of the plane. The fourth and fifth parameters define how many tiles (10 * 10) inside the plane. 

After we create the mesh, we add a node with this mesh into the scene:

	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, &groundMaterial, nullptr, true);

We call the `smgr->addMeshNode` method to create a mesh node. The first parameter represents the mesh that we have created. The material `groundMaterial` that we have created are passed as its second parameter. The third parameter defines the node's parent, `nullptr` means its parent is the scene manager. The fourth parameter defines whether it is a static node. `Static` means the object will never move during the game.

Then we add a sphere object in the same way:

	// create sphere's material
	SMaterial sphereMaterial;
	sphereMaterial.setAttribute("ambient", XMFLOAT4(0.7f, 0.5f, 0.1f, 1.0f));
	sphereMaterial.setAttribute("diffuse", XMFLOAT4(1.0f, 0.9f, 0.17f, 1.0f));
	sphereMaterial.setAttribute("specular", XMFLOAT4(0.5f, 0.5f, 0.5f, 32.0f));
	sphereMaterial.setAttribute("emissive", XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	sphereMaterial.setPipeline("gf/geometry_dirlight");

	ISimpleMesh* sphereMesh = meshManager->createSphereMesh("sphere", 1.0f, 100, 100);
	IMeshNode* sphereNode = smgr->addMeshNode(sphereMesh, &sphereMaterial, nullptr, true, XMFLOAT3(0, 1.0f, 0));

The sphere's radius is set to 1.0f, and we put it in the position of `(0, 1.0f, 0)`; so the sphere touches the ground precisely.

Although we finished adding geometries into scene, it is not enough; because so far we didn't add any light source into the scene. So now we begin creating a directional light:

	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(1.0f, -1.0f, 1.0f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

The first parameter of `addDirectionalLight` is the id of light. Here the light's id is 1. The direction of the light is given as `(5.0f, -5.0f, 2.0f)`. We also call `setSpecular` and `setDiffuse` method to set the light's diffuse and specular color. 

Usually, we want to add some environmental light. We can do this by calling `setAmbient` method:
	
	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

The last thing is to add a camera which gives the position and orientation of the player's eyes.

	ICameraNode* camera = smgr->addCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f), 
		XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));

The first parameter is the camera's id. Here we put the camera in the position of `(0, 1.0f, -6.0)`. The target position that the camera looks at is given as `(0, 1.0f, 0)`. The ups vector of the camera is `(0, 1, 0)`.

Finally, we compile and run our program, we could see the window:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/199694ecea56b119e69d5c18e625473fc3b0a7b8/Tutorials/img/01-01.png)

The entire code is given at last:)
	
	#include <GameFinal.h>
	
	#pragma comment(lib, "GameFinal.lib")
	#pragma comment(lib, "winmm.lib")
	
	using namespace gf;
	
	int main()
	{
		IDevice* device = createDevice(EDT_DIRECT3D11, 800, 600);
		IVideoDriver* driver = device->getVideoDriver();
		
		ISceneManager* smgr = device->createSceneManager();
		
		IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
		resourceGroupManager->init("Resources.cfg");
	
		// get mesh manager.
		IMeshManager* meshManager = driver->getMeshManager();
	
		// set up ground.
		// create ground's material
		SMaterial groundMaterial;
		groundMaterial.setAttribute("ambient", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		groundMaterial.setAttribute("diffuse", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		groundMaterial.setAttribute("specular", XMFLOAT4(0.05f, 0.05f, 0.05f, 32.0f));
		groundMaterial.setAttribute("emissive", XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		groundMaterial.setPipeline("gf/geometry_dirlight");
	
		// create ground's mesh
		ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10);
	
		// add a mesh node to the scene manager
		IMeshNode* groundNode = smgr->addMeshNode(groundMesh, &groundMaterial, nullptr, true);
	
		// create sphere's material
		SMaterial sphereMaterial;
		sphereMaterial.setAttribute("ambient", XMFLOAT4(0.7f, 0.5f, 0.1f, 1.0f));
		sphereMaterial.setAttribute("diffuse", XMFLOAT4(1.0f, 0.9f, 0.17f, 1.0f));
		sphereMaterial.setAttribute("specular", XMFLOAT4(0.5f, 0.5f, 0.5f, 32.0f));
		sphereMaterial.setAttribute("emissive", XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		sphereMaterial.setPipeline("gf/geometry_dirlight");
	
		ISimpleMesh* sphereMesh = meshManager->createSphereMesh("sphere", 1.0f, 100, 100);
		IMeshNode* sphereNode = smgr->addMeshNode(sphereMesh, &sphereMaterial, nullptr, true, 
							XMFLOAT3(0, 1.0f, 0));
	
		// add directional light
		ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(5.0f, -5.0f, 2.0f));
		light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
		light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	
		// set scene's environmental light 
		smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	
		// create camera
		ICameraNode* camera = smgr->addCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f), 
			XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));
	
		ITimer* timer = device->getTimer();
		timer->reset();
	
		const f32 color[] = { 0, 0, 0, 1.0f };
		while (device->run()) {
			
			float dt = timer->tick();
			smgr->update(dt);
			driver->beginScene(true, true, color);
			smgr->drawAll();
			driver->endScene();
		}
	
		smgr->destroy();
		device->drop();
	
		return 0;
	}



