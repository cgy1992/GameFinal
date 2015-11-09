#Lighting#

This tutorial will show how to set light sources' attributes, cast shadows, and deferred shading.

## Add Shadow ##

Now, let's add shadows in the scene. If we want a light source to cast shadows, then we need to call the `enableShadow` method of `ILightNode` interface:

	light->enableShadow(true);

Then we decide those objects that will cast shadows and call the `addShadow` method on those objects. In our program, the sphere, cubebox and teapot will all cast shadow:

	sphereNode->addShadow(1);
	boxNode->addShadow(1);
	teapot->addShadow(1);

When calling the `addShadow` method, we pass in the identifier of the light source that will cast shadow.

We also do some optimization by ignoring the shadow that is too far away. We call `setShadowRange` to set a distance value. The shadow that is further than this distance would not be rendered. In our program, we set this value to 100.0f:

	camera->setShadowRange(100.0f);

The materials also need to be modified. All the objects including ground will receive shadow on their surfaces. 

	<material name="sphere_material">
		<attributes>
		  <attribute name="ambient" value="0.7f, 0.5f, 0.1f, 1.0f"/>
		  <attribute name="diffuse" value="1.0f, 0.9f, 0.17f, 1.0f"/>
		  <attribute name="specular" value="0.5f, 0.5f, 0.5f, 32.0f"/>
		  <attribute name="emissive" value="0.0f, 0.0f, 0.0f, 1.0f"/>
		  <attribute name="shadow_softness" value="8.0f"/>
		</attributes>
		<pipelines>
		  <pipeline name="gf/geometry_shadow_dirlight"/>
		  <pipeline name="gf/geometry_shadow_map" usage="shadow_map"/>
		</pipelines>
	</material>

We change the pipeline from "gf/geometry\_dirlight" to "gf/geometry\_shadow\_dirlight". This is a built-in pipeline which will decrease the illumination in light's shadow. We add another pipeline with the name "geometry\_shadow\_map" with the usage "shadow\_map". The pipeline with the "shadow\_map" usage would render the shadow map. Usually, we don't need to change it, just use the built in pipeline("gf/geometry\_shadow\_map"). In addition, we add a new attribute called "shadow\_softness" and set its value to 8.0f. This value would determine how soft the shadow's edge will be. 

Then we change the pipeline in "ground\_material" to "gf/geometry\_texture\_shadow\_dirlight":

	<material name="ground_material" prototype="sphere_material">
		<attributes>
		  <attribute name="ambient" value="0.5f, 0.5f, 0.5f, 1.0f"/>
		  <attribute name="diffuse" value="0.5f, 0.5f, 0.5f, 1.0f"/>
		  <attribute name="specular" value="0.05f, 0.05f, 0.05f, 32.0f"/>
		</attributes>
		<textures>
		  <texture name="floor.dds" index="0"></texture>
		</textures>
		<pipelines>
		  <pipeline name="gf/geometry_texture_shadow_dirlight"/>
		</pipelines>
	</material>

Finally, we run the program and can notice the directional light's shadow:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/04-01.png) 


## Point Light ##

Now, let's try to replace the directional light with a point light. A point light has no unique direction, it emits its energy in a given position to all the directions. We create a point light by calling `ISceneManager::addPointLight` method:

	ILightNode* light = smgr->addPointLight(1, nullptr, false, XMFLOAT3(0, 4.0f, 0), 100);
	light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light->setAttenuation(1.0f, 0.05f, 0);
	light->enableShadow(true);

Notice that instead of specifying a direction, we give it a position (0, 4.0, 0), and set its range to 100. Those pixels that are outside this range would not be illuminated. We call `setAttenuation` to set the point light' attenuation coefficients. These three values specify how the light intensity changes over distance.

