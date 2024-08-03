#pragma once
#include <d3d11.h>
#include <wrl.h>

#include "rage/grcore/fvf.h"
#include "common/types.h"
#include "rage/atl/atArray.h"

namespace _system
{ 
	struct sysMemContainerData
	{
		void*	Block; // Has to be allocated on heap
		u32		Size;
	};

}

using Microsoft::WRL::ComPtr;
namespace rage
{
	enum grcEffectVarType
	{
		VT_NONE				   = 0,	 // 
		VT_INT				   = 1,	 // 4 bytes
		VT_FLOAT			   = 2,	 // 4 bytes
		VT_VECTOR2			   = 3,	 // 8 bytes
		VT_VECTOR3			   = 4,	 // 16 bytes (used only 3)
		VT_VECTOR4			   = 5,	 // 16 bytes
		VT_TEXTURE			   = 6,	 // grcTexture*, 8 bytes
		VT_BOOL				   = 7,	 // 1 byte
		VT_MATRIX34			   = 8,	 // 
		VT_MATRIX44			   = 9,  // 64 bytes, 4x4 matrix (see vehicle_tire.fxc)
		VT_STRING			   = 10, // 
		VT_INT1				   = 11, // int[1]
		VT_INT2				   = 12, // int[2]
		VT_INT3				   = 13, // int[3]
		VT_INT4				   = 14, // int[4]
		VT_STRUCTURED_BUFFER   = 15,
		VT_SAMPLER_STATE	   = 16,
		VT_UNUSED1			   = 17,
		VT_UNUSED2			   = 18,
		VT_UNUSED3			   = 19,
		VT_UNUSED4			   = 20,
		VT_UAV_STRUCTURED	   = 21,
		VT_UAV_TEXTURE		   = 22,
		VT_BYTE_ADDRESS_BUFFER = 23,
		VT_UAV_BYTE_ADDRESS	   = 24,
		VT_COUNT			   = 25,
	};

	enum grcShaderType
	{
		grcShaderVS,
		grcShaderPS,
		grcShaderCS,
		grcShaderDS,
		grcShaderGS,
		grcShaderHS,
		grcShaderCount,
	};

	class grcInstanceData;

	struct grcEffectPass
	{
		uint8_t VS;
		uint8_t PS;
		uint8_t CS;
		uint8_t DS;
		uint8_t GS;
		uint8_t HS;
		uint8_t uint6;
		uint8_t uint7;
		uint8_t uint8;
		uint8_t uint9;
		uint8_t uintA;
		uint8_t uintB;
	};

	struct grcEffectTechnique
	{
		u32 m_NameHash;
		ConstString m_Name;
		grcEffectPass* m_Passes;
		uint16_t m_PassCount;
		int16_t unk1A;
		int16_t unk1C;
		int16_t unk20;
	};

	struct grcInstanceVar
	{
		u8 ValueCount;
		u8 Register;
		u8 SamplerIndex;
		u8 SavedSamplerIndex;

		union
		{
			char*  Value;
			int	   Int;
			float  Float;
			void*  Texture;	//grcTexture
		};
	};

	class grcInstanceData
	{
	protected:
		enum
		{
			FLAG_OWN_MATERIAL = 1 << 0,
		};

		grcInstanceVar* m_Vars;

		union
		{
			void* Ptr;	//grcEffect*
			u32			NameHash;
		} m_Effect;

		u8  m_VarCount;
		u8  m_DrawBucket;
		u8  m_PhysMtl;
		u8  m_Flags;
		u16 m_VarsSize;
		u16 m_TotalSize;

		union
		{
			grcInstanceData* Ptr;
			u32				 NameHash;
			ConstString		 Name;
		} m_Preset;

		u32			m_DrawBucketMask;
		bool        m_IsInstancedDraw;
		u8          m_UserFlags;
		u8          m_Padding;
		u8          m_TextureCount;
		u64         m_SortKey;

	};

	struct grcEffectVar
	{
		struct Annotation
		{
			enum AnnoType
			{
				AT_INT,
				AT_FLOAT,
				AT_STRING
			};

			AnnoType Type;
			u32		 NameHash;
			union
			{
				int			  Int;
				float		  Float;
				const char* String;
			};
		};
	};

	class grcCBuffer
	{
		struct Header
		{
			u32  Reserved;
			bool IsDirty;
			bool IsLocked;
			bool IsFirstUse;
		};

		u32                    m_Size;
		u16                    m_ShaderSlots[grcShaderCount];
		u32                    m_NameHash;
		const char*            m_Name;
		ComPtr<ID3D11Buffer>   m_BufferObject;

		// **************************************  // Local copy of the buffer, not really used on pc
		char*                  m_AlignedBackingStore;	// Aligned to cache line
		char*                  m_AllocatedBackingStore;

		u32                    m_BufferStride;		// Used only for multithreaded rendering, where we have buffer per thread
		                                            // on win32 D3D11 there's only single rendering thread
	};


	class grcProgram
	{
	public:

		char* m_Name = nullptr;
		atArray<u32> m_VarNameHashes;

		ID3D11Buffer** m_CbResources;
		u32 m_CbHashSum;
		u8 m_CbMinSlot;
		u8 m_CbMaxSlot;
		u32 m_CbCount;
		grcCBuffer* m_CBuffers[14];

		u32 dwordA8;
		u8 gapAC[4];
		char charB0[368];

	public:
		
		grcProgram() = default;
		virtual ~grcProgram() {};
		virtual u32 GetBytecodeSize() = 0;

	};

	struct grcVertexProgram : public grcProgram
	{
		struct DeclNode
		{
			grcVertexDeclaration* Declaration;
			ID3D11InputLayout* Layout;
			DeclNode* Next;
		};
		ComPtr<ID3D11VertexShader> m_Shader_;
		ComPtr<ID3D11VertexShader> m_Shader;

		u32	m_BytecodeSize = 0;
		void* m_Bytecode = nullptr;
		
		DeclNode* m_DeclList = nullptr;					// Cached input layouts are stored for every vertex declaration in linked list

	};

	struct grcFragmentProgram : public grcProgram
	{
		ComPtr<ID3D11PixelShader> m_Shader_;
		ComPtr<ID3D11PixelShader> m_Shader;

		u32 m_BytecodeSize = 0;
	};
	
	struct grcComputeProgram : public grcProgram
	{
		ComPtr<ID3D11ComputeShader> m_Shader_;
		ComPtr<ID3D11ComputeShader> m_Shader;
		u32 m_BytecodeSize = 0;
		u32 m_Unused234 = 0;
	};

	struct grcDomainProgram : public grcProgram
	{
		ComPtr<ID3D11DomainShader> m_Shader_;
		ComPtr<ID3D11DomainShader> m_Shader;
		u32 m_BytecodeSize = 0;
		u32 m_Unused234 = 0;
	};

	struct grcGeometryProgram : public grcProgram
	{
		struct UnknownData
		{
			u8 Unknown0[16];
			u8 Unknown16;
			u8 Unknown17;
			u8 Unknown18;
			u8 Unknown19;
		};

		ComPtr<ID3D11GeometryShader> m_Shader_;
		ComPtr<ID3D11GeometryShader> m_Shader;
		u32 m_BytecodeSize = 0;

		UnknownData* m_UnknownDatas = nullptr;
		u32 m_UnknownDataCount = 0;
	};
	
	struct grcHullProgram : public grcProgram
	{
		ComPtr<ID3D11HullShader> m_Shader_;
		ComPtr<ID3D11HullShader> m_Shader;
		u32 m_BytecodeSize = 0;
	};


	struct grcEffect
	{
		struct VariableInfo
		{
			// Metadata from loaded effect file
		};

		atArray<grcEffectTechnique>       m_Techniques;
		atArray<grcEffectVar>             m_Locals;
		atArray<grcEffectVar*>            m_LocalsCBuf;
		atArray<grcVertexProgram>         m_VertexPrograms;
		atArray<grcFragmentProgram>       m_FragmentPrograms;
		u8                                m_TechniqueMap[64 * 8];
		_system::sysMemContainerData	  m_MemContainer;
		char                              m_Name[40];
		grcInstanceData                   m_InstanceDataTemplate;
		u64                               m_FileTime;
		ConstString                       m_FilePath;
		u32                               m_NameHash;
		atArray<grcEffectVar::Annotation> m_Properties;
		u32                               m_Dcl;
		u16                               m_Ordinal;
		atArray<VariableInfo>             m_VarInfo;
		atArray<grcComputeProgram>        m_ComputePrograms;
		atArray<grcDomainProgram>         m_DomainPrograms;
		atArray<grcGeometryProgram>       m_GeometryPrograms;
		atArray<grcHullProgram>           m_HullPrograms;
	};



}