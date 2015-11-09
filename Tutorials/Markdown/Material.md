#Material#

One drawback in previous tuturial is that we hardcoded too much in the program. This tutorial will teach how to define materials in separate xml files.

First, create a new folder called 'media' under our project's folder. We'll put all user-defined resources inside this folder. Then edit the `Resources.cfg` and add the following lines:

	[General]
	FileSystem=./media

We create a new resource group called 'General', and add the path `./media` to this group.

Secondly, we created a new file named `geometry.material.xml` under the `./media` folder. Notice that the file's name must end with the specific suffix, `.material` or `.material.xml`; because the engine determines the category of resources based on the suffix. Meeting this requirement, you can name this file whatever you like.
 
Open 'geometry.material.xml' in any editor, and type in the following XML:

	<materials>
	  <material name="sphere_material">
	    <attributes>
	      <attribute name="ambient" value="0.7f, 0.5f, 0.1f, 1.0f"/>
	      <attribute name="diffuse" value="1.0f, 0.9f, 0.17f, 1.0f"/>
	      <attribute name="specular" value="0.5f, 0.5f, 0.5f, 32.0f"/>
	      <attribute name="emissive" value="0.0f, 0.0f, 0.0f, 1.0f"/>
	    </attributes>
	    <pipelines>
	      <pipeline name="gf/geometry_dirlight"/>
	    </pipelines>
	  </material>
	    
	  <material name="ground_material" prototype="sphere_material">
	    <attributes>
	      <attribute name="ambient" value="0.5f, 0.5f, 0.5f, 1.0f"/>
	      <attribute name="diffuse" value="0.5f, 0.5f, 0.5f, 1.0f"/>
	      <attribute name="specular" value="0.05f, 0.05f, 0.05f, 32.0f"/>
	    </attributes>
	  </material>
	
	  <material name="box_material" prototype="sphere_material">
	    <attributes>
	      <attribute name="ambient" value="0.1f,  0.4f, 0.6f, 1.0f"/>
	      <attribute name="diffuse" value="0.23f, 0.7f, 0.96f, 1.0f"/>
	      <attribute name="specular" value="0.5f, 0.5f, 0.5f, 32.0f"/>
	    </attributes>  
	  </material>
	
	  <material name="teapot_material" prototype="sphere_material">
	    <attributes>
	      <attribute name="ambient" value="0.6f,  0.1f, 0.02f, 1.0f"/>
	      <attribute name="diffuse" value="0.96f, 0.15f, 0.01f, 1.0f"/>
	      <attribute name="specular" value="0.5f, 0.5f, 0.5f, 32.0f"/>
	    </attributes>
	  </material>
	</materials>

In this file, we defined four different materials. In our C++ code, when we want to create the sphere using the first material, we just write the following very concise code:
	
	ISimpleMesh* sphereMesh = meshManager->createSphereMesh("sphere", 1.0f, 100, 100);
	IMeshNode* sphereNode = smgr->addMeshNode(sphereMesh, "sphere_material", nullptr, true, XMFLOAT3(-1.2f, 1.0f, 0));


Instead of passing a `IMaterial*`, this time we pass the material's name ("sphere_material") as the second parameter of `addMeshNode`. The material's name corresponds to the 'name' attribute of `<material>` tag in the material xml file:
	
	<material name="sphere_material">
		<attributes>
		  <attribute name="ambient" value="0.7f, 0.5f, 0.1f, 1.0f"/>
		  <attribute name="diffuse" value="1.0f, 0.9f, 0.17f, 1.0f"/>
		  <attribute name="specular" value="0.5f, 0.5f, 0.5f, 32.0f"/>
		  <attribute name="emissive" value="0.0f, 0.0f, 0.0f, 1.0f"/>
		</attributes>
		<pipelines>
		  <pipeline name="gf/geometry_dirlight"/>
		</pipelines>
	</material>

In the xml file, we specify four attributes for the material: ambient, diffuse, specular, emissive; as well as a pipeline named 'gf/geometry_dirlight'. These attributes' meanings and values are the same as the previous tutorial.

You may notice that in the second material's definition, "ground_material", there seems no emissive attribute as well as pipeline. However, we added another attribute called `prototype` to the `<material>` tag:

	<material name="ground_material" prototype="sphere_material">

We set its prototype to "sphere\_material", which means that this new material would inherit all the attributes from one another material(in this example, it is "sphere_material") unless we modify their values. Except for those inherited values, we could add new attributes to the new material. In our example, we modified its ambient, diffuse and emissive values.

After that, we created the other two materials in XML: "box\_material" and "teapot\_material"

In our C++, except for the sphere, we created a plane, a cube box and a teapot using those materials defined in XML:

	// set up ground.
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, "ground_material", nullptr, true);

	// create a sphere
	ISimpleMesh* sphereMesh = meshManager->createSphereMesh("sphere", 1.0f, 100, 100);
	IMeshNode* sphereNode = smgr->addMeshNode(sphereMesh, "sphere_material", nullptr, true, XMFLOAT3(-1.2f, 1.0f, 0));

	// create a box
	ISimpleMesh* boxMesh = meshManager->createCubeMesh("box", 1.2f, 3.0f, 1.2f);
	IMeshNode* boxNode = smgr->addMeshNode(boxMesh, "box_material", nullptr, true, XMFLOAT3(1.0f, 1.5f, 1.0f));

	// create a teapot
	IModelMesh* teapotMesh = meshManager->getModelMesh("teapot.mesh");
	IMeshNode* teapot = smgr->addModelMeshNode(teapotMesh, nullptr, true, XMFLOAT3(1.2f, 0, -1.0f));
	teapot->setMaterialName("teapot_material");

The we run the program, the final effect is like this:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/05663b210f74139df702d4cd0bf9fc5e01cbb2ef/Tutorials/img/02-01.png)

##Add Textures##
Let's add texture to the ground. Firstly, copy a image file ("floor.dds") to the "./media" folder. Then open and  modify the "geometry.material.xml", add a texture tag to the "ground_material".

	<material name="ground_material" prototype="sphere_material">
		<attributes>
		  <attribute name="ambient" value="0.5f, 0.5f, 0.5f, 1.0f"/>
		  <attribute name="diffuse" value="0.5f, 0.5f, 0.5f, 1.0f"/>
		  <attribute name="specular" value="0.05f, 0.05f, 0.05f, 32.0f"/>
		</attributes>
		<textures>
			<!-- add the texture at the index of 0 -->		  
			<texture name="floor.dds" index="0"></texture>
		</textures>
		<pipelines>
			<pipeline name="gf/geometry_texture_dirlight"/>
    	</pipelines>
	</material>

In addition, we changed the pipeline to another built-in pipeline, `gf/geometry_texture_dirlight`, which applies the specific texture to the object's surface.

Then we change the C++ code that created the plane mesh:

	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10, 50, 50);

The last two parameters of `createPlaneMesh` specify how many times the texture repeats across the plane in u,v directions.

The final effect after applying texture is like this:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/02-02.png)



