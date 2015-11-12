# Terrain #

Terrain is widely used in many different kinds of video games as well as other 3D applications. This tutorial will tell you how to use the built-in terrain system in GameFinal in game applications. With the help of GameFinal engine, you can render a realistic landscape in your game only with a few lines of code.

First of all, we need to prepare a height map file in `.raw` format. Each pixel's value in this heigh map corresponds to a height on the landscape. Then we need to create a `ITerrainMesh` by calling the `IMeshManager::createTerrainMesh` method.

	// create terrain mesh
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", "terrain-heightmap.raw",
		3.0f, 0.2f, false, true, 50.0f);

There are many parameters: the first parameter is an unique mesh name in the application. The second parameter is the height map file's name. The third one is the horizontal distance between two neighbouring vertexs in the mesh. The fourth parameter is the height scale; the default range in the height map is 0-255, we can change the range by this parameter. The fifth figures out whether you will use tessellation in the mesh. The sixth parameter specifies whether you would like to create normal vector for each vertex in the mesh. The last parameter is the texcoord scale: here we set its value to 50.0f, which means the range of u,v is 0.0f-50.0f.

After we create the terrain mesh, in order to show the terrain in the scene, we have to add a terrain node to the scene manager by calling `ISceneManager::addTerrainNode`:

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh, "gf/terrain_material");

The first parameter specifies the terrain mesh that we have created previously. The second parameter is the material that we will use to render the terrain; here we set it to `gf/terrain_material`, which is a built-in material.

	


