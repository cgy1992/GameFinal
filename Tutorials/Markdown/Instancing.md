# Instancing #

This tutorial will tell you how to render many many meshes in the same scene with the help of instancing. Instancing is a new feature that allows developers to render multiple objects in the same call, and thus save the performance. GameFinal supports instancing in a simple way, you can create many instances of the same mesh in the scene and the scene manager will help to manage those instances. In this tutorial, we would plant many trees on the terrain we created in the previous tutorial.

	IModelMesh* treeMesh = meshManager->getModelMesh("roundtreeA.mesh");
	IInstanceCollectionNode* collection = smgr->addInstanceCollectionNode(treeMesh, nullptr, 300, 0);

	for (u32 i = 0; i < 300; i++) {
		addTreeInstance(collection, terrainMesh);
	}

	collection->addShadow(1);

The program first loads the tree mesh and get a `IMeshMesh`. Then we need to to create a `IInstanceCollectionNode` interface. This interface is the root of all the instances and manages those instances under it. In order to create this interface, we call the `ISceneManager::addInstanceCollectionNode`. We pass the tree mesh as its first parameter. Then all the instances under this instance collection would share this mesh. The second parameter is the node's parent, `nullptr` means the scene manager. The third parameter specifies the maximum number of instances under the collection node. The fourth parameter specifies the extra space the engine would allocate for each vertex. Here we set it to 0.

Then we use a `for` loop to add 300 instances of trees to the collection node. The `addTreeInstance` function is defined like this:

	void addTreeInstance(IInstanceCollectionNode* collection, ITerrainMesh* terrain)
	{
		const static f32 range = 200;
		f32 x = math::RandomFloat(-range, range);
		f32 z = math::RandomFloat(-range, range);
		f32 y = terrain->getHeight(x, z);
	
		IInstanceNode* treeNode = collection->addInstance(true, XMFLOAT3(x, y, z));
		treeNode->addShadow(1);
	}

We put the tree at a random position on the terrain. We call the `IInstanceCollectionNode::addInstance` to add the instance into the collection node. The first parameter specifies whether it is static. The second parameter specifies the instance's position.

In addition, we need make a few modifications in the shader code. First, the structure of vertex should be changed:

	struct VertexIn
	{
		float3 PosL		: POSITION;
		float3 Normal	: NORMAL;
		float2 Tex		: TEXCOORD;
		GF_DECLARE_INTANCES_VERTEX   // must add this when using instancing
	};

You may notice that at end of the VertexIn structure, we add a built-in macro `GF_DECLARE_INTANCES_VERTEX`. Then in the vertex shader, we have to replace `GF_WORLD` with the `INSTANCE_WORLD`:

	VertexOut vs_main(VertexIn vin)
	{
		VertexOut vout;
		float4 PosW = mul(float4(vin.PosL, 1.0f), INSTANCE_WORLD);
		vout.Normal = mul(vin.Normal, (float3x3)INSTANCE_WORLD);
		vout.PosH = mul(PosW, GF_VIEW_PROJ);
		vout.PosW = PosW.xyz;
		vout.Tex = vin.Tex;
		return vout;
	}
		 
Furthermore, we have to change the `input-layout` in the pipeline. We must include the positon of each instance , namely the world transformation, into each vertex. We define the pipeline like this:

	<pipeline name="tree_pipeline">
		<input-layout>
			<element semantic="POSITION" format="float3"/>
			<element semantic="NORMAL" format="float3"/>
			<element semantic="TEXCOORD" format="float2"/>
			<element semantic="WORLD" index="0" format="float4" slot="1" instance="true" instance-rate="1"/>
			<element semantic="WORLD" index="1" format="float4" slot="1" instance="true" instance-rate="1"/>
			<element semantic="WORLD" index="2" format="float4" slot="1" instance="true" instance-rate="1"/>
			<element semantic="WORLD" index="3" format="float4" slot="1" instance="true" instance-rate="1"/>
		</input-layout>
		.....
	</pipeline>

After finishing all these modifications, we can run the program now:






