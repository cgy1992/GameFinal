#include "stdafx.h"
#include "ISceneNode.h"
#include "CResourceLoader.h"
#include "CResourceXmlParser.h"
#include "CModelFileParser.h"
#include "gfTypes.h"

namespace gf
{
	

	CResourceLoader::CResourceLoader(IResourceGroupManager* ResourceGroupManager,
		ITextureManager* TextureManager,
		IShaderManager* ShaderManager,
		IInputLayoutManager* InputlayoutManager,
		IRenderStateManager* RenderStateManager,
		IPipelineManager* PipelineManager,
		IMaterialManager* MaterialManager,
		IMeshManager* MeshManager,
		ISamplerManager* SamplerManager)
		:mResourceGroupManager(ResourceGroupManager)
		, mTextureManager(TextureManager)
		, mShaderManager(ShaderManager)
		, mInputlayoutManager(InputlayoutManager)
		, mRenderStateManager(RenderStateManager)
		, mPipelineManager(PipelineManager)
		, mMaterialManager(MaterialManager)
		, mMeshManager(MeshManager)
		, mSamplerManager(SamplerManager)
	{
		mResourceXmlParser = new CResourceXmlParser;
		IResourceXmlParser::_setInstance(mResourceXmlParser);

		mModelFileParser = new CModelFileParser;
	}

	bool CResourceLoader::loadTexture(const std::string& name)
	{
		ITexture* texture = mTextureManager->load(name);
		if (texture == nullptr)
			return false;

		return true;
	}

	IPipeline* CResourceLoader::loadPipeline(const std::string& pipelineName, const std::string& filepath)
	{
		IResourceXmlParser::SPipelineCreateParams createParams;
		if (mResourceXmlParser->parseOnePipeline(filepath, pipelineName, createParams))
		{
			if (loadPipeline(filepath, createParams))
			{
				IPipeline* pipeline = mPipelineManager->get(pipelineName, false);
				return pipeline;
			}
		}
		return nullptr;
	}

