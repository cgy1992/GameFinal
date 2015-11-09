# Model #

GameFinal has its own mesh format with the suffix `.mesh`. The engine provides a command line tool that enables you to convert other formats to ".mesh" file.

In this demo, we have prepared a obj file,'apartmentB.obj'. Now, let's convert this object file to a ".mesh" file. 
Go to the directory that contains the "MeshConverter.exe", then copy the obj file into this folder (You may need to copy the "mtl" file as well, "apartmentB.mtl").

Open your command window and go to this folder, then press the following command:
	
	MeshConverter.exe "apartmentB.obj" -f "p|n|tex"

The string "apartmentB.obj" is the name of the model file. The string "p|n|tex" following "-f" specifies the vertex's format. "p|n|tex" indicates that each vertex includes position(p), normal vector(n) and texture coordinates(tex). If you want to add another tangent vector to each vertex, you could set the format as "p|n|t|tex".

If the program doesn't show any error message, it means you converted the file successfully:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/05-01.png)

You might have found that except for the mesh file "apartmentB.mesh", several other files were generated, including a shader file "apartmentB.hlsl", a pipeline file "apartmentB.pipeline.xml" and a material file "apartmentB.material.xml". We need copy all these files into the "./media" folder. We also copy texture file "apartmentB_tex.bmp" into the "./media" folder.

The C++ code that sets up the scene is shown here:

	ISceneManager* setupScene(IDevice* device) {
		// create scene manager
		ISceneManager* smgr = device->createSceneManager();
	
		// get mesh manager
		IMeshManager* meshManager = IMeshManager::getInstance();
	
		// set up ground.
		ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10, 50, 50);
		IMeshNode* groundNode = smgr->addMeshNode(groundMesh, "ground_material", nullptr, true);
	
		IModelMesh* apartmentMesh = meshManager->getModelMesh("apartmentB.mesh");
		IMeshNode* apartmentNode = smgr->addModelMeshNode(apartmentMesh, nullptr, true, XMFLOAT3(0, 0, 0));
	
		// add directional light
		ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(5.0f, -5.0f, 2.0f));
		light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
		light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	
		// create a camera node
		ICameraNode* camera = smgr->addCameraNode(1, nullptr, XMFLOAT3(15.0f, 20.0f, -25.0f), 
			XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));
	
		// set ambient in the environment.
		smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	
		return smgr;
	}

In C++ code, we can get a model mesh by calling `IMeshManager::getModelMesh`, then we call the `ISceneManager::addModelMeshNode` to add a model node into the scene at the position (0, 0, 0):

	IModelMesh* apartmentMesh = meshManager->getModelMesh("apartmentB.mesh");
	IMeshNode* apartmentNode = smgr->addModelMeshNode(apartmentMesh, nullptr, true, XMFLOAT3(0, 0, 0));

We run the program, then we have already drawn the model on the screen successfully:

 


