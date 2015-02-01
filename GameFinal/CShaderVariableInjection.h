#ifndef __SHADER_VARIABLE_INJECTION_CLASS_H__
#define __SHADER_VARIABLE_INJECTION_CLASS_H__

#include "IShaderVariableInjection.h"
#include "ISceneManager.h"
#include "ILightNode.h"
#include "ITerrainMesh.h"

namespace gf
{
	class CShaderVariableInjection
	{
	public:

		CShaderVariableInjection(){}

		static void inject(IMeshNode* mesh, IPipeline* pipeline, u32 subset = 0);

		static void injectToComputeShader(IShader* shader, ISceneNode* node);

		static void injectBoneTransforms(IPipeline* pipeline, const f32* matrixs, u32 count);

		static void injectMaterial(IMaterial* material, IPipeline* pipeline);

		/* inject just one variable*/
		static void injectVariable(const SShaderAutoVariable& var, IMeshNode* mesh, 
			IPipeline* pipeline, IShader* shader, u32 subset);

		/* inject world, view, proj transforms. */
		static void injectTransformations(const SShaderAutoVariable& var, ISceneNode* node, IShader* shader);

		/* inject materials. */
		static void injectMaterial(const SShaderAutoVariable& var, IShader* shader, IMaterial* material);

		/* inject scene information */
		static void injectSceneInfo(const SShaderAutoVariable& var, ISceneNode* node, IPipeline* pipeline, IShader* shader);

		/* inject window and system information */
		static void injectWindowSystemInfo(const SShaderAutoVariable& var, IShader* shader);

		static void injectTerrainVariable(const SShaderAutoVariable& var, ITerrainMesh* mesh, IShader* shader);


	private:

		static void injectLightSourceNum(IPipeline* pipeline, 
			IShader* shader,
			E_SHADER_AUTO_VARIABLE_TYPE shaderVariableType,
			u32 lightCount);

	};
}

#endif
