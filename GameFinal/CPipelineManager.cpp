#include "stdafx.h"
#include "CPipelineManager.h"
#include "CPipeline.h"

namespace gf
{


	CPipelineManager::CPipelineManager(IResourceFactory* pResourceFactory)
		:mResourceFactory(pResourceFactory)
	{
		mBuiltInVariables["GF_WORLD"] = SShaderVariableAttribute(ESAVT_WORLD_MATRIX);
		mBuiltInVariables["GF_INV_WORLD"] = SShaderVariableAttribute(ESAVT_INVERSE_WORLD_MATRIX);
		mBuiltInVariables["GF_TRAN_WORLD"] = SShaderVariableAttribute(ESAVT_TRANSPOSE_WORLD_MATRIX);
		mBuiltInVariables["GF_INV_TRAN_WORLD"] = SShaderVariableAttribute(ESAVT_INVERSE_TRANSPOSE_WORLD_MATRIX);
		mBuiltInVariables["GF_WORLD_VIEW"] = SShaderVariableAttribute(ESAVT_WORLD_VIEW_MATRIX);
		mBuiltInVariables["GF_INV_WORLD_VIEW"] = SShaderVariableAttribute(ESAVT_INVERSE_WORLD_VIEW_MATRIX);
		mBuiltInVariables["GF_TRAN_WORLD_VIEW"] = SShaderVariableAttribute(ESAVT_TRANSPOSE_WORLD_VIEW_MATRIX);
		mBuiltInVariables["GF_INV_TRAN_WORLD_VIEW"] = SShaderVariableAttribute(ESAVT_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX);
		mBuiltInVariables["GF_WVP"] = SShaderVariableAttribute(ESAVT_WORLD_VIEW_PROJ_MATRIX);
		mBuiltInVariables["GF_INV_WVP"] = SShaderVariableAttribute(ESAVT_INVERSE_WORLD_VIEW_PROJ_MATRIX);
		mBuiltInVariables["GF_TRAN_WVP"] = SShaderVariableAttribute(ESAVT_TRANSPOSE_WORLD_VIEW_PROJ_MATRIX);
		mBuiltInVariables["GF_INV_TRAN_WVP"] = SShaderVariableAttribute(ESAVT_INVERSE_TRANSPOSE_WORLD_VIEW_PROJ_MATRIX);

		mBuiltInVariables["GF_VIEW"] = SShaderVariableAttribute(ESAVT_VIEW_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_INV_VIEW"] = SShaderVariableAttribute(ESAVT_INVERSE_VIEW_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_TRAN_VIEW"] = SShaderVariableAttribute(ESAVT_TRANSPOSE_VIEW_MATRIX, EUF_PER_FRAME); 
		mBuiltInVariables["GF_INV_TRAN_VIEW"] = SShaderVariableAttribute(ESAVT_INVERSE_TRANSPOSE_VIEW_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_PROJ"] = SShaderVariableAttribute(ESAVT_PROJ_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_INV_PROJ"] = SShaderVariableAttribute(ESAVT_INVERSE_PROJ_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_TRAN_PROJ"] = SShaderVariableAttribute(ESAVT_TRANSPOSE_PROJ_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_INV_TRAN_PROJ"] = SShaderVariableAttribute(ESAVT_INVERSE_TRANSPOSE_PROJ_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_VIEW_PROJ"] = SShaderVariableAttribute(ESAVT_VIEW_PROJ_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_INV_VIEW_PROJ"] = SShaderVariableAttribute(ESAVT_INVERSE_VIEW_PROJ_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_TRAN_VIEW_PROJ"] = SShaderVariableAttribute(ESAVT_TRANSPOSE_VIEW_PROJ_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_INV_TRAN_VIEW_PROJ"] = SShaderVariableAttribute(ESAVT_INVERSE_TRANSPOSE_VIEW_PROJ_MATRIX, EUF_PER_FRAME);
		mBuiltInVariables["GF_CAMERA_POS"] = SShaderVariableAttribute(ESAVT_CAMERA_POSITION, EUF_PER_FRAME);
		mBuiltInVariables["GF_CAMERA_SEGMENTS"] = SShaderVariableAttribute(ESAVT_CAMERA_SEGMENTS, EUF_PER_FRAME);
		mBuiltInVariables["GF_AMBIENT"] = SShaderVariableAttribute(ESAVT_AMBIENT, EUF_PER_FRAME);


		mBuiltInVariables["GF_MTRL_AMBIENT"] = SShaderVariableAttribute(ESAVT_MATERIAL_AMBIENT, EUF_PER_OBJECT);
		mBuiltInVariables["GF_MTRL_DIFFUSE"] = SShaderVariableAttribute(ESAVT_MATERIAL_DIFFUSE, EUF_PER_OBJECT);
		mBuiltInVariables["GF_MTRL_SPECULAR"] = SShaderVariableAttribute(ESAVT_MATERIAL_SPECULAR, EUF_PER_OBJECT);
		mBuiltInVariables["GF_MTRL_EMISSIVE"] = SShaderVariableAttribute(ESAVT_MATERIAL_EMISSIVE, EUF_PER_OBJECT);

		mBuiltInVariables["GF_SCREEN_SIZE"] = SShaderVariableAttribute(ESAVT_SCREEN_SIZE, EUF_PER_OBJECT);

		mBuiltInVariables["GF_POINT_LIGHTS"] = SShaderVariableAttribute(ESAVT_NEAR_POINT_LIGHTS, EUF_PER_OBJECT);
		mBuiltInVariables["GF_POINT_LIGHTS_NUM"] = SShaderVariableAttribute(ESAVT_NEAR_POINT_LIGHTS_NUM, EUF_PER_OBJECT);
		mBuiltInVariables["GF_DIR_LIGHTS"] = SShaderVariableAttribute(ESAVT_DIRECTIONAL_LIGHTS, EUF_PER_FRAME);
		mBuiltInVariables["GF_DIR_LIGHTS_NUM"] = SShaderVariableAttribute(ESAVT_DIRECTIONAL_LIGHTS_NUM, EUF_PER_FRAME);
		mBuiltInVariables["GF_SPOT_LIGHTS"] = SShaderVariableAttribute(ESAVT_NEAR_SPOT_LIGHTS, EUF_PER_OBJECT);
		mBuiltInVariables["GF_SPOT_LIGHTS_NUM"] = SShaderVariableAttribute(ESAVT_NEAR_SPOT_LIGHTS_NUM, EUF_PER_OBJECT);

		mBuiltInVariables["GF_TEXTURE_0"] = SShaderVariableAttribute(ESAVT_TEXTURE, EUF_PER_OBJECT, 0);
		mBuiltInVariables["GF_TEXTURE_1"] = SShaderVariableAttribute(ESAVT_TEXTURE, EUF_PER_OBJECT, 1);
		mBuiltInVariables["GF_TEXTURE_2"] = SShaderVariableAttribute(ESAVT_TEXTURE, EUF_PER_OBJECT, 2);
		mBuiltInVariables["GF_TEXTURE_3"] = SShaderVariableAttribute(ESAVT_TEXTURE, EUF_PER_OBJECT, 3);
		mBuiltInVariables["GF_TEXTURE_4"] = SShaderVariableAttribute(ESAVT_TEXTURE, EUF_PER_OBJECT, 4);
		mBuiltInVariables["GF_TEXTURE_5"] = SShaderVariableAttribute(ESAVT_TEXTURE, EUF_PER_OBJECT, 5);
		mBuiltInVariables["GF_TEXTURE_6"] = SShaderVariableAttribute(ESAVT_TEXTURE, EUF_PER_OBJECT, 6);
		mBuiltInVariables["GF_TEXTURE_7"] = SShaderVariableAttribute(ESAVT_TEXTURE, EUF_PER_OBJECT, 7);

		mBuiltInVariables["GF_SHADOW_MAP_0"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 0);
		mBuiltInVariables["GF_SHADOW_MAP_1"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 1);
		mBuiltInVariables["GF_SHADOW_MAP_2"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 2);
		mBuiltInVariables["GF_SHADOW_MAP_3"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 3);
		mBuiltInVariables["GF_SHADOW_MAP_4"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 4);
		mBuiltInVariables["GF_SHADOW_MAP_5"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 5);
		mBuiltInVariables["GF_SHADOW_MAP_6"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 6);
		mBuiltInVariables["GF_SHADOW_MAP_7"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 7);
		
		mBuiltInVariables["GF_PL_SHADOW_MAP_0"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 0);
		mBuiltInVariables["GF_PL_SHADOW_MAP_1"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 1);
		mBuiltInVariables["GF_PL_SHADOW_MAP_2"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 2);
		mBuiltInVariables["GF_PL_SHADOW_MAP_3"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 3);
		mBuiltInVariables["GF_PL_SHADOW_MAP_4"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 4);
		mBuiltInVariables["GF_PL_SHADOW_MAP_5"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 5);
		mBuiltInVariables["GF_PL_SHADOW_MAP_6"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 6);
		mBuiltInVariables["GF_PL_SHADOW_MAP_7"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 7);
		mBuiltInVariables["GF_PL_SHADOW_MAP_8"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 8);
		mBuiltInVariables["GF_PL_SHADOW_MAP_9"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 9);
		mBuiltInVariables["GF_PL_SHADOW_MAP_10"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 10);
		mBuiltInVariables["GF_PL_SHADOW_MAP_11"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 11);
		mBuiltInVariables["GF_PL_SHADOW_MAP_12"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 12);
		mBuiltInVariables["GF_PL_SHADOW_MAP_13"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 13);
		mBuiltInVariables["GF_PL_SHADOW_MAP_14"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 14);
		mBuiltInVariables["GF_PL_SHADOW_MAP_15"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 15);

		mBuiltInVariables["GF_SHADOW_MAP_TRANSFORM_0"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORM, EUF_PER_FRAME, 0);
		mBuiltInVariables["GF_SHADOW_MAP_TRANSFORM_1"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORM, EUF_PER_FRAME, 1);
		mBuiltInVariables["GF_SHADOW_MAP_TRANSFORM_2"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORM, EUF_PER_FRAME, 2);
		mBuiltInVariables["GF_SHADOW_MAP_TRANSFORM_3"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORM, EUF_PER_FRAME, 3);
		mBuiltInVariables["GF_SHADOW_MAP_TRANSFORM_4"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORM, EUF_PER_FRAME, 4);
		mBuiltInVariables["GF_SHADOW_MAP_TRANSFORM_5"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORM, EUF_PER_FRAME, 5);
		mBuiltInVariables["GF_SHADOW_MAP_TRANSFORM_6"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORM, EUF_PER_FRAME, 6);
		mBuiltInVariables["GF_SHADOW_MAP_TRANSFORM_7"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORM, EUF_PER_FRAME, 7);

		mBuiltInVariables["GF_SHADOW_MAP_SIZE_0"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_SIZE, EUF_PER_FRAME, 0);
		mBuiltInVariables["GF_SHADOW_MAP_SIZE_1"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_SIZE, EUF_PER_FRAME, 1);
		mBuiltInVariables["GF_SHADOW_MAP_SIZE_2"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_SIZE, EUF_PER_FRAME, 2);
		mBuiltInVariables["GF_SHADOW_MAP_SIZE_3"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_SIZE, EUF_PER_FRAME, 3);
		mBuiltInVariables["GF_SHADOW_MAP_SIZE_4"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_SIZE, EUF_PER_FRAME, 4);
		mBuiltInVariables["GF_SHADOW_MAP_SIZE_5"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_SIZE, EUF_PER_FRAME, 5);
		mBuiltInVariables["GF_SHADOW_MAP_SIZE_6"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_SIZE, EUF_PER_FRAME, 6);
		mBuiltInVariables["GF_SHADOW_MAP_SIZE_7"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_SIZE, EUF_PER_FRAME, 7);

		mBuiltInVariables["GF_DIR_SHADOW_MAP_0"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 0);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_1"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 1);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_2"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 2);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_3"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 3);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_4"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 4);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_5"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 5);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_6"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 6);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_7"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP, EUF_PER_FRAME, 7);

		mBuiltInVariables["GF_DIR_SHADOW_MAP_TRANSFORMS_0"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORMS, EUF_PER_FRAME, 0);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_TRANSFORMS_1"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORMS, EUF_PER_FRAME, 1);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_TRANSFORMS_2"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORMS, EUF_PER_FRAME, 2);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_TRANSFORMS_3"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORMS, EUF_PER_FRAME, 3);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_TRANSFORMS_4"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORMS, EUF_PER_FRAME, 4);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_TRANSFORMS_5"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORMS, EUF_PER_FRAME, 5);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_TRANSFORMS_6"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORMS, EUF_PER_FRAME, 6);
		mBuiltInVariables["GF_DIR_SHADOW_MAP_TRANSFORMS_7"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_TRANSFORMS, EUF_PER_FRAME, 7);

		mBuiltInVariables["GF_SHADOW_MAP_JITTER_TEXTURE"] = SShaderVariableAttribute(ESAVT_SHADOW_MAP_JITTER_TEXTURE, EUF_PER_FRAME, 0);
		mBuiltInVariables["GF_PL_SHADOW_MAP_JITTER_TEXTURE"] = SShaderVariableAttribute(ESAVT_PL_SHADOW_MAP_JITTER_TEXTURE, EUF_PER_FRAME, 0);

		// create built-in samplers
		ISamplerManager* samplerMgr = ISamplerManager::getInstance();
		mBuiltInSamplers["GF_SHADOW_MAP_SAMPLER"] = samplerMgr->get(ISamplerManager::SHADOW_MAP_SAMPLER);
		mBuiltInSamplers["GF_WRAP_POINT_SAMPLER"] = samplerMgr->get(ISamplerManager::WRAP_POINT_SAMPLER);
	}


	IPipeline* CPipelineManager::create(const std::string& name,
		IShader** shaders,
		u32 shaderCount,
		IInputLayout* inputlayout,
		E_PRIMITIVE_TYPE primitiveType,
		IRenderState* renderState)
	{
		auto it = mPipelinesCache.find(name);
		if (it != mPipelinesCache.end())
		{
			GF_PRINT_CONSOLE_INFO("create pipeline (%s) repeatedly!\n", name.c_str());
			return it->second;
		}

		IPipeline* pipeline = mResourceFactory->createPipeline(name,
			shaders, shaderCount, inputlayout, primitiveType, renderState);

		if (!pipeline)
		{
			GF_PRINT_CONSOLE_INFO("create pipeline '%s' failed.\n", name.c_str());
			return nullptr;
		}

		// insert the shader built-in variables to pipeline.
		for (u32 i = 0; i < shaderCount; i++)
		{
			IShader* shader = shaders[i];
			if (shader)
			{
				shader->registerAutoVariablesToPipeline(pipeline, mBuiltInVariables);
			}
		}

		pipeline->registerAutoSamplers(mBuiltInSamplers);

		mPipelinesCache.insert(std::make_pair(name, pipeline));

		return pipeline;
	}


	IPipeline* CPipelineManager::get(const std::string& name, bool bLoadIfNotExist)
	{
		auto it = mPipelinesCache.find(name);
		if (it != mPipelinesCache.end())
			return it->second;

		if (bLoadIfNotExist)
		{
			return IResourceGroupManager::getInstance()->loadPipeline(name);
		}

		return nullptr;
	}

}
