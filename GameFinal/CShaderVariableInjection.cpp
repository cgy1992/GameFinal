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
			injectVariable(shaderVariables[i], node, pipeline, subset);
		}
	}

	void CShaderVariableInjection::injectBoneTransforms(IPipeline* pipeline, const f32* matrixs, u32 count)
	{
		const std::vector<SShaderAutoVariable>& shaderVariables = pipeline->getShaderAutoVariables();
		u32 shaderVariableCount = shaderVariables.size();

		for (u32 i = 0; i < shaderVariableCount; i++)
		{
			if (shaderVariables[i].Type == ESAVT_BONE_TRANSFORMS)
			{
				pipeline->setTransposedMatrixArray(shaderVariables[i].ShaderType,
					shaderVariables[i].VariableName,
					matrixs, count, shaderVariables[i].UpdateFrequency == EUF_PER_FRAME);
			}
		}
	}

	void CShaderVariableInjection::injectVariable(
		const SShaderAutoVariable& var,
		IMeshNode* mesh,
		IPipeline* pipeline,
		u32 subset)
	{
		E_SHADER_AUTO_VARIABLE_TYPE type = var.Type;
		if (type >= ESAVT_TRANSFORMS_BEGIN && type <= ESAVT_TRANSFORMS_END)
		{
			injectTransformations(var, mesh, pipeline);
		}
		else if (type >= ESAVT_MATERIAL_BEGIN && type <= ESAVT_MATERIAL_END)
		{
			injectMaterial(var, pipeline, mesh->getMaterial(subset));
		}
		else if (type >= ESAVT_SCENE_BEGIN && type <= ESAVT_SCENE_END)
		{
			injectSceneInfo(var, mesh, pipeline);
		}
		else if (type >= ESAVT_WINDOW_SYSTEM_BEGIN && type <= ESAVT_WINDOW_SYSTEM_END)
		{
			injectWindowSystemInfo(var, mesh, pipeline);
		}
	}

	void CShaderVariableInjection::injectMaterial(IMaterial* material, IPipeline* pipeline)
	{
		const std::vector<SShaderAutoVariable>& shaderVariables = pipeline->getShaderAutoVariables();
		u32 shaderVariableCount = shaderVariables.size();

		for (u32 i = 0; i < shaderVariableCount; i++)
		{
			const SShaderAutoVariable& var = shaderVariables[i];
			if (var.Type >= ESAVT_MATERIAL_BEGIN && var.Type <= ESAVT_MATERIAL_END)
			{
				injectMaterial(var, pipeline, material);
			}
		}
	}


	void CShaderVariableInjection::injectTransformations(const SShaderAutoVariable& var, ISceneNode* node, IPipeline* pipeline)
	{
		XMMATRIX M;
		ICameraNode* camera = nullptr;

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
			pipeline->setMatrix(shaderType, var.VariableName, M, ignoreIfAlreadyUpdate);
		}

	}

	void CShaderVariableInjection::injectMaterial(const SShaderAutoVariable& var, IPipeline* pipeline, IMaterial* material)
	{
		if (!material)
			return;

		E_SHADER_TYPE shaderType = var.ShaderType;
		bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);
		ITexture* pTexture = nullptr;

		
		switch (var.Type)
		{
		case ESAVT_MATERIAL:
			pipeline->setRawValue(shaderType, var.VariableName, &material->Colors, sizeof(material->Colors), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_MATERIAL_AMBIENT:
			pipeline->setVector(shaderType, var.VariableName, reinterpret_cast<f32*>(&material->Colors.Ambient), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_MATERIAL_DIFFUSE:
			pipeline->setVector(shaderType, var.VariableName, reinterpret_cast<f32*>(&material->Colors.Diffuse), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_MATERIAL_SPECULAR:
			pipeline->setVector(shaderType, var.VariableName, reinterpret_cast<f32*>(&material->Colors.Specular), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_MATERIAL_EMISSIVE:
			pipeline->setVector(shaderType, var.VariableName, reinterpret_cast<f32*>(&material->Colors.Emissive), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_TEXTURE:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				pipeline->setTexture(shaderType, var.VariableName, pTexture);
			}
			break;
		case ESAVT_TEXTURE_WIDTH:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				pipeline->setUint(shaderType, var.VariableName, pTexture->getWidth(), ignoreIfAlreadyUpdate);
			}
			break;
		case ESAVT_TEXTURE_HEIGHT:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				pipeline->setUint(shaderType, var.VariableName, pTexture->getHeight(), ignoreIfAlreadyUpdate);
			}
			break;
		case ESAVT_INVERSE_TEXTURE_WIDTH:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				f32 invWidth = 1.0f / pTexture->getWidth();
				pipeline->setFloat(shaderType, var.VariableName, invWidth, ignoreIfAlreadyUpdate);
			}
			break;
		case ESAVT_INVERSE_TEXTURE_HEIGHT:
			pTexture = material->getTexture(var.IndexParam);
			if (pTexture)
			{
				f32 invHeight = 1.0f / pTexture->getHeight();
				pipeline->setFloat(shaderType, var.VariableName, invHeight, ignoreIfAlreadyUpdate);
			}
			break;
		}
	}


	void CShaderVariableInjection::injectSceneInfo(const SShaderAutoVariable& var, ISceneNode* node, IPipeline* pipeline)
	{
		ISceneManager* sceneManager = node->getSceneManager();
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
				pipeline->setVector(shaderType, var.VariableName, reinterpret_cast<f32*>(&p), ignoreIfAlreadyUpdate);
			}
			return;
		}
		else if (var.Type == ESAVT_CAMERA_FRUSTUM)
		{
			ICameraNode* camera = sceneManager->getCameraNode(var.IndexParam);
			if (camera)
			{
				math::SFrustum frustum = camera->getFrustum();
				pipeline->setRawValue(shaderType, var.VariableName, &frustum, sizeof(math::SFrustum), ignoreIfAlreadyUpdate);
			}
			return;
		}


		/* 注入整个光照结构体 */
		bool flag = false;
		ILightNode* light = nullptr;
		switch (var.Type)
		{
		case ESAVT_LIGHT:
			light = sceneManager->getLightNode(var.IndexParam);
			flag = true;
			break;
		}

		if (flag && light)
		{
			const SLightData& lightData = light->getLightData();
			pipeline->setRawValue(shaderType, var.VariableName, (void*)&lightData, sizeof(SLightData), ignoreIfAlreadyUpdate);
		}
	}

	void CShaderVariableInjection::injectForTerrainNode(ITerrainNode* node, IPipeline* pipeline)
	{
		const std::vector<SShaderAutoVariable>& shaderVariables = pipeline->getShaderAutoVariables();
		u32 shaderVariableCount = shaderVariables.size();

		for (u32 i = 0; i < shaderVariableCount; i++)
		{
			injectVariable(shaderVariables[i], node, pipeline, 0);
			injectTerrainVariable(shaderVariables[i], node->getTerrainMesh(), pipeline);
		}
	}

	void CShaderVariableInjection::injectTerrainVariable(const SShaderAutoVariable& var, ITerrainMesh* mesh, IPipeline* pipeline)
	{
		bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);

		if (var.Type == ESAVT_TERRAIN_ROW_CELL)
		{
			u32 cellNum = mesh->getRowCellNum();
			pipeline->setUint(var.ShaderType, var.VariableName, cellNum, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_INVERSE_TERRAIN_ROW_CELL)
		{
			f32 cellNumReciprocal = 1.0f / mesh->getRowCellNum();
			pipeline->setFloat(var.ShaderType, var.VariableName, cellNumReciprocal, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_ROW_VERTEX)
		{
			u32 vertexNum = mesh->getRowVertexNum();
			pipeline->setUint(var.ShaderType, var.VariableName, vertexNum, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_HEIGHT_SCALE)
		{
			f32 heightScale = mesh->getHeightScale();
			pipeline->setFloat(var.ShaderType, var.VariableName, heightScale, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_VERTEX_SPACE)
		{
			f32 vertexSpace = mesh->getVertexSpace();
			pipeline->setFloat(var.ShaderType, var.VariableName, vertexSpace, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_TEXCOORD_SCALE)
		{
			f32 texcoordScale = mesh->getTexcoordScale();
			pipeline->setFloat(var.ShaderType, var.VariableName, texcoordScale, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_HEIGHTMAP_TEXTURE)
		{
			ITexture* texture = mesh->getHeightMapTexture();
			pipeline->setTexture(var.ShaderType, var.VariableName, texture);
		}
		else if (var.Type == ESAVT_TERRAIN_WIDTH)
		{
			f32 width = mesh->getTotalWidth();
			pipeline->setFloat(var.ShaderType, var.VariableName, width, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_PATCH_WIDTH)
		{
			f32 patchWidth = mesh->getPatchWidth();
			pipeline->setFloat(var.ShaderType, var.VariableName, patchWidth, ignoreIfAlreadyUpdate);
		}
		else if (var.Type == ESAVT_TERRAIN_ROW_PATCH)
		{
			u32 patchNum = mesh->getRowPatchNum();
			pipeline->setUint(var.ShaderType, var.VariableName, patchNum, ignoreIfAlreadyUpdate);
		}
	}

	void CShaderVariableInjection::injectWindowSystemInfo(const SShaderAutoVariable& var, ISceneNode* node, IPipeline* pipeline)
	{
		bool ignoreIfAlreadyUpdate = (var.UpdateFrequency == EUF_PER_FRAME);

		ISceneManager* smgr = node->getSceneManager();
		IVideoDriver* driver = smgr->getVideoDriver();
		IDevice* device = smgr->getDevice();

		switch (var.Type)
		{
		case ESAVT_VIEWPORT_WIDTH:
			pipeline->setFloat(var.ShaderType, var.VariableName, driver->getViewport().Width, ignoreIfAlreadyUpdate);
			break;
		case ESAVT_VIEWPORT_HEIGHT:
			pipeline->setFloat(var.ShaderType, var.VariableName, driver->getViewport().Height, ignoreIfAlreadyUpdate);
			break;
		case ESAVT_INVERSE_VIEWPORT_WIDTH:
			pipeline->setFloat(var.ShaderType, var.VariableName, 1.0f / driver->getViewport().Width, ignoreIfAlreadyUpdate);
			break;
		case ESAVT_INVERSE_VIEWPORT_HEIGHT:
			pipeline->setFloat(var.ShaderType, var.VariableName, 1.0f / driver->getViewport().Height, ignoreIfAlreadyUpdate);
			break;
		case ESAVT_WINDOW_WIDTH:
			pipeline->setUint(var.ShaderType, var.VariableName, device->getClientWidth(), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_WINDOW_HEIGHT:
			pipeline->setUint(var.ShaderType, var.VariableName, device->getClientHeight(), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_INVERSE_WINDOW_WIDTH:
			pipeline->setFloat(var.ShaderType, var.VariableName, 1.0f / device->getClientWidth(), ignoreIfAlreadyUpdate);
			break;
		case ESAVT_INVERSE_WINDOW_HEIGHT:
			pipeline->setFloat(var.ShaderType, var.VariableName, 1.0f / device->getClientHeight(), ignoreIfAlreadyUpdate);
			break;
		}
	}

}


