#pragma once

#include <map>
#include "common/types.h"

#include "rage/dat/datBase.h"
#include "rage/paging/ref.h"

#include "rage/atl/atBinaryMap.h"
#include "rage/atl/atArray.h"

#include "rage/grcore/texture.h"
#include "rage/grm/shader.h"

#include "ptxKeyframe.h"


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

	struct grmVariableInfo
	{
		enum VarType {
			VT_NONE, VT_INT_DONTUSE, VT_FLOAT, VT_VECTOR2,				// 0-3
			VT_VECTOR3, VT_VECTOR4, VT_TEXTURE, VT_BOOL_DONTUSE,		// 4-7
			VT_MATRIX43, VT_MATRIX44, VT_STRING,						// 8-A

			VT_INT, VT_INT2, VT_INT3, VT_INT4, VT_STRUCTUREDBUFFER, VT_SAMPLERSTATE,

			VT_UNUSED1, VT_UNUSED2, VT_UNUSED3, VT_UNUSED4, //these are to be used for the particle system to add custom vertex buffer values
			VT_UAV_STRUCTURED, VT_UAV_TEXTURE,	// these only affect Compute Shaders
			
			VT_BYTEADDRESSBUFFER, VT_UAV_BYTEADDRESS,
			VT_COUNT /*Must be the last one*/
		};

		const char*	m_Name;			// Semantic name
		const char*	m_ActualName;	// Actual underlying parameter name, only on __WIN32PC builds.
		VarType		m_Type;
		const char*	m_TypeName;
		const char*	m_UiName;		// Name to display in widgets
		const char*	m_UiWidget;		// Used to describe what type of widget to display
		const char*	m_UiHelp;		// Extra info for widget tool tips
		const char*	m_UiHint;
		const char*	m_AssetName;	// Primarily useful for hardcoding texture filenames
		const char*	m_GeoValueSource;//The source of the value ("instance","type","template") "instance" is the assumed default
		float		m_UiMin;		// Minimum allowed value for a widget
		float		m_UiMax;		// Maximum allowed value for a widget
		float		m_UiStep;		// Step size
		bool		m_UiHidden;		// Flag for if this variable should be hidden from the UI.
		int			m_UvSetIndex;	// Used for texture variables to formally declare which UV set index the shader uses
		const char*	m_UvSetName;	// Used for texture variables to formally declare which UV set name the exporter should use
		const char*	m_TextureOutputFormats;		// The output formats of the texture, this is a ; separated list of 'platform'='format' pairs
		int			m_iNoOfMipLevels;			// No of mip map levels to create (If this value is zero, a complete mipmap chain is created.  This also the default.)
		int			m_MaterialDataUvSetIndex; // Used for misc. material data that needs to get pushed into the vertex buffer. -1 means not using
		int			m_ArrayCount;	// Number of elements in the array
		bool		m_IsMaterial;
	};


	struct ptxShaderVar : public datBase
	{
		virtual ~ptxShaderVar() {};

		grmVariableInfo* m_pInfo;			//grmVariableInfo
		u32 m_hashName;
		ptxShaderVarType m_type;
		u32 m_id;
		bool m_isKeyframeable;
		bool m_ownsInfo;
		u8 pad[10];

		void UiWidgets(size_t entryIdx, size_t varIdx);
		const char* GetTypeStr();
	};

	struct ptxShaderVarVector : public ptxShaderVar
	{
		u8 pad[8];
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