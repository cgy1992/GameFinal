#ifndef __RESOURCE_XML_PARSER_INTERFACE_H__
#define __RESOURCE_XML_PARSER_INTERFACE_H__

#include "IReferenceCounted.h"
#include "IPipeline.h"
#include "IRenderState.h"
#include "IMaterial.h"
namespace gf
{
	class IResourceXmlParser : public IReferenceCounted
	{
	public:

		/* 该结构体用来存储创建一个shader所需的数据 */
		struct SShaderCreateParams
		{
			E_SHADER_TYPE					Type;
			std::string						FileName;
			std::string						FunctionName;
			SShaderMacroSet					Macros;
		};

		struct SRenderStateCreateParams
		{
			E_RENDER_STATE					StateType;
			u32								DwordValue;
			f32								FloatValue;
		};

		/* 该结构体用来存储创建pipeline所需的数据 */
		struct SPipelineCreateParams
		{
			std::string								Name;
			std::vector<SShaderCreateParams>		Shaders;
			std::vector<SShaderAutoVariable>		ShaderAutoVariables;
			std::vector<SInputLayoutElement>		InputLayoutElements;
			E_PRIMITIVE_TYPE						PrimitiveType;
			std::vector<SRenderStateCreateParams>	RenderStates;
			std::map<std::string, SSamplerDesc>		SamplerDescs;

			SPipelineCreateParams()
			{
				//std::fill(Shaders, Shaders + EST_SHADER_COUNT, nullptr);
				PrimitiveType = EPT_TRIANGLELIST;
			}
		};

		struct SMaterialTextureParam
		{
			std::string								Name;
			u32										Index;
		};

		struct SMaterialAttributeParam
		{
			std::string								Name;
			XMFLOAT4								Value;
		};

		struct SMaterialCreateParams
		{
			std::string								Name;
			std::vector<SMaterialAttributeParam>	AttributeParams;
			std::vector<std::string>				PipelineNames;
			std::vector<E_PIPELINE_USAGE>			PipelineUsages;
			std::vector<SMaterialTextureParam>		TextureParams;
		};

		struct SRenderTargetParams
		{
			std::string								Name;
			u32										Width;
			u32										Height;
			E_GI_FORMAT								Format;
			bool									MultiSampling;
			u32										Count;
			u32										Quality;
			SRenderTargetParams()
				:Width(0), Height(0),
				MultiSampling(false), 
				Count(1), Quality(0), 
				Format(EGF_R32G32B32A32_FLOAT)
			{
			}
		};

		struct SDepthStencilSurfaceParams
		{
			std::string								Name;
			u32										Width;
			u32										Height;
			u32										DepthBits;
			u32										StencilBits;
			bool									MultiSampling;
			u32										Count;
			u32										Quality;
			bool									BindingShader;
			bool									BindDepthToShader;

			SDepthStencilSurfaceParams()
				:Width(0), Height(0), DepthBits(24), StencilBits(8),
				MultiSampling(false), Count(1), Quality(0),
				BindingShader(false), BindDepthToShader(true)
			{

			}
		};

	public:
		virtual bool extractSubNodeNames(const std::string& filepath, const char* rootNode,
			const char* subNode, std::vector<std::string>& vec) const = 0;

		virtual bool extractMaterialNames(const std::string& filepath, std::vector<std::string>& vec) const = 0;

		virtual bool extractPipelineNames(const std::string& filepath, std::vector<std::string>& vec) const = 0;

		virtual bool parsePipelineFile(const std::string& filepath, std::vector<SPipelineCreateParams>& createParamsArray) const = 0;

		virtual bool parseMaterialFile(const std::string& filepath, std::vector<SMaterialCreateParams>& createParamsArray) const = 0;

		virtual bool parseTextureXmlFile(const std::string& filepath, 
			std::vector<SRenderTargetParams>& renderTargetParamsArray, 
			std::vector<SDepthStencilSurfaceParams>& depthStencilParamsArray) const = 0;

		virtual bool parseOnePipeline(const std::string& filepath,
			const std::string& pipelineName,
			SPipelineCreateParams& createParams) const = 0;

		virtual bool parseOneMaterial(const std::string& filepath,
			const std::string& materialName,
			SMaterialCreateParams& createParams) const = 0;

		_DECLARE_SINGLETON_INSTANCE(IResourceXmlParser);
	};

}

#endif
