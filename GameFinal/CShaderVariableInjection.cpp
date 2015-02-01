#include "stdafx.h"
#include "CShaderVariableInjection.h"

namespace gf
{
	void CShaderVariableInjection::inject(IMeshNode* node, IPipeline* pipeline, u32 subset)
	{
		const std::vector<SShaderAutoVariable>& shaderVariables = pipeline->getShaderAutoVariables();
		u32 shaderVariableCount = shaderVariables.size();

		for (u32 i = 0; i < shaderVariableCount; i++)
		{
			IShader* shader = pipeline->getShader(shaderVariables[i].ShaderType);
			if (shader)
				injectVariable(shaderVariables[i], node, pipeline, shader, subset);
		}


		for (u32 i = 0; i < EST_SHADER_COUNT; i++)
		{
			IShader* shader = pipeline->getShader((E_SHADER_TYPE)i);
			if (shader)
			{
				const std::vector<SShaderAutoVariable>& vars = shader->getShaderAutoVariables();
				for (u32 j = 0; j < vars.size(); j++)
					injectVariable(vars[j], node, pipeline, shader, subset);
			}
		}
	}

	void CShaderVariableInjection::injectMaterial(IMaterial* material, IPipeline* pipeline)
	{
		const std::vector<SShaderAutoVariable>& shaderVariables = pipeline->getShaderAutoVariables();
		u32 shaderVariableCount = shaderVariables.size();

		for (u32 i = 0; i < shaderVariableCount; i++)
		{
			IShader* shader = pipeline->getShader(shaderVariables[i].ShaderType);
			if (shader)
				injectMaterial(shaderVariables[i], shader, material);
		}

		for (u32 i = 0; i < EST_SHADER_COUNT; i++)
		{
			IShader* shader = pipeline->getShader((E_SHADER_TYPE)i);
			if (shader)
			{
				const std::vector<SShaderAutoVariable>& vars = shader->getShaderAutoVariables();
				for (u32 j = 0; j < vars.size(); j++)
					injectMaterial(vars[j], shader, material);
			}
		}
	}

	void CShaderVariableInjection::injectBoneTransforms(IPipeline* pipeline, const f32* matrixs, u32 count)
	{
		const std::vector<SShaderAutoVariable>& shaderVariables = pipeline->getShaderAutoVariables();
		u32 shaderVariableCount = shaderVariables.size();

		for (u32 i = 0; i < shaderVariableCount; i++)
		{
			IShader* shader = pipeline->getShader(shaderVariables[i].ShaderType);
			if (shader && shaderVariables[i].Type == ESAVT_BONE_TRANSFORMS)
			{
				shader->setTransposedMatrixArray(shaderVariables[i].VariableName,
					matrixs, count, shaderVariables[i].UpdateFrequency == EUF_PER_FRAME);
			}
		}

		for (u32 i = 0; i < EST_SHADER_COUNT; i++)
		{
			IShader* shader = pipeline->getShader((E_SHADER_TYPE)i);
			if (shader)
			{
				const std::vector<SShaderAutoVariable>& vars = shader->getShaderAutoVariables();
				for (u32 j = 0; j < vars.size(); j++)
				{
					if (vars[j].Type == ESAVT_BONE_TRANSFORMS)
					{
						shader->setTransposedMatrixArray(vars[j].VariableName,
							matrixs, count, vars[j].UpdateFrequency == EUF_PER_FRAME);
					}
				}
			}
		}

	}

	void CShaderVariableInjection::injectVariable(
		const SShaderAutoVariable& var,
		IMeshNode* node,
		IPipeline* pipeline,
		IShader* shader,
		u32 subset)
	{
		E_SHADER_AUTO_VARIABLE_TYPE type = var.Type;
		if (type >= ESAVT_TRANSFORMS_BEGIN && type <= ESAVT_TRANSFORMS_END)
		{
			injectTransformations(var, node, shader);
		}
		else if (type >= ESAVT_MATERIAL_BEGIN && type <= ESAVT_MATERIAL_END)
		{
			injectMaterial(var, shader, node->getMaterial(subset));
		}
		else if (type >= ESAVT_SCENE_BEGIN && type <= ESAVT_SCENE_END)
		{
			injectSceneInfo(var, node, pipeline, shader);
		}
		else if (type >= ESAVT_WINDOW_SYSTEM_BEGIN && type <= ESAVT_WINDOW_SYSTEM_END)
		{
			injectWindowSystemInfo(var, shader);
		}
		else if (type >= ESAVT_TERRAIN_BEGIN && type <= ESAVT_TERRAIN_END)
		{
			if (node->getNodeType() == ESNT_TERRAIN_MESH)
			{
				ITerrainNode* terrainNode = dynamic_cast<ITerrainNode*>(node);
				injectTerrainVariable(var, terrainNode->getTerrainMesh(), shader);
			}
		}
	}

