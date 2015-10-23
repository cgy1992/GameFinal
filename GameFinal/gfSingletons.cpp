#include "stdafx.h"
#include "ITextureManager.h"
#include "ISamplerManager.h"
#include "IShaderManager.h"
#include "IRenderStateManager.h"
#include "IPipelineManager.h"
#include "IMeshManager.h"
#include "IMaterialManager.h"
#include "IInputLayoutManager.h"
#include "IResourceGroupManager.h"
#include "IGeometryCreator.h"
#include "IDevice.h"
#include "IVideoDriver.h"
#include "IInputDriver.h"

namespace gf
{
	//_DEFINE_SINGLETON_INSTANCE(IDevice);
	_DEFINE_SINGLETON_INSTANCE(ITextureManager);
	_DEFINE_SINGLETON_INSTANCE(ISamplerManager);
	_DEFINE_SINGLETON_INSTANCE(IShaderManager);
	_DEFINE_SINGLETON_INSTANCE(IResourceXmlParser);
	_DEFINE_SINGLETON_INSTANCE(IResourceLoader);
	_DEFINE_SINGLETON_INSTANCE(IResourceGroupManager);
	_DEFINE_SINGLETON_INSTANCE(IResourceFactory);
	_DEFINE_SINGLETON_INSTANCE(IRenderStateManager);
	_DEFINE_SINGLETON_INSTANCE(IPipelineManager);
	_DEFINE_SINGLETON_INSTANCE(IMeshManager);
	_DEFINE_SINGLETON_INSTANCE(IMaterialManager);
	_DEFINE_SINGLETON_INSTANCE(IInputLayoutManager);
	_DEFINE_SINGLETON_INSTANCE(IModelFileParser);
	_DEFINE_SINGLETON_INSTANCE(IGeometryCreator);
	_DEFINE_SINGLETON_INSTANCE(IDevice);
	_DEFINE_SINGLETON_INSTANCE(IVideoDriver);
	_DEFINE_SINGLETON_INSTANCE(IInputDriver);

	const std::string ISamplerManager::WRAP_POINT_SAMPLER = std::string("gf_point_sampler");
	const std::string ISamplerManager::SHADOW_MAP_SAMPLER = std::string("gf_shadow_map_sampler");

	const std::string ITextureManager::SHADOW_MAP_JITTER_TEXTURE = std::string("gf_shadow_map_jitter_texture");
	const std::string ITextureManager::PL_SHADOW_MAP_JITTER_TEXTURE = std::string("gf_pl_shadow_map_jitter_texture");
}

