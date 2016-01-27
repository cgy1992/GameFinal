#ifndef __RESOURCE_XML_PARSER_CLASS_H__
#define __RESOURCE_XML_PARSER_CLASS_H__

#include "IResourceXmlParser.h"
#include "gfGIFormat.h"
#include "tinyxml/tinyxml2.h"
namespace gf
{
	class CResourceXmlParser : public IResourceXmlParser
	{

	public:
		CResourceXmlParser();

		virtual bool parsePipelineFile(const std::string& filepath, std::vector<SPipelineCreateParams>& createParamsArray) const;

		virtual bool parseMaterialFile(const std::string& filepath, std::vector<SMaterialCreateParams>& createParamsArray) const;

		virtual bool parseOnePipeline(const std::string& filepath,
			const std::string& pipelineName, 
			SPipelineCreateParams& createParams) const;

		virtual bool parseOneMaterial(const std::string& filepath,
			const std::string& materialName,
			SMaterialCreateParams& createParams) const;

		virtual bool parseTextureXmlFile(const std::string& filepath, 
			std::vector<SRenderTargetParams>& renderTargetParamsArray,
			std::vector<SDepthStencilSurfaceParams>& depthStencilParamsArray) const;

		virtual bool extractSubNodeNames(const std::string& filepath, const char* rootNode, 
			const char* subNode, std::vector<std::string>& vec) const;

		virtual bool extractMaterialNames(const std::string& filepath, std::vector<std::string>& vec) const;

		virtual bool extractPipelineNames(const std::string& filepath, std::vector<std::string>& vec) const;

	private:

		virtual bool handlePipelineNode(const std::string& filepath, tinyxml2::XMLElement* node, SPipelineCreateParams& createParams) const;

		E_PIPELINE_USAGE getPipelineUsage(const char* s) const;

		E_PRIMITIVE_TYPE getPrimitiveType(std::string& s) const;

		E_GI_FORMAT		getInputLayoutFormat(std::string& s) const;

		E_RENDER_STATE	getRenderStateType(std::string& s) const;

		bool			getRenderStateValue(std::string& s, const std::map<std::string, u32> mapping, u32& value) const;

		bool			getBoolValue(const char* s, bool& value) const;

		bool			getBoolValue(const char* s) const;

		bool			getMaskValue(const char* s, u32& value) const;

		/* this function just tell the char is a valid charactor to form a float. */
		bool			isFloatCharactor(char c) const { return (c >= '0' && c <= '9') || c == '.'; }

		


		bool handleInputLayoutElementNode(const std::string& filepath,
			tinyxml2::XMLElement* node,
			SInputLayoutElement& element) const;

		bool handleShaderNode(const std::string& filepath,
			E_SHADER_TYPE shaderType,
			tinyxml2::XMLElement* node,
			SPipelineCreateParams& createParams) const;

		bool handleShaderVariableNode(const std::string& filepath,
			E_SHADER_TYPE shaderType,
			tinyxml2::XMLElement* node,
			SShaderCreateParams& createParams) const;

		bool handleShaderMacroNode(E_SHADER_TYPE shaderType,
			tinyxml2::XMLElement* node, 
			SShaderCreateParams& createParams) const;

		bool handleRenderStateNode(const std::string& filepath,
			tinyxml2::XMLElement* node,
			SPipelineCreateParams& createParams) const;

		bool handleSamplerNode(const std::string& filepath,
			tinyxml2::XMLElement* node,
			SPipelineCreateParams& createParams) const;

		bool handleMaterialNode(const std::string& filepath,
			tinyxml2::XMLElement* node,
			SMaterialCreateParams& createParams) const;

		bool handleRenderTargetNode(const std::string& filepath,
			tinyxml2::XMLElement* node,
			SRenderTargetParams& createParams) const;

		bool handleDepthStencilSurfaceNode(const std::string& filepath,
			tinyxml2::XMLElement* node,
			SDepthStencilSurfaceParams& createParams) const;


		XMFLOAT4 getVectorFromString(const char* s) const;

		bool handleMaterialAttributeNode(const std::string& filepath,
			tinyxml2::XMLElement* node,
			SMaterialCreateParams& createParams) const;

		bool handleMaterialPipelineNode(const std::string& filepath,
			tinyxml2::XMLElement* node,
			SMaterialCreateParams& createParams) const;

		/* handle <texture> element in material file. */
		bool handleMaterialTextureNode(const std::string& filepath,
			const std::string& materialName,
			tinyxml2::XMLElement* node, SMaterialCreateParams& createParams) const;


		/* 顶点格式的文本形式与枚举类型的映射 */
		std::map<std::string, E_GI_FORMAT>				mVertexFormatMapping;

		/* 图元类型的文本形式与枚举类型的映射 */
		std::map<std::string, E_PRIMITIVE_TYPE>			mPrimitiveTypeMapping;

		/* 自动注入Shader变量的映射 */
		std::map<std::string, SShaderVariableAttribute>	mShaderVariableMapping;

		/* 绘制状态文本形式与枚举类型的映射 */
		std::map<std::string, E_RENDER_STATE>			mRenderStateMapping;

		/* E_FILL_MODE 类型的文本形式与枚举类型的映射 */
		std::map<std::string, u32>						mFillModeMapping;

		/* E_CULL_MODE 类型的文本形式与枚举类型的映射 */
		std::map<std::string, u32>						mCullModeMapping;

		/* E_BLEND_TYPE 类型的文本形式与枚举类型的映射 */
		std::map<std::string, u32>						mBlendTypeMapping;

		/* E_BLEND_OP 类型的文本形式与枚举类型的映射 */
		std::map<std::string, u32>						mBlendOpMapping;

		/* E_COMPARISON_FUNC 类型的文本形式与枚举类型的映射 */
		std::map<std::string, u32>						mComparisonFuncMapping;

		/* E_STENCIL_OP 类型的文本形式与枚举类型的映射 */
		std::map<std::string, u32>						mStencilOpMapping;

		/* E_ADDRESS_MODE 类型的文本形式与枚举类型的映射 */
		std::map<std::string, E_ADDRESS_MODE>			mAddressModeMapping;

		/* E_SAMPLER_FILTER 类型的文本形式与枚举类型的映射 */
		std::map<std::string, E_SAMPLER_FILTER>			mSamplerFilterMapping;

		/* E_PIPELINE_USAGE 类型的文本形式与枚举类型的映射 */
		std::map<std::string, E_PIPELINE_USAGE>			mPipelineUsageMapping;
	};
}

#endif