Then we change the material file to let objects receive point light's energy.

	<material name="sphere_material">
		<attributes>
			<attribute name="ambient" value="0.7f, 0.5f, 0.1f, 1.0f"/>
			<attribute name="diffuse" value="1.0f, 0.9f, 0.17f, 1.0f"/>
			<attribute name="specular" value="0.5f, 0.5f, 0.5f, 32.0f"/>
			<attribute name="emissive" value="0.0f, 0.0f, 0.0f, 1.0f"/>
			<attribute name="shadow_softness" value="8.0f"/>
		</attributes>
		<pipelines>
			<pipeline name="gf/geometry_shadow_pointlight"/>
			<pipeline name="gf/geometry_shadow_map" usage="shadow_map"/>
			<pipeline name="gf/geometry_point_shadow_map" usage="point_shadow_map"/>
		</pipelines>
	</material>
	<material name="ground_material" prototype="sphere_material">
		<attributes>
			<attribute name="ambient" value="0.5f, 0.5f, 0.5f, 1.0f"/>
			<attribute name="diffuse" value="0.5f, 0.5f, 0.5f, 1.0f"/>
			<attribute name="specular" value="0.05f, 0.05f, 0.05f, 32.0f"/>
		</attributes>
		<textures>
		  	<texture name="floor.dds" index="0"></texture>
		</textures>
		<pipelines>
		  	<pipeline name="gf/geometry_texture_shadow_pointlight"/>
		</pipelines>
	</material>
	
This time, We take a built-in built-in pipeline `geometry_shadow_pointlight`, which renders those objects under a point light and also draws the shadow of the light source. In addition, we add another built-in pipeline named `gf/geometry_point_shadow_map`. This pipeline is reponsible for rendering the point light's shadow map, so we must set the `usage` attribute to `point_shadow_map`.

After running the program, we could find the whole scene is affected by a point light now:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/04-02.png)

## Deferred Shading ##

Let's try to switch to another shading algorithm - deferred shading. If you want to put many light sources in the scene, then you'd better try deferred shading because of its better performance and ease of use.

Now, let's add 100 point lights into the scene, giving them random positions and colors:

	// add 100 point lights
	for (u32 i = 0; i < 100; i++) {
		XMFLOAT3 pos(math::RandomFloat(-20.0, 20.0f), math::RandomFloat(3.0, 10.0f), math::RandomFloat(-20.0, 20.0f));
		XMFLOAT4 diffuse(math::RandomFloat(0, 0.3f), math::RandomFloat(0, 0.3f), math::RandomFloat(0, 0.3f), 1.0f);
		XMFLOAT4 specular(math::RandomFloat(0, 0.3f), math::RandomFloat(0, 0.3f), math::RandomFloat(0, 0.3f), 32.0f);

		ILightNode* light = smgr->addPointLight(i, nullptr, false, pos, 20.0f);
		light->setSpecular(specular);
		light->setDiffuse(diffuse);
		light->setAttenuation(1.0f, 0.05f, 0);
	}

In order to use deferred shading, we need to firstly call `ISceneManager::setDeferredShading` and set its parameter to `true`. 

		driver->setDeferredShading(true);

Then we modify the material file, add another pipeline with the usage of `defer` to each material:

	<material name="sphere_material">
		<attributes>
		  <attribute name="ambient" value="0.7f, 0.5f, 0.1f, 1.0f"/>
		  <attribute name="diffuse" value="1.0f, 0.9f, 0.17f, 1.0f"/>
		  <attribute name="specular" value="0.5f, 0.5f, 0.5f, 32.0f"/>
		  <attribute name="emissive" value="0.0f, 0.0f, 0.0f, 1.0f"/>
		  <attribute name="shadow_softness" value="8.0f"/>
		</attributes>
		<pipelines>
		  <pipeline name="gf/geometry_shadow_pointlight"/>
		  <pipeline name="gf/geometry_shadow_map" usage="shadow_map"/>
		  <pipeline name="gf/geometry_point_shadow_map" usage="point_shadow_map"/>
		  <pipeline name="gf/geometry_defer" usage="defer"/>
		</pipelines>
		</material>
		
		<material name="ground_material" prototype="sphere_material">
		<attributes>
		  <attribute name="ambient" value="0.5f, 0.5f, 0.5f, 1.0f"/>
		  <attribute name="diffuse" value="0.5f, 0.5f, 0.5f, 1.0f"/>
		  <attribute name="specular" value="0.05f, 0.05f, 0.05f, 32.0f"/>
		</attributes>
		<textures>
		  <texture name="floor.dds" index="0"></texture>
		</textures>
		<pipelines>
		  <pipeline name="gf/geometry_texture_shadow_pointlight"/>
		  <pipeline name="gf/geometry_texture_defer" usage="defer"/>
		</pipelines>
	</material>

The pipeline with `defer` usage specifies how to write the objects' information into G-Buffer; here, we set it to `gf/geometry_defer` and `gf/geometry_texture_defer`. Both are built-in pipelines. In this demonstration, we could safely delete all those pipelines except these two "defer" pipelines.

Then run the program, we could found that because there are 100 light sources in total, the whole scene becomes very bright:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/04-03.png)