	bool CResourceLoader::loadPipeline(const std::string& fullpath, const IResourceXmlParser::SPipelineCreateParams& createParams) const
	{
		/* if the pipeline with the same name has already been loaded,
		maybe some mistake has occurred. such as two pipeline file with the same
		pipeline name. */
		IPipeline* pipeline = mPipelineManager->get(createParams.Name, false);
		if (pipeline)
		{
			GF_PRINT_CONSOLE_INFO("Pipeline '%s' (in the file '%s') has already been loaded. It can't been loaded again. \
								  							  			Do you put the pipelines with same names in different files ?\n",
																		createParams.Name.c_str(), fullpath.c_str());
			return false;
		}


		IShader* shaders[5] = { 0 };
		u32 shaderCount = 0;
		IShader* vertexShader = nullptr;
		for (u32 i = 0; i < EST_SHADER_COUNT; i++)
		{
			const IResourceXmlParser::SShaderCreateParams& shaderCreateParams = createParams.Shaders[i];
			if (!shaderCreateParams.Valid)
				continue;

			std::string shaderName = mShaderManager->makeUpShaderName(shaderCreateParams.FileName, shaderCreateParams.FunctionName);
			IShader* shader = mShaderManager->get(shaderName, shaderCreateParams.Macros, true, shaderCreateParams.Type);

			shaders[shaderCount++] = shader;
			/* find the vertex shader, which will be used to create input-layout soon.*/
			if (shader->getType() == EST_VERTEX_SHADER)
			{
				vertexShader = shader;
			}
		}

		/* create the input-layout. */
		/* if the input-layout with the same layout has been created before, just get it.*/
		IInputLayout* inputLayout = mInputlayoutManager->get(createParams.InputLayoutElements);
		// if there is no input-layout with the same vertex formats. just create it.
		if (!inputLayout)
		{
			inputLayout = mInputlayoutManager->create(createParams.InputLayoutElements, vertexShader);
			if (!inputLayout)
			{
				GF_PRINT_CONSOLE_INFO("Pipeline '%s' creation failed. Due to the input-layout create failure in file '%s'.\n",
					createParams.Name, fullpath.c_str());
				return false;
			}
		}


		/* create render state */
		std::string rsname = createParams.Name + ".rs";
		IRenderState* pRenderState = mRenderStateManager->get(rsname);
		if (!pRenderState)
		{
			pRenderState = mRenderStateManager->create(rsname);
			if (!pRenderState)
			{
				GF_PRINT_CONSOLE_INFO("Pipeline '%s' creation failed. Due to the render-state create failure in file '%s'.\n",
					createParams.Name, fullpath.c_str());
				return false;
			}
		}

		// set all the render states.
		for (u32 i = 0; i < createParams.RenderStates.size(); i++)
		{
			const IResourceXmlParser::SRenderStateCreateParams& rsCreateParam = createParams.RenderStates[i];
			switch (rsCreateParam.StateType)
			{
				/* if the render-state need a float value */
			case ERS_DEPTH_BIAS_CLAMP:
			case ERS_SLOPE_SCALED_DEPTH_BIAS:
				pRenderState->setFloat(rsCreateParam.StateType, rsCreateParam.FloatValue);
				break;
				/* others are unsigned int. */
			default:
				pRenderState->set(rsCreateParam.StateType, rsCreateParam.DwordValue);
				break;
			}
		}

		pRenderState->confirm();

		// create the pipeline object
		pipeline = mPipelineManager->create(createParams.Name, shaders, shaderCount, inputLayout,
			createParams.PrimitiveType, pRenderState);
		if (!pipeline)
		{
			GF_PRINT_CONSOLE_INFO("Pipeline '%s' creation failed (in file '%s').\n",
				createParams.Name, fullpath.c_str());

			// TODO: should drop pRenderState object.
			return false;
		}

		for (u32 i = 0; i < EST_SHADER_COUNT; i++)
		{
			const IResourceXmlParser::SShaderCreateParams& shaderCreateParams = createParams.Shaders[i];
			if (shaderCreateParams.Valid)
			{
				for (u32 j = 0; j < shaderCreateParams.ShaderAutoVariables.size(); j++)
					pipeline->addShaderAutoVariable(shaderCreateParams.ShaderAutoVariables[j]);
			}
		}

		/* create sampler object */
		for (auto it = createParams.SamplerDescs.begin(); it != createParams.SamplerDescs.end(); it++)
		{
			const std::string samplerName = createParams.Name + "-" + it->first;
			ISampler* sampler = mSamplerManager->get(samplerName);
			if (!sampler)
			{
				sampler = mSamplerManager->create(samplerName, it->second);
			}
			pipeline->setSampler(it->first, sampler);
		}

		return true;
	}

	bool CResourceLoader::loadPipelinesFromFile(const std::string& name)
	{

		std::string fullpath;
		if (!mResourceGroupManager->getResourceFullPath(name, fullpath))
		{
			GF_PRINT_CONSOLE_INFO("The pipeline named %s doesn't exist.\n", name);
			return false;
		}

		std::vector<IResourceXmlParser::SPipelineCreateParams> createParamsArray;
		if (!mResourceXmlParser->parsePipelineFile(fullpath, createParamsArray))
			return false;

		for (u32 i = 0; i < createParamsArray.size(); i++)
		{
			loadPipeline(fullpath, createParamsArray[i]);
		}

		return true;
	}

	bool CResourceLoader::loadMaterialsFromFile(const std::string& name)
	{
		std::string fullpath;
		if (!mResourceGroupManager->getResourceFullPath(name, fullpath))
		{
			GF_PRINT_CONSOLE_INFO("The material file named %s doesn't exist.\n", name.c_str());
			return false;
		}

		std::vector<IResourceXmlParser::SMaterialCreateParams> createParamsArray;
		if (!mResourceXmlParser->parseMaterialFile(fullpath, createParamsArray))
			return false;

		for (u32 i = 0; i < createParamsArray.size(); i++)
		{
			loadMaterial(fullpath, createParamsArray[i]);
		}

		return true;
	}

	bool CResourceLoader::loadTexturesFromXml(const std::string& name)
	{
		std::string fullpath;
		if (!mResourceGroupManager->getResourceFullPath(name, fullpath))
		{
			GF_PRINT_CONSOLE_INFO("The texture xml file named %s doesn't exist.\n", name.c_str());
			return false;
		}

		std::vector<IResourceXmlParser::SRenderTargetParams> renderTargetParamsArray;
		std::vector<IResourceXmlParser::SDepthStencilSurfaceParams> depthStencilParamsArray;
		if (!mResourceXmlParser->parseTextureXmlFile(fullpath, renderTargetParamsArray, depthStencilParamsArray))
			return false;

		/*
		for (u32 i = 0; i < renderTargetParamsArray.size(); i++)
		{
			createRenderTarget(fullpath, renderTargetParamsArray[i]);
		}

		for (u32 i = 0; i < depthStencilParamsArray.size(); i++)
		{
			createDepthStencilSurface(fullpath, depthStencilParamsArray[i]);
		}
		*/

		return true;
	}

