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

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/07-01.png)

If you have added a FPS camera in the scene, you'll realize that you can walk on the terrain very smoothly without writing any other controlling code.

## Define Terrain Material ##

The built-in terrain material `gf/terrain_material`. If you want to define your own terrain material, you can inherit the built-in material every easily and modify some attributes in it.

Now, we add create a new material called "sand_terrain_material" like this:

	<materials>
	  <material name="sand_terrain_material" prototype="gf/terrain_material">
	    <attributes>
	      <attribute name="ambient" value="0.3f, 0.3f, 0.3f, 1.0f"/>
	      <attribute name="diffuse" value="1.0f, 1.0f, 0.8f, 1.0f"/>
	    </attributes>
	    <textures>
	      <texture name="ground.jpg" index="0"/>
	    </textures>
	  </material>
	</materials>

Here we set its prototype to "gf/terrain_material", which means we will inherit all attributes from the built-in material. Then we modify the ambient and diffuse color as well as the texture. Then remember to change the material in our program:

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh, "sand_terrain_material");

Then run the program, we could find the change:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/07-02.png)

## Tessellated Terrain ##

DirectX11 supports tessellation on the GPU, which allows you to save more space and performance while rendering terrain. In GameFinal, you can render a tessellated terrain very easily just by setting a few parameters.

When creating the terrain mesh, we have to create a tessellated mesh. Notice that this time we set its fifth parameter to "true":

	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", "terrain-heightmap.raw",
		3.0f, 0.3f, true, false, 50.0f);

Then we have to change its material to `gf/tess_terrain_material`, which is another built-in material applying for tessellated terrain:

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh, "gf/tess_terrain_material"); 

Remember that you can also define your own material by inheriting from the `gf/tess_terrain_material".

## Add Vegetation ##

So far we have created a broad but barren land. Now let's add some a layer of grass on the terrain.

To achieve this, just add one line of code:

	terrainNode->addGrassLand(5000 * 64, 10.0f, 10.0f, "gf/grassland_material");

The first parameter is the total number of blades you want to plant on the terrain. The second the third parameters specify the width and height of each blade. The fourth specifies the material you want to use. Here it is a built-in material, `gf/grassland_material`. This method also has three other parameters, by which you can specify the pitch numbers on the terrain, the amplitude of the blades and the wind's direction.

Run the program, we could notice the grass blades are waving in the wind:



