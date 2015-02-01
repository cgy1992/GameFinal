#ifndef __ISHADER_H__
#define __ISHADER_H__

#include "gfTypes.h"
#include "gfUtil.h"
#include "ISampler.h"
#include "ITexture.h"
#include <vector>
#include <string>
#include <iostream>

namespace gf
{
	class IPipeline;
	struct SShaderVariableAttribute;

	enum E_SHADER_TYPE
	{
		EST_UNDEFINED_SHADER = -1,
		EST_VERTEX_SHADER = 0,
		EST_GEOMETRY_SHADER,
		EST_HULL_SHADER,
		EST_DOMAIN_SHADER,
		EST_PIXEL_SHADER,
		EST_COMPUTE_SHADER,
	};

	enum E_SHADER_COUNT
	{
		EST_SHADER_COUNT = 5
	};

	enum E_SHADER_VARIABLE_TYPE
	{
		ESVT_NOT_EXIST,
		ESVT_UNKNOWN,
		ESVT_INT,
		ESVT_UINT,
		ESVT_FLOAT,
		ESVT_FLOAT2,
		ESVT_FLOAT3,
		ESVT_FLOAT4,
		ESVT_FLOAT2X2,
		ESVT_FLOAT2X3,
		ESVT_FLOAT3X2,
		ESVT_FLOAT3X3,
		ESVT_FLOAT3X4,
		ESVT_FLOAT4X3,
		ESVT_FLOAT4X4,
	};

	//param_indexed_auto
	enum E_SHADER_AUTO_VARIABLE_TYPE
	{
		/* transforms */
		ESAVT_TRANSFORMS_BEGIN,							/* transform begin */
		ESAVT_WORLD_MATRIX,
		ESAVT_INVERSE_WORLD_MATRIX,
		ESAVT_TRANSPOSE_WORLD_MATRIX,
		ESAVT_INVERSE_TRANSPOSE_WORLD_MATRIX,
		ESAVT_VIEW_MATRIX,
		ESAVT_INVERSE_VIEW_MATRIX,
		ESAVT_TRANSPOSE_VIEW_MATRIX,
		ESAVT_INVERSE_TRANSPOSE_VIEW_MATRIX,
		ESAVT_PROJ_MATRIX,
		ESAVT_INVERSE_PROJ_MATRIX,
		ESAVT_TRANSPOSE_PROJ_MATRIX,
		ESAVT_INVERSE_TRANSPOSE_PROJ_MATRIX,
		ESAVT_WORLD_VIEW_MATRIX,
		ESAVT_INVERSE_WORLD_VIEW_MATRIX,
		ESAVT_TRANSPOSE_WORLD_VIEW_MATRIX,
		ESAVT_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX,
		ESAVT_VIEW_PROJ_MATRIX,
		ESAVT_INVERSE_VIEW_PROJ_MATRIX,
		ESAVT_TRANSPOSE_VIEW_PROJ_MATRIX,
		ESAVT_INVERSE_TRANSPOSE_VIEW_PROJ_MATRIX,
		ESAVT_WORLD_VIEW_PROJ_MATRIX,
		ESAVT_INVERSE_WORLD_VIEW_PROJ_MATRIX,
		ESAVT_TRANSPOSE_WORLD_VIEW_PROJ_MATRIX,
		ESAVT_INVERSE_TRANSPOSE_WORLD_VIEW_PROJ_MATRIX,
		ESAVT_TRANSFORMS_END,							/* transform end */

		/* materials */
		ESAVT_MATERIAL_BEGIN,					/* material begin */
		ESAVT_MATERIAL_COLOR,
		ESAVT_MATERIAL_AMBIENT,
		ESAVT_MATERIAL_DIFFUSE,
		ESAVT_MATERIAL_SPECULAR,
		ESAVT_MATERIAL_EMISSIVE,
		ESAVT_MATERIAL_ATTRIBUTE,
		ESAVT_TEXTURE,
		ESAVT_TEXTURE_WIDTH,
		ESAVT_TEXTURE_HEIGHT,
		ESAVT_INVERSE_TEXTURE_WIDTH,
		ESAVT_INVERSE_TEXTURE_HEIGHT,
		ESAVT_MATERIAL_END,						/* material end */

		/* scene info */
		ESAVT_SCENE_BEGIN,						/* scene info begin */
		ESAVT_CAMERA_POSITION,
		ESAVT_CAMERA_FRUSTUM,
		ESAVT_CAMERA_SEGMENTS,
		ESAVT_LIGHT,
		ESAVT_AMBIENT,
		ESAVT_NEAR_POINT_LIGHTS,
		ESAVT_NEAR_POINT_LIGHTS_NUM,
		ESAVT_NEAR_SPOT_LIGHTS,
		ESAVT_NEAR_SPOT_LIGHTS_NUM,
		ESAVT_DIRECTIONAL_LIGHTS,
		ESAVT_DIRECTIONAL_LIGHTS_NUM,
		ESAVT_SHADOW_MAP,
		ESAVT_SHADOW_MAP_TRANSFORM,
		ESAVT_SHADOW_MAP_TRANSFORMS,
		ESAVT_SHADOW_MAP_SIZE,
		ESAVT_SHADOW_MAP_JITTER_TEXTURE,
		ESAVT_PL_SHADOW_MAP_JITTER_TEXTURE,
		ESAVT_SCENE_END,						/* scene info end*/

		/* bone transforms. */
		ESAVT_BONE_TRANSFORMS,

		/* terrain */
		ESAVT_TERRAIN_BEGIN,					/* terrain begin */
		ESAVT_TERRAIN_ROW_CELL,
		ESAVT_INVERSE_TERRAIN_ROW_CELL,
		ESAVT_TERRAIN_ROW_VERTEX,
		ESAVT_TERRAIN_HEIGHT_SCALE,
		ESAVT_TERRAIN_VERTEX_SPACE,
		ESAVT_TERRAIN_TEXCOORD_SCALE,
		ESAVT_TERRAIN_HEIGHTMAP_TEXTURE,
		ESAVT_TERRAIN_WIDTH,
		ESAVT_TERRAIN_PATCH_WIDTH,
		ESAVT_TERRAIN_ROW_PATCH,
		ESAVT_TERRAIN_END,						/* terrain end */

		/* window and system */

		ESAVT_WINDOW_SYSTEM_BEGIN,				/* window and system begin */
		ESAVT_VIEWPORT_WIDTH,
		ESAVT_VIEWPORT_HEIGHT,
		ESAVT_INVERSE_VIEWPORT_WIDTH,
		ESAVT_INVERSE_VIEWPORT_HEIGHT,
		ESAVT_WINDOW_WIDTH,
		ESAVT_WINDOW_HEIGHT,
		ESAVT_INVERSE_WINDOW_WIDTH,
		ESAVT_INVERSE_WINDOW_HEIGHT,
		ESAVT_SCREEN_SIZE,
		ESAVT_WINDOW_SYSTEM_END,				/* window and system end */
	};

	enum E_UPDATE_FREQUENCY
	{
		EUF_PER_OBJECT,
		EUF_PER_FRAME
	};

	struct SShaderAutoVariable
	{
		std::string						VariableName;
		E_SHADER_AUTO_VARIABLE_TYPE		Type;
		E_SHADER_TYPE					ShaderType;
		E_UPDATE_FREQUENCY				UpdateFrequency;
		u32								IndexParam;
		std::string						MaterialAttributeName;
	};

	struct SShaderSamplerVariable
	{
		std::string						VariableName;
		u32								ShaderTypes;
		ISampler*						Sampler;
	};

	struct SShaderVariableAttribute
	{
		E_SHADER_AUTO_VARIABLE_TYPE		Meaning;
		E_UPDATE_FREQUENCY				DefaultUpdateFrequency;
		u32								IndexParam;

		SShaderVariableAttribute(){}

		SShaderVariableAttribute(E_SHADER_AUTO_VARIABLE_TYPE meaning,
			E_UPDATE_FREQUENCY freq = EUF_PER_OBJECT,
			u32 index = 0)
			:Meaning(meaning), DefaultUpdateFrequency(freq), IndexParam(index)
		{

		}
	};

	struct SShaderMacro
	{
		std::string			Name;
		std::string			Definition;

		bool operator==(const SShaderMacro& macro) const
		{
			return Name == macro.Name && Definition == macro.Definition;
		}

		bool operator!=(const SShaderMacro& macro) const
		{
			return !(*this == macro);
		}
	};

	struct SShaderMacroSet
	{
		class Iterator;

	public:
		u32 size() const
		{
			return mShaderMacros.size();
		}

		void reset()
		{
			mShaderMacros.clear();
		}

		void set(const std::string& name, const std::string& definition)
		{
			for (auto it = mShaderMacros.begin(); it != mShaderMacros.end(); it++)
			{
				if (it->Name == name)
				{
					it->Definition = definition;
					return;
				}
			}

			SShaderMacro macro;
			macro.Name = name;
			macro.Definition = definition;
			mShaderMacros.push_back(macro);
		}

		bool remove(const std::string& name)
		{
			for (auto it = mShaderMacros.begin(); it != mShaderMacros.end(); it++)
			{
				if (it->Name == name)
				{
					mShaderMacros.erase(it);
					return true;
				}
			}
			return false;
		}

		const char* get(const std::string& name) const
		{
			for (auto it = mShaderMacros.begin(); it != mShaderMacros.end(); it++)
			{
				if (it->Name == name)
				{
					return it->Definition.c_str();
				}
			}
			return nullptr;
		}

		bool exist(const std::string name)
		{
			for (u32 i = 0; i < mShaderMacros.size(); i++)
			{
				if (mShaderMacros[i].Name == name)
					return true;
			}
			return false;
		}

		bool operator==(const SShaderMacroSet& macroSet) const
		{
			if (macroSet.mShaderMacros.size() != mShaderMacros.size())
				return false;
			
			for (u32 i = 0; i < mShaderMacros.size(); i++)
			{
				const char* value = macroSet.get(mShaderMacros[i].Name);
				if (!value)
					return false;

				if (_stricmp(mShaderMacros[i].Definition.c_str(), value) != 0)
					return false;
			}

			return true;
		}

		Iterator getIterator() const
		{
			return Iterator(mShaderMacros);
		}

	private:
		std::vector<SShaderMacro> mShaderMacros;

	public:

		class Iterator
		{
		public:
			Iterator(const std::vector<SShaderMacro>& macros)
				:mMacros(macros)
				, mCurIndex(0) {}

			bool hasNext()
			{
				return mCurIndex < mMacros.size();
			}

			const SShaderMacro& next()
			{
				return mMacros[mCurIndex++];
			}

		private:
			u32									mCurIndex;
			const std::vector<SShaderMacro>&	mMacros;
		};
	};

	class IShader : public IReferenceCounted
	{
	public:
		IShader(u32 sortCode, const std::string& name, const SShaderMacroSet& macros)
			: mSortCode(sortCode)
			, mName(name)
			, mMacros(macros)
		{

		}

		const std::string& getName() const
		{
			return mName;
		}

		virtual E_SHADER_TYPE getType() const = 0;

		virtual void submit() = 0;

		virtual bool setRawData(const std::string& varname, void* data, u32 size, bool ignoreIfAlreadyUpdate = false) = 0;

		// return the actual element count that has been set.
		virtual u32 setArray(const std::string& varname, void* data, u32 arraySize, u32 elementSize, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setUint(const std::string& varname, u32 val, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setFloat(const std::string& varname, f32 val, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setVector(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setVector(const std::string& varname, const f32* val, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setAttribute(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdate = false) = 0;

		//	virtual bool setVector3(const std::string& varname, const f32* val, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setMatrix(const std::string& varname, const f32* matrix, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setMatrix(const std::string& varname, CXMMATRIX matrix, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setMatrix(const std::string& varname, const XMFLOAT4X4& matrix, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setTexture(const std::string& varname, ITexture* texture) = 0;

		virtual bool setTransposedMatrixArray(const std::string& var, const f32* matrixs, u32 count, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setMatrixArray(const std::string& var, XMFLOAT4X4 matrixs[], u32 count, bool ignoreIfAlreadyUpdate = false) = 0;

		virtual bool setSampler(const std::string& varname, ISampler* sampler) = 0;

		virtual bool existSampler(const std::string& varname) const = 0;

		virtual void update(bool forceUpdate = false) = 0;

		virtual bool isAlreadyUpdated(const std::string& varname) = 0;

		u32 getSortCode() const
		{
			return mSortCode;
		}

		virtual u32 getTextureCount() const = 0;

		virtual u32 getRWTextureCount() const = 0;

		virtual u32 getSamplerCount() const = 0;

		virtual void reset() = 0;

		virtual bool isContantVariable(const std::string& varname) const = 0;

		//virtual bool isTextureVariable(const std::string& varname) const = 0;

		virtual void registerAutoVariables(const std::map<std::string, SShaderVariableAttribute>& varMap) = 0;

		virtual void addShaderAutoVariable(const SShaderAutoVariable& var) = 0;

		virtual void registerAutoSamplers(const std::map<std::string, ISampler*>& samplerMap) = 0;

		virtual void applySamplers() = 0;

		virtual const std::vector<SShaderAutoVariable>& getShaderAutoVariables() const = 0;

		const SShaderMacroSet& getMacros() const
		{
			return mMacros;
		}

		virtual ~IShader()
		{

		}

	protected:
		u32										mSortCode;
		std::string								mName;
		const SShaderMacroSet					mMacros;
		
	};
}

#endif


