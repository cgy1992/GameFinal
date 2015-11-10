# Shader #

So far, we have only tried those built-in pipelines. In this tutorial, we are going to write our own shaders and pipelines to realize more complicated effects.

## Pipeline definition #

A pipeline is represented by a `IPipeline` interface in GameFinal; however, you'll rarelly use this interface directly in your development; because a pipeline can also be defined in a xml file separately. A typical pipeline's definition looks like this:

	<pipeline name="xxx">
		<primitive-type value="..."/>
		<input-layout>
			<element semantic="POSITION" format="float3"/>
			...
		</input-layout>
		<shaders>
			<vertex-shader file="xxx.hlsl" main="VS">...</vertex-shader>
			<hull-shader file="xxx.hlsl" main="HS">...</hull-shader>
			<domain-shader file="xxx.hlsl" main="DS">...</domain-shader>
			<geometry-shader file="xxx.hlsl" main="GS">...</geometry-shader>
			<pixel-shader file="xxx.hlsl" main="PS">...</pixel-shader>
		</shaders>
		<render-states>
			...
		</render-states>
		<samplers>
			....
		<samplers>
	</pipeline>
	
The pipeline file' name must be ended with `.pipeline` or `pipeline.xml`. A pipeline must have a name, which is its identifier in the program, it also include a few sub-tags:

* **primitive-type**: the type of the primitive, if we don't include this tag explicitly, its default type is triangle list.
* **input-layout**: defines the format of each vertex and includes several `<element>` tag.
* **shaders**: include several different sub-tag, which defines different stage of the rendering process.
* **render-states**: include different render state settings here, each of them is represented as a `<render-state` sub-tag.
* **sampler**: include the texture samplers that might be used in shaders.

## First Shader ##

Let's write our first shader, to apply a normal map to the ground. Normal map can make the objects look more realistic, especially those under specular lighting.

Firstly, create a shader file named `first_shader.hlsl` under `./media` folder, and type in the following code:

	cbuffer cbPerFrame
	{
		SPointLight gPointLight;
	};
	
	SamplerState gSampleState;
	
	Texture2D gTexture;
	Texture2D gNormalMap;
	
	struct VertexIn
	{
		float3 PosL		: POSITION;
		float3 Normal	: NORMAL;
		float3 Tangent	: TANGENT;
		float2 Tex		: TEXCOORD;
	};
	
	struct VertexOut
	{
		float4 PosH		: SV_POSITION;
		float3 PosW		: POSITION;
		float3 Normal	: NORMAL; 
		float3 Tangent  : TANGENT;
		float2 Tex		: TEXCOORD;
	};
	
	VertexOut vs_main(VertexIn vin)
	{
		VertexOut vout;
		float4 PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
		vout.PosH = mul(PosW, GF_VIEW_PROJ);
		vout.PosW = PosW.xyz;
		vout.Normal = mul(vin.Normal, (float3x3)GF_WORLD);
		vout.Tangent = mul(vin.Tangent, (float3x3)GF_WORLD);
		vout.Tex = vin.Tex;
		return vout;
	}
	
	float4 ground_ps_main(VertexOut pin) : SV_TARGET
	{	
		float3 normal = normalize(pin.Normal);
		float3 normalMapSample = gNormalMap.Sample(gSampleState, pin.Tex);
		// sample the normal map and create the new normal vector
		normal = NormalSampleToWorldSpace(normalMapSample, normal, pin.Tangent);
	
		float4 diffuse;
		float4 specular;
		float4 texColor = gTexture.Sample(gSampleState, pin.Tex);
	
		float3 lightDir;
		ComputeIrradianceOfPointLight(pin.PosW, gPointLight, lightDir, diffuse, specular);
		PhoneShading(pin.PosW, lightDir, normal, diffuse, specular, GF_MTRL_SPECULAR.w);
	
		float shadowFactor = CalcPointLightShadowFactor(1, gPointLight.Position, GF_SHADOW_SOFTNESS);
		return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE + 
			diffuse * GF_MTRL_DIFFUSE * texColor * shadowFactor + 
			specular * GF_MTRL_SPECULAR * shadowFactor;
	}

The shader file contains two functions: `vs_main` and `ground_ps_main`, which are the entry points of vertex shader and pixel shader respectively. Notice that in this file, there are some variables appeared that we didn't declare, such as `GF_WORLD` and `GF_MTRL_DIFFUSE`. These are the built-in variables and will be set by the engine automatically during the program's running. `GF_WORLD` means the matrix of world transformation, `GF_VIEW_PROJ` means the combination of view transformation and projection transformation.  `GF_MTRL_AMBIENT`, `GF_MTRL_DIFFUSE`, `GF_MTRL_EMISSIVE`, `GF_MTRL_SPECULAR` and `GF_SHADOW_SOFTNESS` are the attributes defined in material file: "ambient", "diffuse", "emissive", "specular" and "shadow_softness" respectively.

We call some built-in functions that the engine provides for us. The `ComputeIrradianceOfPointLight` could compute the irradiance at a specific point under a point light. Then we call `PhoneShading` function to apply Phong shading model to this pixel. In order to determine whether the pixel is in the shadow, we call `CalcPointLightShadowFactor` whose first parameter is the id of the point light, second parameter is the light's position and third parameter is the "shadow_softness". 

