#pragma once
#include <map>
#include "common/types.h"
#include "rage/atl/atArray.h"
#include "rage/dat/datBase.h"
#include "rage/paging/ref.h"


namespace rage
{
	enum ptxDiffuseMode
	{
		PTXDIFFUSEMODE_FIRST = 0,

		PTXDIFFUSEMODE_TEX1_RGBA = 0,
		PTXDIFFUSEMODE_TEX1_RRRR,
		PTXDIFFUSEMODE_TEX1_GGGG,
		PTXDIFFUSEMODE_TEX1_BBBB,
		PTXDIFFUSEMODE_TEX1_RGB,
		PTXDIFFUSEMODE_TEX1_RG_BLEND,

		PTXDIFFUSEMODE_NUM
	};

	enum ptxProjectionMode
	{
		PTXPROJMODE_NONE = 0,
		PTXPROJMODE_WATER,
		PTXPROJMODE_NON_WATER,
		PTXPROJMODE_ALL,

		PTXPROJMODE_NUM
	};

	enum ptxShaderVarType
	{
		PTXSHADERVAR_BOOL = 0,				// not used at the moment - could be removed but will need to change the editor code and data to match
		PTXSHADERVAR_INT,								// not used at the moment - could be removed but will need to change the editor code and data to match
		PTXSHADERVAR_FLOAT,
		PTXSHADERVAR_FLOAT2,
		PTXSHADERVAR_FLOAT3,
		PTXSHADERVAR_FLOAT4,
		PTXSHADERVAR_TEXTURE,
		PTXSHADERVAR_KEYFRAME,

		PTXSHADERVAR_COUNT,
		PTXSHADERVAR_INVALID,
	};


	struct ptxTechniqueDesc
	{
		virtual ~ptxTechniqueDesc() {};

		ptxDiffuseMode m_diffuseMode;
		ptxProjectionMode m_projMode;
		bool m_isLit;
		bool m_isSoft;
		bool m_isScreenSpace;
		bool m_isRefract;
		bool m_isNormalSpec;
		u8 pad[3];
	};

	struct alignas(16) ptxShaderVar : public datBase
	{
		virtual ~ptxShaderVar() {};

		const char* GetTypeStr()
		{
			static std::map<ptxShaderVarType, const char*> typesMap = {
				{ PTXSHADERVAR_BOOL, "bool" },
				{ PTXSHADERVAR_INT, "int" },
				{ PTXSHADERVAR_FLOAT, "float" },
				{ PTXSHADERVAR_FLOAT2, "float2" },
				{ PTXSHADERVAR_FLOAT3, "float3" },
				{ PTXSHADERVAR_FLOAT4, "float4" },
				{ PTXSHADERVAR_TEXTURE, "texture" },
				{ PTXSHADERVAR_KEYFRAME, "keyframe" }
			};

			auto it = typesMap.find(m_type);
			return it == typesMap.end() ? "unknown" : it->second;
		}

		void* m_pInfo;			//grmVariableInfo
		u32 m_hashName;
		ptxShaderVarType m_type;
		u32 m_id;
		bool m_isKeyframeable;
		bool m_ownsInfo;
		u8 pad[18];
	};

	struct ptxShaderVarVector : public ptxShaderVar
	{
		rage::Vector4 m_vector;
	};

	struct ptxShaderVarKeyframe : public ptxShaderVar
	{
		ptxKeyframe m_keyframe;
	};

	struct ptxShaderVarTexture : public ptxShaderVar
	{
		void* m_pTexture;			//pgRef<grcTexture>
		const char* m_textureName;
		u32 m_textureHashName;
		bool m_externalReference;
		u8 pad[3];
	};


	struct ptxShaderInst
	{
		virtual ~ptxShaderInst() {};

		const char* m_shaderTemplateName;
		const char* m_shaderTemplateTechniqueName;
		void* m_pShaderTemplate;						//ptxShaderTemplate
		u32 m_shaderTemplateTechniqueId;
		
		u32 pad;

		ptxTechniqueDesc m_techniqueDesc;

		atArray<datOwner<ptxShaderVar>> m_instVars;		// instanced variables - does *NOT* include UI info from shader

		bool m_isDataInSync;							// flag to determine if data matches shader
		u8 m_pad[7];

		u32 m_shaderTemplateHashName;
	};
}