	bool CResourceLoader::loadMaterial(const std::string& fullpath,
		const IResourceXmlParser::SMaterialCreateParams& createParams) const
	{
		IMaterial* material = mMaterialManager->get(createParams.Name, false);
		if (material)
		{
			GF_PRINT_CONSOLE_INFO("Material '%s' (in the file '%s') has already been loaded. It can't been loaded again. \
								  					Do you put materials with same names in material files ?\n",
													createParams.Name.c_str(), fullpath.c_str());
			return false;
		}

		IPipeline* pipelines[EPU_COUNT] = { 0 };

		for (u32 i = 0; i < createParams.PipelineParams.size(); i++)
		{
			const std::string& pipelineName = createParams.PipelineParams[i].Name;
			E_PIPELINE_USAGE usage = createParams.PipelineParams[i].Usage;

			IPipeline* pipeline = mPipelineManager->get(pipelineName);
			if (!pipeline)
			{
				GF_PRINT_CONSOLE_INFO("The pipeline named '%s' doesn't exist in material '%s' (file location: %s).\n",
					pipelineName.c_str(), createParams.Name.c_str(), fullpath.c_str());
				return false;
			}

			pipelines[usage] = pipeline;
		}

		material = mMaterialManager->create(createParams.Name, pipelines);

		if (!material)
		{
			GF_PRINT_CONSOLE_INFO("The material named '%s' created failed! ( file location: %s ).\n",
				createParams.Name.c_str(), fullpath.c_str());
			return false;
		}

		// set all attributes
		for (u32 i = 0; i < createParams.AttributeParams.size(); i++)
		{
			material->setAttribute(createParams.AttributeParams[i].Name,
				createParams.AttributeParams[i].Value);
		}


		// set all textures
		for (u32 i = 0; i < createParams.TextureParams.size(); i++)
		{
			ITexture* texture = mTextureManager->get(createParams.TextureParams[i].Name);
			if (!texture)
			{
				GF_PRINT_CONSOLE_INFO("Texture '%s' doesn't exist in material '%s'. (file location: %s).\n",
					createParams.TextureParams[i].Name.c_str(), createParams.Name.c_str(), fullpath.c_str());
			}
			else
			{
				material->setTexture(createParams.TextureParams[i].Index, texture);
			}
		}
		return true;
	}

	IMaterial* CResourceLoader::loadMaterial(const std::string& materialName, const std::string& filepath)
	{
		IResourceXmlParser::SMaterialCreateParams createParams;
		if (mResourceXmlParser->parseOneMaterial(filepath, materialName, createParams))
		{
			if (loadMaterial(filepath, createParams))
			{
				return mMaterialManager->get(materialName, false);
			}
		}

		return nullptr;
	}

