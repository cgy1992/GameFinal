#include "stdafx.h"
#include "CD3D11Shader.h"
#include "D3DUtil.h"

namespace gf
{
	bool CD3D11Shader::compile(const char* szFileName, const char* szFunctionName, E_SHADER_TYPE type)
	{
		HRESULT hr;
		IResourceGroupManager* rmgr = IResourceGroupManager::getInstance();
		const char* szProfile = getShaderProfile();
		ID3D10Blob* errorMessage = NULL;
		std::string filename = szFileName;

		// collect all macros.
		D3D10_SHADER_MACRO* pDefines = NULL;
		if (mMacros.size() > 0)
		{
			pDefines = new D3D10_SHADER_MACRO[mMacros.size() + 1];
			SShaderMacroSet::Iterator it = mMacros.getIterator();
			u32 i = 0;
			while (it.hasNext())
			{
				const SShaderMacro& macro = it.next();
				pDefines[i].Name = macro.Name.c_str();
				pDefines[i].Definition = macro.Definition.c_str();
				i++;
			}
			pDefines[i].Name = NULL;
			pDefines[i].Definition = NULL;
		}
		

		std::string filepath;
		if (!rmgr->getFullPath(filename, filepath, ERFT_SHADER))
		{
			GF_PRINT_CONSOLE_INFO("The shader file named '%s' has not been found.\n", szFileName);
			return false;
		}

		const u32 MAX_CHAR_NUM = 3000 * 80;
		char pSrcData[MAX_CHAR_NUM];
		memset(pSrcData, 0, sizeof(pSrcData));
		if (!getShaderFileContent(filepath.c_str(), pSrcData))
		{
			GF_PRINT_CONSOLE_INFO("The shader file named '%s' cannot be opened.\n", szFileName);
			return false;
		}
		
		hr = D3DX11CompileFromMemory(pSrcData, strlen(pSrcData), filepath.c_str(), 
			pDefines, NULL, szFunctionName, szProfile, D3D10_SHADER_ENABLE_STRICTNESS, 0,
			NULL, &mShaderBuffer, &errorMessage, NULL);

		//hr = D3DX11CompileFromFileA(szFileName, pDefines, NULL, szFunctionName, szProfile, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		//	&mShaderBuffer, &errorMessage, NULL);

		delete[] pDefines;
		
		if (FAILED(hr))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
			{
				GF_PRINT_CONSOLE_INFO((char*)errorMessage->GetBufferPointer());
			}
			// If there was  nothing in the error message then it simply could not find the shader file itself.
			else
			{
				GF_PRINT_CONSOLE_INFO("load shader file('%s') and function ('%s') failed!\n", szFileName, szFunctionName);
			}
			return false;
		}

		initContext();

