#ifndef __IPIPELINE_H__
#define __IPIPELINE_H__

#include "gfTypes.h"
#include "gfUtil.h"
#include "gfEnums.h"
#include "IShader.h"
#include "IInputLayout.h"
#include "IRenderState.h"

namespace gf
{

	enum E_PRIMITIVE_TYPE
	{
		EPT_UNDEFINED = 0,
		EPT_POINTLIST = 1,
		EPT_LINELIST = 2,
		EPT_LINESTRIP = 3,
		EPT_TRIANGLELIST = 4,
		EPT_TRIANGLESTRIP = 5,
		EPT_LINELIST_ADJ = 10,
		EPT_LINESTRIP_ADJ = 11,
		EPT_TRIANGLELIST_ADJ = 12,
		EPT_TRIANGLESTRIP_ADJ = 13,
		EPT_1_CONTROL_POINT_PATCHLIST = 33,
		EPT_2_CONTROL_POINT_PATCHLIST = 34,
		EPT_3_CONTROL_POINT_PATCHLIST = 35,
		EPT_4_CONTROL_POINT_PATCHLIST = 36,
		EPT_5_CONTROL_POINT_PATCHLIST = 37,
		EPT_6_CONTROL_POINT_PATCHLIST = 38,
		EPT_7_CONTROL_POINT_PATCHLIST = 39,
		EPT_8_CONTROL_POINT_PATCHLIST = 40,
		EPT_9_CONTROL_POINT_PATCHLIST = 41,
		EPT_10_CONTROL_POINT_PATCHLIST = 42,
		EPT_11_CONTROL_POINT_PATCHLIST = 43,
		EPT_12_CONTROL_POINT_PATCHLIST = 44,
		EPT_13_CONTROL_POINT_PATCHLIST = 45,
		EPT_14_CONTROL_POINT_PATCHLIST = 46,
		EPT_15_CONTROL_POINT_PATCHLIST = 47,
		EPT_16_CONTROL_POINT_PATCHLIST = 48,
		EPT_17_CONTROL_POINT_PATCHLIST = 49,
		EPT_18_CONTROL_POINT_PATCHLIST = 50,
		EPT_19_CONTROL_POINT_PATCHLIST = 51,
		EPT_20_CONTROL_POINT_PATCHLIST = 52,
		EPT_21_CONTROL_POINT_PATCHLIST = 53,
		EPT_22_CONTROL_POINT_PATCHLIST = 54,
		EPT_23_CONTROL_POINT_PATCHLIST = 55,
		EPT_24_CONTROL_POINT_PATCHLIST = 56,
		EPT_25_CONTROL_POINT_PATCHLIST = 57,
		EPT_26_CONTROL_POINT_PATCHLIST = 58,
		EPT_27_CONTROL_POINT_PATCHLIST = 59,
		EPT_28_CONTROL_POINT_PATCHLIST = 60,
		EPT_29_CONTROL_POINT_PATCHLIST = 61,
		EPT_30_CONTROL_POINT_PATCHLIST = 62,
		EPT_31_CONTROL_POINT_PATCHLIST = 63,
		EPT_32_CONTROL_POINT_PATCHLIST = 64

	};

	

	class IPipeline : public IReferenceCounted
	{
	public:

		virtual u32 setMatrix(const std::string& varname, const f32* m, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual bool setMatrix(E_SHADER_TYPE shaderType, const std::string& varname, const f32* m, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual u32 setMatrix(const std::string& varname, CXMMATRIX matrix, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual u32 setMatrix(E_SHADER_TYPE shaderType, const std::string& varname, CXMMATRIX matrix, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual u32 setMatrix(const std::string& varname, const XMFLOAT4X4& matrix, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual u32 setMatrix(E_SHADER_TYPE shaderType, const std::string& varname, const XMFLOAT4X4& matrix, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual bool setTransposedMatrixArray(E_SHADER_TYPE shaderType, const std::string& varname, const f32* matrixs, u32 count, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual bool setMatrixArray(E_SHADER_TYPE shaderType, const std::string& varname, XMFLOAT4X4 matrixs[], u32 count, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual u32 setVector(const std::string& varname, const f32* v, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual bool setVector(E_SHADER_TYPE shaderType, const std::string& varname, const f32* v, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual u32 setVector(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual bool setVector(E_SHADER_TYPE shaderType, const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual u32 setAttribute(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual bool setAttribute(E_SHADER_TYPE shaderType, const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual u32 setFloat(const std::string& varname, f32 v, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual bool setFloat(E_SHADER_TYPE shaderType, const std::string& varname, f32 v, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual u32 setUint(const std::string& varname, u32 v, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual bool setUint(E_SHADER_TYPE shaderType, const std::string& varname, u32 v, bool ignoreIfAlreadyUpdated = false) = 0;


		virtual u32 setRawValue(const std::string& varname, void* raw, u32 size, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual bool setRawValue(E_SHADER_TYPE shaderType, const std::string& varname, void* raw, u32 size, bool ignoreIfAlreadyUpdated = false) = 0;

		virtual u32 setArray(const std::string& varname, void* data, u32 arraySize, u32 elementSize, bool ignoreIfAlreadyUpdate = false) = 0;
		
		// return the actual element count that has been set.
		virtual u32 setArray(E_SHADER_TYPE shaderType, const std::string& varname, void* data, u32 arraySize, u32 elementSize, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual u32 setTexture(const std::string& varname, ITexture* texture) = 0;

		virtual bool setTexture(E_SHADER_TYPE shaderType, const std::string& varname, ITexture* texture) = 0;

		virtual IShader* getShader(E_SHADER_TYPE type) = 0;

		virtual IInputLayout* getInputLayout() = 0;

		virtual IRenderState* getRenderState() = 0;

		virtual E_PRIMITIVE_TYPE getPrimitiveType() const = 0;

		virtual void addShaderAutoVariable(const SShaderAutoVariable& var) = 0;

		

		virtual bool setSampler(const std::string& varname, ISampler* sampler) = 0;

		virtual void applySamplers() const = 0;

		virtual const std::vector<SShaderAutoVariable>& getShaderAutoVariables() const = 0;

		virtual void apply(E_PIPELINE_USAGE) = 0;

		virtual u64 getSortCode() const = 0;

		virtual const std::string& getName() const = 0;
	};
}

#endif