	bool CResourceLoader::loadMeshFromFile(const std::string& name)
	{
		std::string fullpath;
		if (!mResourceGroupManager->getResourceFullPath(name, fullpath, ERFT_MESH))
		{
			GF_PRINT_CONSOLE_INFO("The mesh named %s doesn't exist.\n", name.c_str());
			return false;
		}

		IModelFileParser::SModelMeshCreateParams createParams;
		if (!mModelFileParser->parseModelFile(fullpath, createParams))
			return false;

		if (mMeshManager->getMesh(name, false))
		{
			GF_PRINT_CONSOLE_INFO("The mesh named '%s' has already existed. \
								  							  Do you have two different mesh files named '%s' ?\n",
															  name.c_str(), name.c_str());
			return false;
		}

		if (!createParams.Header.AnimatedMesh)
		{
			std::vector<IModelMesh::SModelSubset> subsets(createParams.Header.SubsetCount);
			for (u32 i = 0; i < createParams.Header.SubsetCount; i++)
			{
				subsets[i].StartIndexLocation = createParams.Subsets[i].StartIndexLocation;
				subsets[i].IndexCount = createParams.Subsets[i].IndexCount;
				subsets[i].BaseVertexLocation = createParams.Subsets[i].BaseVertexLocation;
				subsets[i].OffsetMatrix = createParams.Subsets[i].OffsetMatrix;

				std::string materialName = createParams.Subsets[i].MaterialName;
				IMaterial* material = mMaterialManager->get(materialName);
				if (!material)
				{
					GF_PRINT_CONSOLE_INFO("FAIL:The material (%s) of model mesh (%s) can't be found.\n",
						materialName.c_str(), name.c_str());

				}

				subsets[i].Material = material;
			}

			IModelMesh* mesh = mMeshManager->createModelMesh(name,
				createParams.Header.VertexElements,
				createParams.VertexBuffer, createParams.IndexBuffer,
				createParams.Header.VertexCount, createParams.Header.VertexStride,
				createParams.Header.IndiceCount, createParams.Header.Aabb, subsets, createParams.Header.Bit32Indice, EMU_STATIC);

			if (!mesh)
				return false;
		}
		else
		{
			/* get subsets */
			std::vector<SAnimatedModelSubset> subsets(createParams.Header.SubsetCount);
			for (u32 i = 0; i < createParams.Header.SubsetCount; i++)
			{
				subsets[i].StartIndexLocation = createParams.Subsets[i].StartIndexLocation;
				subsets[i].IndexCount = createParams.Subsets[i].IndexCount;
				subsets[i].BaseVertexLocation = createParams.Subsets[i].BaseVertexLocation;
				subsets[i].Skinned = createParams.Subsets[i].Skinned;
				subsets[i].Bones = createParams.SubsetBones[i];
				subsets[i].BoneId = createParams.Subsets[i].BoneId;
				std::string materialName = createParams.Subsets[i].MaterialName;
				IMaterial* material = mMaterialManager->get(materialName);
				if (!material)
				{
					GF_PRINT_CONSOLE_INFO("FAIL:The material (%s) of model mesh (%s) can't be found.\n",
						materialName.c_str(), name.c_str());

				}

				subsets[i].Material = material;
			}

			// get bone trees
			const std::vector<SModelBone>& bonesTree = createParams.Bones;

			// get animations
			const std::vector<SBoneAnimationClip>& animateClips = createParams.AnimationClips;

			IAnimatedMesh* mesh = mMeshManager->createAnimatedModelMesh(name,
				createParams.Header.VertexElements,
				createParams.VertexBuffer,
				createParams.AnimateVertexBuffer,
				createParams.IndexBuffer,
				createParams.Header.Bit32Indice,
				createParams.Header.VertexCount,
				createParams.Header.AnimateVertexCount,
				createParams.Header.IndiceCount,
				createParams.Header.VertexStride,
				createParams.Header.AnimateVertexStride,
				createParams.Header.Aabb,
				EMU_STATIC,
				bonesTree,
				subsets,
				animateClips);

			if (!mesh)
				return false;
		}

		return true;
	}

	/*
	bool CResourceLoader::createRenderTarget(const std::string& fullpath, const IResourceXmlParser::SRenderTargetParams& createParams) const
	{
		IRenderTarget* renderTarget = mTextureManager->createRenderTarget(createParams.Name,
			createParams.MultiSampling, createParams.Width, createParams.Height, 
			createParams.Format, createParams.Count, createParams.Quality);

		if (!renderTarget)
		{
			GF_PRINT_CONSOLE_INFO("Texture xml file location: %s\n", fullpath.c_str());
			return false;
		}


		return true;
	}

	bool CResourceLoader::createDepthStencilSurface(const std::string& fullpath, 
		const IResourceXmlParser::SDepthStencilSurfaceParams& createParams) const
	{
		IDepthStencilSurface* surface = mTextureManager->createDepthStencilSurface(createParams.Name,
			createParams.Width, createParams.Height, createParams.DepthBits,
			createParams.StencilBits, createParams.MultiSampling,
			createParams.Count, createParams.Quality, createParams.BindingShader, createParams.BindDepthToShader);

		if (!surface)
		{
			GF_PRINT_CONSOLE_INFO("Texture file location: %s\n", fullpath.c_str());
			return false;
		}

		return true;
	}
	*/
}