	void CShaderVariableInjection::injectToComputeShader(IShader* shader, ISceneNode* node)
	{
		const std::vector<SShaderAutoVariable>& vars = shader->getShaderAutoVariables();
		for (u32 i = 0; i < vars.size(); i++)
		{
			const SShaderAutoVariable& var = vars[i];
			E_SHADER_AUTO_VARIABLE_TYPE type = var.Type;
			if (type >= ESAVT_TRANSFORMS_BEGIN && type <= ESAVT_TRANSFORMS_END)
			{
				injectTransformations(var, node, shader);
			}
			else if (type >= ESAVT_SCENE_BEGIN && type <= ESAVT_SCENE_END)
			{
				injectSceneInfo(var, node, nullptr, shader);
			}
			else if (type >= ESAVT_WINDOW_SYSTEM_BEGIN && type <= ESAVT_WINDOW_SYSTEM_END)
			{
				injectWindowSystemInfo(var, shader);
			}
		}
	}


	void CShaderVariableInjection::injectTransformations(const SShaderAutoVariable& var, ISceneNode* node, IShader* shader)
	{
		XMMATRIX M;
		ICameraNode* camera = nullptr;
		if (!shader)
			return;
		if (!node) 
			return;

		ISceneManager* sceneManager = node->getSceneManager();

		/* 是否需要设置该变量 */
		bool flag = false;

		switch (var.Type)
		{
		case ESAVT_WORLD_MATRIX:
			M = node->getAbsoluteTransformation();
			flag = true;
			break;
		case ESAVT_INVERSE_WORLD_MATRIX:
		{
										   
										   M = node->getAbsoluteTransformation();
										   XMVECTOR det = XMMatrixDeterminant(M);
										   M = XMMatrixInverse(&det, M);
										   flag = true;
		}
			break;
		case ESAVT_TRANSPOSE_WORLD_MATRIX:
			M = node->getAbsoluteTransformation();
			M = XMMatrixTranspose(M);
			flag = true;
			break;
		case ESAVT_INVERSE_TRANSPOSE_WORLD_MATRIX: // (A-1)T
			M = node->getAbsoluteTransformation();
			M = math::InverseTranspose(M);
			flag = true;
			break;
		case ESAVT_VIEW_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getViewMatrix());
				flag = true;
			}
			break;
		case ESAVT_INVERSE_VIEW_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getViewMatrix());
				XMVECTOR det = XMMatrixDeterminant(M);
				M = XMMatrixInverse(&det, M);
				flag = true;
			}
			break;
		case ESAVT_TRANSPOSE_VIEW_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getViewMatrix());
				M = XMMatrixTranspose(M);
				flag = true;
			}
			break;
		case ESAVT_INVERSE_TRANSPOSE_VIEW_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getViewMatrix());
				M = math::InverseTranspose(M);
				flag = true;
			}
			break;
		case ESAVT_PROJ_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getProjMatrix());
				flag = true;
			}
			break;
		case ESAVT_INVERSE_PROJ_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getProjMatrix());
				XMVECTOR det = XMMatrixDeterminant(M);
				M = XMMatrixInverse(&det, M);
				flag = true;
			}
			break;
		case ESAVT_TRANSPOSE_PROJ_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getProjMatrix());
				M = XMMatrixTranspose(M);
				flag = true;
			}
			break;
		case ESAVT_INVERSE_TRANSPOSE_PROJ_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getProjMatrix());
				M = math::InverseTranspose(M);
				flag = true;
			}
			break;
		case ESAVT_VIEW_PROJ_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getViewProjMatrix());
				flag = true;
			}
			break;
		case ESAVT_INVERSE_VIEW_PROJ_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getViewProjMatrix());
				XMVECTOR det = XMMatrixDeterminant(M);
				M = XMMatrixInverse(&det, M);
				flag = true;
			}
			break;
		case ESAVT_TRANSPOSE_VIEW_PROJ_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getViewProjMatrix());
				M = XMMatrixTranspose(M);
				flag = true;
			}
			break;
		case ESAVT_INVERSE_TRANSPOSE_VIEW_PROJ_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				M = XMLoadFloat4x4(&camera->getViewProjMatrix());
				M = math::InverseTranspose(M);
				flag = true;
			}
			break;
		case ESAVT_WORLD_VIEW_PROJ_MATRIX:
			camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				XMMATRIX viewProj = XMLoadFloat4x4(&camera->getViewProjMatrix());
				XMMATRIX world = node->getAbsoluteTransformation();
				M = XMMatrixMultiply(world, viewProj);
				flag = true;
			}
			break;
		}

		if (flag)
		{
			E_SHADER_TYPE shaderType = var.ShaderType;
			bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);
			shader->setMatrix(var.VariableName, M, ignoreIfAlreadyUpdate);
		}

	}

	void CShaderVariableInjection::injectMaterial(const SShaderAutoVariable& var, IShader* shader, IMaterial* material)
	{
		if (!material)
			return;

		if (!shader)
			return;

		E_SHADER_TYPE shaderType = var.ShaderType;
		bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);
		ITexture* pTexture = nullptr;

		switch (var.Type)
		{
		case ESAVT_MATERIAL_COLOR:
		{
									 XMFLOAT4 materialColor[4];
									 materialColor[0] = material->getAttribute("ambient");
									 materialColor[1] = material->getAttribute("diffuse");
									 materialColor[2] = material->getAttribute("specular");
									 materialColor[3] = material->getAttribute("emissive");
									 shader->setRawData(var.VariableName, &materialColor[0], sizeof(materialColor), ignoreIfAlreadyUpdate);
		}
		case ESAVT_MATERIAL_AMBIENT:
		{
									   XMFLOAT4 ambient;
									   ambient = material->getAttribute("ambient");
									   shader->setAttribute(var.VariableName, ambient, ignoreIfAlreadyUpdate);
		}
			break;
		case ESAVT_MATERIAL_DIFFUSE:
		{
									   XMFLOAT4 diffuse;
									   diffuse = material->getAttribute("diffuse");
									   shader->setAttribute(var.VariableName, diffuse, ignoreIfAlreadyUpdate);
		}
			break;
		case ESAVT_MATERIAL_SPECULAR:
		{
									   XMFLOAT4 specular;
									   specular = material->getAttribute("specular");
									   shader->setAttribute(var.VariableName, specular, ignoreIfAlreadyUpdate);
		}
			break;
		case ESAVT_MATERIAL_EMISSIVE:
		{
										XMFLOAT4 emissive;
										emissive = material->getAttribute("emissive");
										shader->setAttribute(var.VariableName, emissive, ignoreIfAlreadyUpdate);
		}
			break;
		case ESAVT_MATERIAL_ATTRIBUTE:
		{
										 XMFLOAT4 val;
										 if (material->getAttribute(var.MaterialAttributeName, val))
										 {
											 shader->setAttribute(var.VariableName, val, ignoreIfAlreadyUpdate);
										 }
		}
			break;
			
		case ESAVT_TEXTURE:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				shader->setTexture(var.VariableName, pTexture);
			}
			break;
		case ESAVT_TEXTURE_WIDTH:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				shader->setUint(var.VariableName, pTexture->getWidth(), ignoreIfAlreadyUpdate);
			}
			break;
		case ESAVT_TEXTURE_HEIGHT:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				shader->setUint(var.VariableName, pTexture->getHeight(), ignoreIfAlreadyUpdate);
			}
			break;
		case ESAVT_INVERSE_TEXTURE_WIDTH:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				f32 invWidth = 1.0f / pTexture->getWidth();
				shader->setFloat(var.VariableName, invWidth, ignoreIfAlreadyUpdate);
			}
			break;
		case ESAVT_INVERSE_TEXTURE_HEIGHT:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				f32 invHeight = 1.0f / pTexture->getHeight();
				shader->setFloat(var.VariableName, invHeight, ignoreIfAlreadyUpdate);
			}
			break;
		}
	}


	void CShaderVariableInjection::injectSceneInfo(const SShaderAutoVariable& var, 
		ISceneNode* node, IPipeline* pipeline, IShader* shader)
	{
		if (!shader)
			return;

		ISceneManager* sceneManager = node->getSceneManager();
		if (!sceneManager)
			return;

		E_SHADER_TYPE shaderType = var.ShaderType;
		bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);

		/* inject camera position. */
		if (var.Type == ESAVT_CAMERA_POSITION)
		{
			ICameraNode* camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				XMFLOAT3 pos = camera->getAbsolutePosition();
				XMFLOAT4 p = XMFLOAT4(pos.x, pos.y, pos.z, 1.0f);
				//pipeline->setVector(shaderType, var.VariableName, reinterpret_cast<f32*>(&p), ignoreIfAlreadyUpdate);
				shader->setAttribute(var.VariableName, p, ignoreIfAlreadyUpdate);
			}
			return;
		}
		else if (var.Type == ESAVT_CAMERA_FRUSTUM)
		{
			ICameraNode* camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				math::SFrustum frustum = camera->getFrustum();
				shader->setRawData(var.VariableName, &frustum, sizeof(math::SFrustum), ignoreIfAlreadyUpdate);
			}
			return;
		}
		else if (var.Type == ESAVT_CAMERA_SEGMENTS)
		{
			ICameraNode* camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				const f32* segments = camera->getShadowSegments();
				shader->setVector(var.VariableName, &segments[1], ignoreIfAlreadyUpdate);
			}
			return;
		}
		else if (var.Type == ESAVT_AMBIENT)
		{
			XMFLOAT4 ambient = sceneManager->getAmbient();
			shader->setVector(var.VariableName, ambient, ignoreIfAlreadyUpdate);
			return;
		}
		else if (var.Type == ESAVT_LIGHT)
		{
			ILightNode* light = sceneManager->getLightNode(var.IndexParam);
			if (light)
			{
				E_LIGHT_TYPE lightType = light->getType();
				if (lightType == ELT_DIRECTIONAL)
				{
					SDirectionalLight lightData;
					light->getLightData(&lightData);
					shader->setRawData(var.VariableName, (void*)&lightData, sizeof(lightData), ignoreIfAlreadyUpdate);
				}
				else if (lightType == ELT_POINT)
				{
					SPointLight lightData;
					light->getLightData(&lightData);
					shader->setRawData(var.VariableName, (void*)&lightData, sizeof(lightData), ignoreIfAlreadyUpdate);
				}
				else if (lightType == ELT_SPOT)
				{
					SSpotLight lightData;
					light->getLightData(&lightData);
					shader->setRawData(var.VariableName, (void*)&lightData, sizeof(lightData), ignoreIfAlreadyUpdate);
				}
			}
			return;
		}
		else if (var.Type == ESAVT_NEAR_POINT_LIGHTS)
		{
			std::vector<ILightNode*> lights;
			u32 lightCount = 0;
			if (node->getNodeType() & ESNT_MESH)
			{
				IMeshNode* meshNode = (IMeshNode*)node;
				sceneManager->getNearLights(meshNode, ELT_POINT, lights);
				lightCount = lights.size();

				if (lightCount > 0)
				{
					std::vector<SPointLight> lightDataArray;
					lightDataArray.resize(lightCount);
					for (u32 i = 0; i < lightCount; i++)
						lights[i]->getLightData(&lightDataArray[i]);
					
					lightCount = shader->setArray(var.VariableName,
						(void*)&lightDataArray[0], lightCount, sizeof(SPointLight), ignoreIfAlreadyUpdate);
				}

				// inject point lights number, if have.
				injectLightSourceNum(pipeline, shader, ESAVT_NEAR_POINT_LIGHTS_NUM, lightCount);
			}
		}
		else if (var.Type == ESAVT_DIRECTIONAL_LIGHTS)
		{
			const std::list<ILightNode*>& lights = sceneManager->getDirectionalLights();
			u32 lightCount = lights.size();

			if (lightCount > 0)
			{
				std::vector<SDirectionalLight> lightDataArray;
				lightDataArray.resize(lightCount);
				u32 i = 0;
				for (auto it = lights.begin(); it != lights.end(); it++, i++)
					(*it)->getLightData(&lightDataArray[i]);
				
				lightCount = shader->setArray(var.VariableName,
					(void*)&lightDataArray[0], lightCount, sizeof(SDirectionalLight), ignoreIfAlreadyUpdate);
			}

			// inject point lights number, if have.
			injectLightSourceNum(pipeline, shader, ESAVT_DIRECTIONAL_LIGHTS_NUM, lightCount);
		}
		else if (var.Type == ESAVT_SHADOW_MAP)
		{
			u32 lightID = var.IndexParam;
			ILightNode* light = sceneManager->getLightNode(lightID);
			if (!light)
				return;

			ITexture* shadowMapTexture = light->getShadowMap();
			if (shadowMapTexture)
			{
				shader->setTexture(var.VariableName, shadowMapTexture);
			}
		}
		else if (var.Type == ESAVT_SHADOW_MAP_TRANSFORM)
		{
			u32 lightID = var.IndexParam;
			ILightNode* light = sceneManager->getLightNode(lightID);
			if (!light)
				return;

			XMFLOAT4X4 viewProjMatrix = light->getShadowMapTransform();
			shader->setMatrix(var.VariableName, viewProjMatrix, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_SHADOW_MAP_TRANSFORMS)
		{
			u32 lightID = var.IndexParam;
			ILightNode* light = sceneManager->getLightNode(lightID);
			if (!light)
				return;
			E_LIGHT_TYPE lightType = light->getType();
			if (lightType == ELT_DIRECTIONAL)
			{
				XMFLOAT4X4 shadowMapTransforms[ICameraNode::CASCADE_SHADOW_LEVEL];
				light->getShadowMapTransforms(shadowMapTransforms);
				shader->setMatrixArray(var.VariableName, shadowMapTransforms, ICameraNode::CASCADE_SHADOW_LEVEL, ignoreIfAlreadyUpdate);
			}
		}
		else if (var.Type == ESAVT_SHADOW_MAP_SIZE)
		{
			u32 lightID = var.IndexParam;
			ILightNode* light = sceneManager->getLightNode(lightID);
			if (!light)
				return;

			ITexture* shadowMapTexture = light->getShadowMap();
			if (shadowMapTexture)
			{
				f32 w = static_cast<f32>(shadowMapTexture->getWidth());
				f32 h = static_cast<f32>(shadowMapTexture->getHeight());
				shader->setVector(var.VariableName, XMFLOAT4(w, h, 1.0f / w, 1.0f / h), ignoreIfAlreadyUpdate);
			}
		}
		else if (var.Type == ESAVT_SHADOW_MAP_JITTER_TEXTURE)
		{
			ITexture* texture = ITextureManager::getInstance()->get(ITextureManager::SHADOW_MAP_JITTER_TEXTURE, false);
			if (texture)
			{
				shader->setTexture(var.VariableName, texture);
			}
		}

	}

	void CShaderVariableInjection::injectLightSourceNum(IPipeline* pipeline,
		IShader* shader,
		E_SHADER_AUTO_VARIABLE_TYPE shaderVariableType,
		u32 lightCount)
	{
		// inject point lights number, if have.
		if (pipeline)
		{
			const std::vector<SShaderAutoVariable>& pipelineVariables = pipeline->getShaderAutoVariables();
			for (u32 i = 0; i < pipelineVariables.size(); i++)
			{
				const SShaderAutoVariable& var = pipelineVariables[i];
				if (var.Type == shaderVariableType)
				{
					bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);
					shader->setUint(var.VariableName, lightCount, ignoreIfAlreadyUpdate);
				}
			}
		}

		const std::vector<SShaderAutoVariable>& shaderVariables = shader->getShaderAutoVariables();
		for (u32 i = 0; i < shaderVariables.size(); i++)
		{
			const SShaderAutoVariable& var = shaderVariables[i];
			if (var.Type == shaderVariableType)
			{
				bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);
				shader->setUint(var.VariableName, lightCount, ignoreIfAlreadyUpdate);
			}
		}
	}

	void CShaderVariableInjection::injectTerrainVariable(const SShaderAutoVariable& var, ITerrainMesh* mesh, IShader* shader)
	{
		if (!shader)
			return;

		bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);

		if (var.Type == ESAVT_TERRAIN_ROW_CELL)
		{
			u32 cellNum = mesh->getRowCellNum();
			shader->setUint(var.VariableName, cellNum, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_INVERSE_TERRAIN_ROW_CELL)
		{
			f32 cellNumReciprocal = 1.0f / mesh->getRowCellNum();
			shader->setFloat(var.VariableName, cellNumReciprocal, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_ROW_VERTEX)
		{
			u32 vertexNum = mesh->getRowVertexNum();
			shader->setUint(var.VariableName, vertexNum, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_HEIGHT_SCALE)
		{
			f32 heightScale = mesh->getHeightScale();
			shader->setFloat(var.VariableName, heightScale, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_VERTEX_SPACE)
		{
			f32 vertexSpace = mesh->getVertexSpace();
			shader->setFloat(var.VariableName, vertexSpace, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_TEXCOORD_SCALE)
		{
			f32 texcoordScale = mesh->getTexcoordScale();
			shader->setFloat(var.VariableName, texcoordScale, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_HEIGHTMAP_TEXTURE)
		{
			ITexture* texture = mesh->getHeightMapTexture();
			shader->setTexture(var.VariableName, texture);
		}
		else if (var.Type == ESAVT_TERRAIN_WIDTH)
		{
			f32 width = mesh->getTotalWidth();
			shader->setFloat(var.VariableName, width, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_PATCH_WIDTH)
		{
			f32 patchWidth = mesh->getPatchWidth();
			shader->setFloat(var.VariableName, patchWidth, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_ROW_PATCH)
		{
			u32 patchNum = mesh->getRowPatchNum();
			shader->setUint(var.VariableName, patchNum, ignoreIfAlreadyUpdate);
		}
	}

	void CShaderVariableInjection::injectWindowSystemInfo(const SShaderAutoVariable& var, IShader* shader)
	{
		if (!shader)
			return;

		bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);

		IVideoDriver* driver = IVideoDriver::getInstance();
		IDevice* device = IDevice::getInstance();

		switch (var.Type)
		{
		case ESAVT_VIEWPORT_WIDTH:
			shader->setFloat(var.VariableName, driver->getViewport().Width, ignoreIfAlreadyUpdate);
			break;
		case ESAVT_VIEWPORT_HEIGHT:
			shader->setFloat(var.VariableName, driver->getViewport().Height, ignoreIfAlreadyUpdate);
			break;
		case ESAVT_INVERSE_VIEWPORT_WIDTH:
			shader->setFloat(var.VariableName, 1.0f / driver->getViewport().Width, ignoreIfAlreadyUpdate);
			break;
		case ESAVT_INVERSE_VIEWPORT_HEIGHT:
			shader->setFloat(var.VariableName, 1.0f / driver->getViewport().Height, ignoreIfAlreadyUpdate);
			break;
		case ESAVT_WINDOW_WIDTH:
			shader->setUint(var.VariableName, device->getClientWidth(), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_WINDOW_HEIGHT:
			shader->setUint(var.VariableName, device->getClientHeight(), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_INVERSE_WINDOW_WIDTH:
			shader->setFloat(var.VariableName, 1.0f / device->getClientWidth(), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_INVERSE_WINDOW_HEIGHT:
			shader->setFloat(var.VariableName, 1.0f / device->getClientHeight(), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_SCREEN_SIZE:
		{
								  f32 w, h;
								  IRenderTarget* rt = driver->getRenderTarget();
								  if (rt)
								  {
									  w = static_cast<f32>(rt->getWidth()); 
									  h = static_cast<f32>(rt->getHeight());
								  }
								  else
								  {
									  const SViewport& viewport = driver->getViewport();
									  w = viewport.Width;
									  h = viewport.Height;
								  }
								  f32 screenSize[] = { w, h, 1.0f / w, 1.0f / h };
								  shader->setVector(var.VariableName, screenSize, ignoreIfAlreadyUpdate);
		}
			break;
		}
	}

}


