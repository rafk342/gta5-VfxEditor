#pragma once
#include <map>
#include "common/types.h"

#include "rage/dat/datBase.h"
#include "rage/paging/ref.h"

#include "rage/atl/atBinaryMap.h"
#include "rage/atl/atArray.h"

#include "rage/grcore/texture.h"
#include "rage/grm/shader.h"

#include "ImGui/imgui.h"

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
		PTXSHADERVAR_BOOL = 0,
		PTXSHADERVAR_INT,
		PTXSHADERVAR_FLOAT,
		PTXSHADERVAR_FLOAT2,
		PTXSHADERVAR_FLOAT3,
		PTXSHADERVAR_FLOAT4,
		PTXSHADERVAR_TEXTURE,
		PTXSHADERVAR_KEYFRAME,

		PTXSHADERVAR_COUNT,
		PTXSHADERVAR_INVALID,
	};

	enum ptxShaderProgVarType
	{
		PTXPROGVAR_FIRST = 0,

		PTXPROGVAR_FRAMEMAP = 0,
		PTXPROGVAR_BLENDMODE,
		PTXPROGVAR_CHANNELMASK,
		PTXPROGVAR_RMPTFX_LAST = PTXPROGVAR_CHANNELMASK,
		PTXPROGVAR_GAME_FIRST,
		PTXPROGVAR_GAME_VAR_1 = PTXPROGVAR_GAME_FIRST,
		PTXPROGVAR_GAME_VAR_2,
		PTXPROGVAR_GAME_VAR_3,
		PTXPROGVAR_GAME_VAR_4,
		PTXPROGVAR_GAME_VAR_5,
		PTXPROGVAR_GAME_VAR_6,
		PTXPROGVAR_GAME_VAR_7,
		PTXPROGVAR_GAME_VAR_8,
		PTXPROGVAR_GAME_VAR_9,
		PTXPROGVAR_COUNT,
		PTXPROGVAR_RMPTFX_COUNT = PTXPROGVAR_RMPTFX_LAST + 1,
		PTXPROGVAR_GAME_COUNT = PTXPROGVAR_COUNT - PTXPROGVAR_RMPTFX_COUNT
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

		void* m_pInfo;			//grmVariableInfo
		u32 m_hashName;
		ptxShaderVarType m_type;
		u32 m_id;
		bool m_isKeyframeable;
		bool m_ownsInfo;
		u8 pad[18];



		void UiWidgets()
		{
			ImGui::Text("Name: %s", m_pInfo);
			ImGui::Text("Type: %s", GetTypeStr());
			ImGui::Text("Hash: %u", m_hashName);
			ImGui::Text("ID: %u", m_id);
			ImGui::Text("Keyframeable: %s", m_isKeyframeable ? "true" : "false");

			switch (m_type)
			{
			case rage::PTXSHADERVAR_BOOL:
				break;
			case rage::PTXSHADERVAR_INT:
				break;
			case rage::PTXSHADERVAR_FLOAT:
				break;
			case rage::PTXSHADERVAR_FLOAT2:
				break;
			case rage::PTXSHADERVAR_FLOAT3:
				break;
			case rage::PTXSHADERVAR_FLOAT4:
				break;
			case rage::PTXSHADERVAR_TEXTURE:
				break;
			case rage::PTXSHADERVAR_KEYFRAME:
				break;
			case rage::PTXSHADERVAR_COUNT:
				break;
			case rage::PTXSHADERVAR_INVALID:
				break;
			default:
				break;
			}
		}

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
		pgRef<grcTexture> m_pTexture;	
		const char* m_textureName;
		u32 m_textureHashName;
		bool m_externalReference;
		u8 pad[3];
	};

	struct ptxShaderTemplateOverride
	{
		struct techniqueOverride
		{
			bool bDisable;
			bool bOverride;
			u16 techniqueId;
		};

		atArray<techniqueOverride> m_techniqueOverrides;
	};

	struct ptxShaderTemplate
	{
		enum PTXTemplatePass
		{
			PTX_DEFAULT_PASS = 0,
			PTX_SHADOW_PASS,
			PTX_INSTSHADOW_PASS,
			PTX_MAX_PASS
		};

		virtual ~ptxShaderTemplate() {};

		grmShader* m_pShader;
		char m_shaderName[128];

		atBinaryMap<u32, ptxShaderVar*> m_shaderVars;
		u32 m_progVars[PTXPROGVAR_COUNT];
		ptxShaderTemplateOverride* m_pShaderOverride[1];
		s32 m_BlendSet[1];
		PTXTemplatePass m_RenderPassToUse[1];
	};

	struct ptxShaderInst
	{
		virtual ~ptxShaderInst() {};

		const char* m_shaderTemplateName;
		const char* m_shaderTemplateTechniqueName;
		ptxShaderTemplate* m_pShaderTemplate;
		u32 m_shaderTemplateTechniqueId;
		
		u32 pad;

		ptxTechniqueDesc m_techniqueDesc;

		atArray<datOwner<ptxShaderVar>> m_instVars;

		bool m_isDataInSync;							// flag to determine if data matches shader
		u8 m_pad[7];

		u32 m_shaderTemplateHashName;
	};
}