		return true;
	}


	bool CD3D11Shader::getShaderFileContent(const char* szFullPath, char content[])
	{
		const static char* szBuiltInShaderNames[] = {
			"GameFinal.hlsl",
			"gfShadow.hlsl",
			NULL
		};

		const u32 MAX_CHAR_NUM = 3000 * 80;
		IResourceGroupManager* rgmr = IResourceGroupManager::getInstance();
		const char* szProcessPath = IDevice::getInstance()->getProcessPath();

		char pOriginData[MAX_CHAR_NUM] = { 0 };
		memset(content, 0, sizeof(content));

		FILE* fp = nullptr;
		errno_t err = fopen_s(&fp, szFullPath, "rb");
		if (err != 0)
		{
			return false;
		}

		fseek(fp, 0, SEEK_END);
		u32 fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(pOriginData, fileSize, 1, fp);

		//printf("%s", pSrcData);
		fclose(fp);

		u32 i = 0;
		
		while (szBuiltInShaderNames[i])
		{
			const char* shaderName = szBuiltInShaderNames[i];
			std::string fullPath;
			char szIncludeCode[256] = { 0 };

			if (rgmr->getFullPath(shaderName, fullPath, ERFT_SHADER))
			{
				strcpy_s(szIncludeCode, "#include\"");
				strcat_s(szIncludeCode, szProcessPath);
				strcat_s(szIncludeCode, fullPath.c_str());
				strcat_s(szIncludeCode, "\"\n");
				u32 c = sizeof(content);
				strcat_s(content, MAX_CHAR_NUM, szIncludeCode);
			}
			i++;
		}

		strcat_s(content, MAX_CHAR_NUM, pOriginData);
		return true;
	}

	bool CD3D11Shader::initContext()
	{
		HRESULT hr;
		ID3D11ShaderReflection* shaderReflection = NULL;

		memset(md3dConstantBuffers, 0, sizeof(md3dConstantBuffers));

		hr = D3DReflect(mShaderBuffer->GetBufferPointer(),
			mShaderBuffer->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			(void**)&shaderReflection);

		if (FAILED(hr))
		{
			throw std::runtime_error("Create Shader Reflection Failed!");
		}

		D3D11_SHADER_DESC shaderDesc;
		shaderReflection->GetDesc(&shaderDesc);

		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		s32 uiMaxTextureBindPoint = -1;
		s32 uiMaxSamplerBindPoint = -1;
		for (u32 i = 0; i < shaderDesc.BoundResources; i++)
		{
			shaderReflection->GetResourceBindingDesc(i, &resourceDesc);

			//std::cout << resourceDesc.Name << std::endl;

			if (resourceDesc.Type == D3D10_SIT_TEXTURE)
			{
				std::string name = resourceDesc.Name;
				mShaderResourceDescs.insert(std::make_pair(name, resourceDesc));

				s32 bindPoint = resourceDesc.BindPoint;
				if (bindPoint > uiMaxTextureBindPoint)
					uiMaxTextureBindPoint = bindPoint;
			}
			else if (resourceDesc.Type == D3D_SIT_SAMPLER)
			{
				std::string name = resourceDesc.Name;
				mShaderSamplerDescs.insert(std::make_pair(name, resourceDesc));

				s32 bindPoint = resourceDesc.BindPoint;
				if (bindPoint > uiMaxSamplerBindPoint)
					uiMaxSamplerBindPoint = bindPoint;
			}
		}

		mSrvNum = (uiMaxTextureBindPoint >= 0) ? uiMaxTextureBindPoint + 1 : 0;
		mSamplerNum = (uiMaxSamplerBindPoint >= 0) ? uiMaxSamplerBindPoint + 1 : 0;

		for (u32 i = 0; i < shaderDesc.ConstantBuffers; i++)
		{
			ID3D11ShaderReflectionConstantBuffer* cbReflection = NULL;
			cbReflection = shaderReflection->GetConstantBufferByIndex(i);

			D3D11_SHADER_BUFFER_DESC cbDesc;
			cbReflection->GetDesc(&cbDesc);

			SShaderConstantBuffer* pCbStruct = new SShaderConstantBuffer(cbDesc.Name);
			pCbStruct->Size = cbDesc.Size;

			D3D11_BUFFER_DESC bufDesc;
			bufDesc.ByteWidth = cbDesc.Size;
			bufDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufDesc.MiscFlags = 0;
			bufDesc.StructureByteStride = 0;

			md3dDevice->CreateBuffer(&bufDesc, 0, &md3dConstantBuffers[i]);

			pCbStruct->pData = malloc(cbDesc.Size);

			std::unique_ptr<SShaderConstantBuffer> cbPtr(pCbStruct);
			mConstantBuffers.push_back(std::move(cbPtr));

			u32 constantBufferIndex = mConstantBuffers.size() - 1;

			for (u32 j = 0; j < cbDesc.Variables; j++)
			{
				ID3D11ShaderReflectionVariable* reflectionVariable = cbReflection->GetVariableByIndex(j);
				if (reflectionVariable)
				{
					SShaderConstantVariable scv;

					reflectionVariable->GetDesc(&scv.Desc);

					ID3D11ShaderReflectionType* reflectionType = reflectionVariable->GetType();

					reflectionType->GetDesc(&scv.TypeDesc);

					std::string varname = scv.Desc.Name;

					if (mConstantVariables.find(varname) != mConstantVariables.end())
					{
						throw std::runtime_error("put the variable with same name into one shader file!");
					}

					scv.ConstantBufferIndex = constantBufferIndex;
					mConstantVariables.insert(std::make_pair(varname, scv));
				}
			}
		}

		ReleaseCOM(shaderReflection);

		return true;
	}

	SShaderConstantVariable* CD3D11Shader::getConstantVariable(const std::string& varname)
	{
		auto it = mConstantVariables.find(varname);
		if (it == mConstantVariables.end())
			return nullptr;

		return &(it->second);
	}

	bool CD3D11Shader::setRawData(const std::string& varname, void* data, u32 size, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		u8* pData = reinterpret_cast<u8*>(mConstantBuffers[cv->ConstantBufferIndex]->pData);
		memcpy(pData + cv->Desc.StartOffset, data, size);
		mConstantBuffers[cv->ConstantBufferIndex]->Dirty = true;
		cv->AlreadyUpdated = true;
		return true;
	}

	bool CD3D11Shader::setFloat(const std::string& varname, f32 val, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		s32 sval;
		u32 uval;

		if (cv->TypeDesc.Class == D3D10_SVC_SCALAR)
		{
			switch (cv->TypeDesc.Type)
			{
			case D3D10_SVT_FLOAT:
				return setRawData(cv, &val, sizeof(f32));
			case D3D10_SVT_INT:
				sval = static_cast<s32>(val);
				return setRawData(cv, &sval, sizeof(s32));
			case D3D10_SVT_UINT:
				uval = static_cast<u32>(val);
				return setRawData(cv, &sval, sizeof(u32));
			}
		}

		return false;
	}

	bool CD3D11Shader::setUint(const std::string& varname, u32 val, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		f32 fval;
		s32 sval;

		if (cv->TypeDesc.Class == D3D10_SVC_SCALAR)
		{
			switch (cv->TypeDesc.Type)
			{
			case D3D10_SVT_UINT:
				return setRawData(cv, &val, sizeof(u32));
			case D3D10_SVT_INT:
				sval = static_cast<s32>(val);
				return setRawData(cv, &sval, sizeof(s32));
			case D3D10_SVT_FLOAT:
				fval = static_cast<f32>(val);
				return setRawData(cv, &fval, sizeof(f32));
			}
		}

		return false;
	}

	bool CD3D11Shader::setVector(const std::string& varname, const f32* val, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		if (cv->TypeDesc.Class != D3D10_SVC_VECTOR ||
			cv->TypeDesc.Type != D3D10_SVT_FLOAT)
		{
			return false;
		}

		return setRawData(cv, (void*)val, sizeof(f32)* cv->TypeDesc.Columns);
	}

	bool CD3D11Shader::setVector(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdate /*= false*/)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		if (cv->TypeDesc.Class != D3D10_SVC_VECTOR ||
			cv->TypeDesc.Type != D3D10_SVT_FLOAT)
		{
			return false;
		}

		return setRawData(cv, (void*)&val, sizeof(f32)* cv->TypeDesc.Columns);
	}

	bool CD3D11Shader::setAttribute(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		if (cv->TypeDesc.Class == D3D10_SVC_VECTOR)
		{
			s32 iVal[4];
			u32 uiVal[4];

			switch (cv->TypeDesc.Type)
			{
			case D3D10_SVT_FLOAT:
				return setRawData(cv, (void*)&val, sizeof(f32)* cv->TypeDesc.Columns);
			case D3D10_SVT_INT:
				iVal[0] = static_cast<s32>(val.x);
				iVal[1] = static_cast<s32>(val.y);
				iVal[2] = static_cast<s32>(val.z);
				iVal[3] = static_cast<s32>(val.w);
				return setRawData(cv, (void*)iVal, sizeof(s32)* cv->TypeDesc.Columns);
			case D3D10_SVT_UINT:
				uiVal[0] = static_cast<u32>(val.x);
				uiVal[1] = static_cast<u32>(val.y);
				uiVal[2] = static_cast<u32>(val.z);
				uiVal[3] = static_cast<u32>(val.w);
				return setRawData(cv, (void*)uiVal, sizeof(u32)* cv->TypeDesc.Columns);
			}
		}
		else if (cv->TypeDesc.Class == D3D10_SVC_SCALAR)
		{
			s32 sval;
			u32 uval;
			f32 fval;
			switch (cv->TypeDesc.Type)
			{
			case D3D10_SVT_FLOAT:
				fval = val.x;
				return setRawData(cv, &fval, sizeof(f32));
			case D3D10_SVT_INT:
				sval = static_cast<s32>(val.x);
				return setRawData(cv, &sval, sizeof(s32));
			case D3D10_SVT_UINT:
				uval = static_cast<u32>(val.x);
				return setRawData(cv, &sval, sizeof(u32));
			}
		}

		return false;
	}

	bool CD3D11Shader::setMatrix(const std::string& varname, const f32* matrix, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		if (cv->TypeDesc.Class != D3D10_SVC_MATRIX_COLUMNS ||
			cv->TypeDesc.Type != D3D10_SVT_FLOAT ||
			cv->TypeDesc.Columns != 4 ||
			cv->TypeDesc.Rows != 4)
		{
			return false;
		}
		XMFLOAT4X4 m = *reinterpret_cast<const XMFLOAT4X4*>(matrix);
		XMMATRIX mat = XMLoadFloat4x4(&m);
		mat = XMMatrixTranspose(mat);

		return setRawData(cv, (void*)&mat, sizeof(f32)* 16);
	}

	bool CD3D11Shader::setMatrix(const std::string& varname, CXMMATRIX matrix, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		if (cv->TypeDesc.Class != D3D10_SVC_MATRIX_COLUMNS ||
			cv->TypeDesc.Type != D3D10_SVT_FLOAT ||
			cv->TypeDesc.Columns != 4 ||
			cv->TypeDesc.Rows != 4)
		{
			return false;
		}
		XMMATRIX m = XMMatrixTranspose(matrix);
		return setRawData(cv, (void*)&m, sizeof(f32)* 16);
	}

	bool CD3D11Shader::setMatrix(const std::string& varname, const XMFLOAT4X4& matrix, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		if (cv->TypeDesc.Class != D3D10_SVC_MATRIX_COLUMNS ||
			cv->TypeDesc.Type != D3D10_SVT_FLOAT ||
			cv->TypeDesc.Columns != 4 ||
			cv->TypeDesc.Rows != 4)
		{
			return false;
		}
		XMMATRIX m = XMLoadFloat4x4(&matrix);
		XMMATRIX transposeMat = XMMatrixTranspose(m);
		return setRawData(cv, (void*)&transposeMat, sizeof(f32)* 16);
	}




	bool CD3D11Shader::setTransposedMatrixArray(const std::string& varname, const f32* matrixs, u32 count, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		if (cv->TypeDesc.Class != D3D10_SVC_MATRIX_COLUMNS ||
			cv->TypeDesc.Type != D3D10_SVT_FLOAT ||
			cv->TypeDesc.Columns != 4 ||
			cv->TypeDesc.Rows != 4 ||
			cv->TypeDesc.Elements == 0)
		{
			return false;
		}

		return setRawData(cv, (void*)matrixs, sizeof(XMFLOAT4X4)* count);
	}

	bool CD3D11Shader::setMatrixArray(const std::string& varname, XMFLOAT4X4 matrixs[], u32 count, bool ignoreIfAlreadyUpdate)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return false;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return false;

		if (cv->TypeDesc.Class != D3D10_SVC_MATRIX_COLUMNS ||
			cv->TypeDesc.Type != D3D10_SVT_FLOAT ||
			cv->TypeDesc.Columns != 4 ||
			cv->TypeDesc.Rows != 4 ||
			cv->TypeDesc.Elements == 0)
		{
			return false;
		}

		std::vector<XMFLOAT4X4> transposedMatrixs(count);
		for (u32 i = 0; i < count; i++)
		{
			XMMATRIX M = XMLoadFloat4x4(&matrixs[i]);
			M = XMMatrixTranspose(M);
			XMStoreFloat4x4(&transposedMatrixs[i], M);
		}

		return setRawData(cv, (void*)&transposedMatrixs[0], sizeof(XMFLOAT4X4)* count);
	}

	bool CD3D11Shader::setTexture(const std::string& varname, ITexture* texture)
	{
		auto it = mShaderResourceDescs.find(varname);
		if (it == mShaderResourceDescs.end())
			return false;

		const D3D11_SHADER_INPUT_BIND_DESC& desc = it->second;
		if (desc.Type != D3D10_SIT_TEXTURE)
			return false;

		u32 bindPoint = desc.BindPoint;
		//md3dDriver->setTexture(getType(), bindPoint, texture);
		texture->apply(getType(), bindPoint);

		return true;
	}

	bool CD3D11Shader::setSampler(const std::string& varname, ISampler* sampler)
	{
		auto it = mShaderSamplerDescs.find(varname);
		if (it == mShaderSamplerDescs.end())
			return false;

		const D3D11_SHADER_INPUT_BIND_DESC& desc = it->second;
		md3dDriver->setSampler(getType(), desc.BindPoint, sampler);
		return true;
	}

	bool CD3D11Shader::existSampler(const std::string& name) const
	{
		return mShaderSamplerDescs.find(name) != mShaderSamplerDescs.end();
	}


	bool CD3D11Shader::setRawData(SShaderConstantVariable* cv, void* data, u32 size)
	{
		u8* pData = reinterpret_cast<u8*>(mConstantBuffers[cv->ConstantBufferIndex]->pData);
		memcpy(pData + cv->Desc.StartOffset, data, size);
		mConstantBuffers[cv->ConstantBufferIndex]->Dirty = true;
		cv->AlreadyUpdated = true;
		return true;
	}

	// return the actual element count that has been set.
	u32 CD3D11Shader::setArray(const std::string& varname, 
		void* data, u32 arraySize, u32 elementSize, 
		bool ignoreIfAlreadyUpdate /*= false*/)
	{
		SShaderConstantVariable* cv = getConstantVariable(varname);
		if (cv == nullptr)
			return 0;

		if (cv->TypeDesc.Elements == 0)
			return 0;

		if (cv->TypeDesc.Elements < arraySize)
			arraySize = cv->TypeDesc.Elements;

		if (ignoreIfAlreadyUpdate && cv->AlreadyUpdated)
			return arraySize;

		if (setRawData(cv, data, arraySize * elementSize))
			return arraySize;

		return 0;
	}

	bool CD3D11Shader::applyConstantBuffer(ID3D11DeviceContext* pd3dDeviceContext, u32 index)
	{
		HRESULT hr;
		SShaderConstantBuffer* pConstantBuffer = mConstantBuffers[index].get();
		ID3D11Buffer* cbBuffer = md3dConstantBuffers[index];
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		hr = pd3dDeviceContext->Map(cbBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
			return false;

		memcpy(mappedResource.pData, pConstantBuffer->pData, pConstantBuffer->Size);

		pd3dDeviceContext->Unmap(cbBuffer, 0);

		return true;
	}

	// 一定调用
	void CD3D11Shader::update(bool forceUpdate /* = false */)
	{
		u32 constantBufferCount = mConstantBuffers.size();
		for (u32 i = 0; i < constantBufferCount; i++)
		{
			SShaderConstantBuffer* pcb = mConstantBuffers[i].get();
			if (pcb->Dirty)
			{
				applyConstantBuffer(md3dDeviceContext, i);
				pcb->Dirty = false;
			}
		}
	}

	void CD3D11Shader::reset()
	{
		auto it = mConstantVariables.begin();
		for (; it != mConstantVariables.end(); it++)
		{
			it->second.AlreadyUpdated = false;
		}
	}

	bool CD3D11Shader::isAlreadyUpdated(const std::string& varname)
	{
		auto it = mConstantVariables.find(varname);
		if (it == mConstantVariables.end())
			return true;

		return it->second.AlreadyUpdated;
	}

	bool CD3D11Shader::isContantVariable(const std::string& varname) const
	{
		return mConstantVariables.find(varname) != mConstantVariables.end();
	}

	bool CD3D11Shader::isTextureVariable(const std::string& varname) const
	{
		return mShaderResourceDescs.find(varname) != mShaderResourceDescs.end();
	}

	void CD3D11Shader::registerAutoVariablesToPipeline(IPipeline* pipeline, const std::map<std::string, SShaderVariableAttribute>& varMap) const
	{
		for (auto i = mConstantVariables.begin(); i != mConstantVariables.end(); i++)
		{
			const std::string& varname = i->first;
			auto j = varMap.find(varname);
			if (j != varMap.end())
			{
				const SShaderVariableAttribute& attr = j->second;
				SShaderAutoVariable var;
				var.ShaderType = getType();
				var.VariableName = varname;
				var.Type = attr.Meaning;
				var.UpdateFrequency = attr.DefaultUpdateFrequency;
				var.IndexParam = attr.IndexParam;
				pipeline->addShaderAutoVariable(var);
			}
		}

		for (auto i = mShaderResourceDescs.begin(); i != mShaderResourceDescs.end(); i++)
		{
			const std::string& varname = i->first;
			auto j = varMap.find(varname);
			if (j != varMap.end())
			{
				const SShaderVariableAttribute& attr = j->second;
				SShaderAutoVariable var;
				var.ShaderType = getType();
				var.VariableName = varname;
				var.Type = attr.Meaning;
				var.UpdateFrequency = attr.DefaultUpdateFrequency;
				var.IndexParam = attr.IndexParam;
				pipeline->addShaderAutoVariable(var);
			}
		}
	}




}