Then we create a pipeline file named `first.pipeline.xml` under the `./media` folder, you can name it whatever you like, which does not matter in development. In this file, you define a new pipeline:

	<pipelines>
	  <pipeline name="ground_pipeline">
	    <input-layout>
	      <element semantic="POSITION" format="float3"></element>
	      <element semantic="NORMAL" format="float3"></element>
	      <element semantic="TANGENT" format="float3"></element>
	      <element semantic="TEXCOORD" format="float2"></element>
	    </input-layout>
	    <shaders>
	      <vertex-shader file="first_shader.hlsl" main="vs_main"/>
	      <pixel-shader file="first_shader.hlsl" main="ground_ps_main">
	        <variable name="gPointLight" value="light" index="1"/>
	        <variable name="gTexture" value="texture" index="0"/>
	        <variable name="gNormalMap" value="texture" index="1"/>
	      </pixel-shader>
	    </shaders>
	    <samplers>
	      <sampler name="gSampleState"
	                 addressU="wrap"
	                 addressV="wrap"
	                 filter="MIN_MAG_MIP_LINEAR"
	                 border-color="0.0f, 0.0f, 0.0f, 0.0f"/>
	    </samplers>
	  </pipeline>
	</pipelines>

You can see that the new pipeline's name is "ground\_pipeline". We specify both vertex-shader and pixel-shader. The "file" attribute in `vertex-shader` and `pixel-shader` tag specifies the shader file's name, and the "main" attribute specifies the name of entry point function. Under the "pixel\_shader" tag, we add three variables which all come up in our shader file. "gPointLight" is a light source (specified by the "value" attribute) and its id is 1 ("index" attribute). Then we declare "gTexture" is a texture which at index 0 in the material and "gNormalMap" is a texture at index 1. Besides these, we finally define a texture sampler named "gSampleState".

Next, we open the material file and modify the ground's material:

	  <material name="ground_material" prototype="sphere_material">
	    <attributes>
	      <attribute name="ambient" value="0.5f, 0.5f, 0.5f, 1.0f"/>
	      <attribute name="diffuse" value="0.5f, 0.5f, 0.5f, 1.0f"/>
	      <attribute name="specular" value="1.0f, 1.0f, 1.0f, 32.0f"/>
	    </attributes>
	    <textures>
	      <texture name="floor.dds" index="0"/>
	      <texture name="floor_NRM.png" index="1"/>
	    </textures>
	    <pipelines>
	      <pipeline name="ground_pipeline"/>
	    </pipelines>
	  </material>
 
We change pipeline to "ground_pipeline" which is defined by ourselves previously. We put the texture "floor.dds" at index 0 and "floor_NRM.png" at index 1, which keep consistent with the corresponding variables declared in pipeline. At last, don't forget copy the normal map image to "./media" directory. Run this program, we can see the ground looks more realistic under specular lighting now:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/06-01.png)

## Environment Mapping ##

The second example is about environment mapping. GameFinal allows us to set a sky dome by calling `ISceneManager::setSkyDome` method. Usually, it receives a cube map's file name as its parameter:

	smgr->setSkyDome("Snow.dds");

"Snow.dds" is a cube map that we have prepared and is put into "./media" folder.

Let's add another function in the `first_shader.hlsl` as the three objects' pixel shader. They can share the same vertex shader with the ground. So we don't need to write a separate one for them:

	float4 object_ps_main(VertexOut pin) : SV_TARGET
	{
		float3 normal = normalize(pin.Normal);
		float4 diffuse;
		float4 specular;
	
		float3 viewDir = normalize(pin.PosW - GF_CAMERA_POS);
		float3 reflectDir = normalize(reflect(viewDir, normal));
		float4 texColor = GF_SKY_TEXTURE.Sample(gSampleState, reflectDir);
	
		float3 lightDir;
		ComputeIrradianceOfPointLight(pin.PosW, gPointLight, lightDir, diffuse, specular);
	
		PhoneShading(pin.PosW, lightDir, normal, diffuse, specular, GF_MTRL_SPECULAR.w);
	
		float shadowFactor = CalcPointLightShadowFactor(1, gPointLight.Position, GF_SHADOW_SOFTNESS);
		return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE + 
			diffuse * GF_MTRL_DIFFUSE * texColor * shadowFactor + 
			specular * GF_MTRL_SPECULAR * shadowFactor;
	}

In this pixel shader code, we sample the sky dome's texture, which is represented by `GF_SKY_TEXTURE`. It is also a built-in variable and will be set automatically during the game running. 

Then we add another pipeline in `first.pipeline.xml`:

	<pipeline name="static_object_pipeline" prototype="ground_pipeline">
		<shaders>
			<pixel-shader main="object_ps_main"/>
		</shaders>
	</pipeline>

Notice that we specify a `prototype` attribute here, which has the similar meaning in material definition. In this example, it will inherit nearly all the attributes from `ground_pipeline`, which we have defined previously. The only thing we need to change is the pixel shader's entry point.

Remember to change the material file:
	
	<material name="sphere_material">
		<attributes>
		  ...
		</attributes>
		<pipelines>
			<!--<pipeline name="gf/geometry_shadow_pointlight"/> -->
			<pipeline name="static_object_pipeline"/>
			<pipeline name="gf/geometry_shadow_map" usage="shadow_map"/>
			<pipeline name="gf/geometry_point_shadow_map" usage="point_shadow_map"/>
		</pipelines>
	</material>

Here we change the pipeline from a built-in one to `static_object_pipeline`, which has been defined in the previous step. Now, start running the program:





