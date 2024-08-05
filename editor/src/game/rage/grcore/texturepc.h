#pragma once

#include <utility>
#include "texture.h"

#include "memory/address.h"
#include "rage/atl/color32.h"
#include "rage/math/vecv.h"

#include "overlayRender/Render.h"

namespace rage
{
	struct grcTextureDX11_ExtraData
	{
		u32                      SyncType : 2;
		u32                      LockFlags : 2;
		ComPtr<ID3D11Resource>   StagingTexture;
		HANDLE                   Mutex;
	};

	enum ImageType : u8
	{
		IMAGE_TYPE_STANDARD,
		IMAGE_TUBE_CUBE,
		IMAGE_TYPE_DEPTH,
		IMAGE_TYPE_VOLUME,
	};

	struct grcTexturePC : public grcTexture
	{
		struct grcDX11InfoBits
		{
			u8 IsSRGB : 1;
			u8 ReadWriteAccess : 3;
			u8 OwnsBackingStore : 1; // TRUE: Virtual; FALSE: Physical
			u8 HasBeenDeleted : 1;
			u8 Dynamic : 1;
			u8 Dirty : 1;
		};

		u32								   m_ExtraFlags;
		u32								   m_ExtraFlagsPadding;
		u16								   m_Width;
		u16								   m_Height;
		u16								   m_Depth;
		u16								   m_Stride;
		u32								   m_Format;		// In compiled resources stored as DX9, converted to DXGI on deserializing
		ImageType						   m_ImageType;
		u8								   m_MipCount;
		u8								   m_CutMipLevels;
		grcDX11InfoBits					   m_InfoBits;
		grcTexturePC*                      m_Next;
		grcTexturePC*                      m_Previous;
		void*							   m_BackingStore;			// Texture pixels, each mip map is stored next to each other without padding
		ComPtr<ID3D11ShaderResourceView>   m_ShaderResourceView;		// Same as for m_CachedTexture
		grcTextureDX11_ExtraData*          m_ExtraData;
	



		virtual u16 GetWidth() const override { return m_Width; }
		virtual u16 GetHeight() const override { return m_Height; }
		virtual u16 GetDepth() const override { return m_Depth; }
		virtual u8 GetMipMapCount() const override { return m_MipCount; }
		virtual u8 GetArraySize() const override { return 1; }

		virtual bool IsValid() const override { return true; };
		virtual bool IsSRGB() const override { return m_InfoBits.IsSRGB; }
		virtual void* GetTextureView() const override { return m_ShaderResourceView.Get(); }
		
		virtual u32 GetImageFormat() const override { return m_Format; }

		virtual bool LockRect(int layer, int mipLevel, grcTextureLock& lock, int lockFlags) override
		{ 
			LogInfo("grcTexturePC::LockRect() -> Not implemented"); std::unreachable();
		}

		virtual void UnlockRect(grcTextureLock& lock) override 
		{ 
			LogInfo("grcTexturePC::UnlockRect() -> Not implemented"); std::unreachable();
		};
		virtual atFixedBitSet<8, u8> FindUsedChannels() const override 
		{ 
			LogInfo("grcTexturePC::FindUsedChannels() -> Not implemented"); std::unreachable();
		}

		virtual void DeviceLost() {}
		virtual void DeviceReset() {}

		virtual void CreateFromBackingStore(bool recreate) = 0;
		
		virtual void SetPrivateData() = 0;	// Sets texture's resource name in DirectX debug layer
		virtual bool CopyTo(void* image, bool invert) = 0;
	};


	class grcTextureDX11 : public grcTexturePC
	{
		virtual bool LockRect(int layer, int mipLevel, grcTextureLock& lock, int lockFlags) override;
		virtual void UnlockRect(grcTextureLock& lock) override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FORMAT_A8R8G8B8 40

	struct grcImage
	{
		enum Format {
			UNKNOWN                    , // Undefined
			DXT1                       , // DXT1 (aka 'BC1')
			DXT3                       , // DXT3 (aka 'BC2')
			DXT5                       , // DXT5 (aka 'BC3', 'DXT5NM', 'RXGB')
			CTX1                       , // CTX1 (like DXT1 but anchor colors are 8.8 instead of 5.6.5)
			DXT3A                      , // alpha block of DXT3 (XENON-specific)
			DXT3A_1111                 , // alpha block of DXT3, split into four 1-bit channels (XENON-specific)
			DXT5A                      , // alpha block of DXT5 (aka 'BC4', 'ATI1')
			DXN                        , // DXN (aka 'BC5', 'ATI2', '3Dc', 'RGTC', 'LATC', etc.) // [CLEMENSP]
			BC6                        , // BC6 (specifically BC6H_UF16)
			BC7                        , // BC7
			A8R8G8B8                   , // 32-bit color with alpha, matches Color32 class
			A8B8G8R8                   , // 32-bit color with alpha, provided for completeness
			A8                         , // 8-bit alpha-only (color is black)
			L8                         , // 8-bit luminance (R=G=B=L, alpha is opaque)
			A8L8                       , // 16-bit alpha + luminance
			A4R4G4B4                   , // 16-bit color and alpha
			A1R5G5B5                   , // 16-bit color with 1-bit alpha
			R5G6B5                     , // 16-bit color
			R3G3B2                     , // 8-bit color (not supported on consoles)
			A8R3G3B2                   , // 16-bit color with 8-bit alpha (not supported on consoles)
			A4L4                       , // 8-bit alpha + luminance (not supported on consoles)
			A2R10G10B10                , // 32-bit color with 2-bit alpha
			A2B10G10R10                , // 32-bit color with 2-bit alpha
			A16B16G16R16               , // 64-bit four channel fixed point (s10e5 per channel -- sign, 5 bit exponent, 10 bit mantissa)
			G16R16                     , // 32-bit two channel fixed point
			L16                        , // 16-bit luminance
			A16B16G16R16F              , // 64-bit four channel floating point (s10e5 per channel)
			G16R16F                    , // 32-bit two channel floating point (s10e5 per channel)
			R16F                       , // 16-bit single channel floating point (s10e5 per channel)
			A32B32G32R32F              , // 128-bit four channel floating point (s23e8 per channel)
			G32R32F                    , // 64-bit two channel floating point (s23e8 per channel)
			R32F                       , // 32-bit single channel floating point (s23e8 per channel)
			D15S1                      , // 16-bit depth + stencil (depth is 15-bit fixed point, stencil is 1-bit) (not supported on consoles)
			D24S8                      , // 32-bit depth + stencil (depth is 24-bit fixed point, stencil is 8-bit)
			D24FS8                     , // 32-bit depth + stencil (depth is 24-bit s15e8, stencil is 8-bit)
			P4                         , // 4-bit palettized (not supported on consoles)
			P8                         , // 8-bit palettized (not supported on consoles)
			A8P8                       , // 16-bit palettized with 8-bit alpha (not supported on consoles)
			R8                         , // non-standard R001 format (8 bits per channel)
			R16                        , // non-standard R001 format (16 bits per channel)
			G8R8                       , // non-standard RG01 format (8 bits per channel)
			LINA32B32G32R32F_DEPRECATED,
			LINA8R8G8B8_DEPRECATED     ,
			LIN8_DEPRECATED            ,
			RGBE                       ,
			LAST_FORMAT = RGBE
		};

		enum ImageType {
			STANDARD, // 2D image (still supports mipmaps, and texture array layers)
			CUBE,     // Cube map image (+x,-x,+y,-y,+z,-z, stored as a 6-layer 2D image)
			DEPTH,    // Depth map image
			VOLUME,   // Volume texture
			FORCETYPE32 = 0x7FFFFFFF
		};

		u16 m_Width;
		u16 m_Height;
		Format m_Format;
		ImageType m_Type;
		u16 m_Stride;
		u8 m_Depth;
		u8 m_StrideHi;
		u8* m_Bits;
		Color32* m_Lut;
		grcImage* m_Next;
		grcImage* m_NextLayer;
		int m_RefCount;
		rage::Vector4 m_ColorExp;
		rage::Vector4 m_ColorOfs;
	};
	
	struct grcManualTextureDef
	{
		int isStaging;
		char pad[24];
		int arraySize;
	};

	struct TextureCreateParams
	{
		enum Memory_t { SYSTEM, VIDEO, STAGING }; // System indicates Dynamic CPU Write Access, Staging indicates CPU Read Access
		enum Format_t { LINEAR, TILED };
		enum Type_t { NORMAL, RENDERTARGET, REFERENCE, DICTIONARY_REFERENCE };
		enum MSAA_t { MSAA_NONE, MSAA_2X, MSAA_4X, MSAA_8X };
		enum ThreadUseHint_t { THREAD_USE_HINT_NONE, THREAD_USE_HINT_CAN_BE_UPDATE_THREAD };

		Memory_t Memory;
		Format_t Format;
		u32 LockFlags;
		void* Buffer;
		Type_t Type;
		MSAA_t MSAA_Type;
		u32 StereoRTMode;
		u32 MipLevels;
		ThreadUseHint_t ThreadUseHint;
	};

	class grcTextureFactory
	{
		grcTextureFactory() {};
		~grcTextureFactory() {};
	public:

		static grcTextureFactory* GetInstance()
		{
			static grcTextureFactory* obj = nullptr;
			if (!obj)
				obj = (grcTextureFactory*)(*gmAddress::Scan("84 DB 48 0F 45 C2 48 89 05").GetRef(9).To<void**>());
			return obj;
		}

		//Can return nullptr
		grcTexture* CreateFromDDS(std::filesystem::path path, grcImage::Format img_format);
		//Can return nullptr
		grcTexture* CreateFromPath(std::filesystem::path path, grcImage::Format img_format = grcImage::Format::A8B8G8R8);

		virtual ~grcTextureFactory() = 0;
		virtual grcTexture* Create(u32 width, u32 height, u32 depth, u32 eFormat, void* pBuffer, TextureCreateParams* params) = 0;
		virtual grcTexture* Create(grcImage* texture, TextureCreateParams* CreateParams) = 0;
		virtual grcTexture* Create(u32 width, u32 height, u32 eFormat, void* pBuffer, u32 numMips, TextureCreateParams* params) = 0;
		virtual grcTexture* Create(const char* pFilename, void* params) = 0;

	private:
		
		virtual void v1() = 0;
		virtual void v2() = 0;
		virtual void v3() = 0;
		virtual void v4() = 0;
		virtual void v5() = 0;
		virtual void v6() = 0;
		virtual void v7() = 0;
		virtual void v8() = 0;
		virtual void v9() = 0;
		virtual void v10() = 0;
		virtual void v11() = 0;
		virtual void v12() = 0;
		virtual void v13() = 0;
		virtual void v14() = 0;
		virtual void v15() = 0;
		virtual void v16() = 0;
		virtual void v17() = 0;
		virtual void v18() = 0;
		virtual void v19() = 0;
		virtual void v20() = 0;
		virtual void v21() = 0;
		virtual void v22() = 0;
		virtual void v23() = 0;
		virtual void v24() = 0;
		virtual void v25() = 0;
		virtual void v26() = 0;
		virtual void v27() = 0;
		virtual void v28() = 0;
		virtual void v29() = 0;
		virtual void v30() = 0;
		virtual void v31() = 0;
		virtual void v32() = 0;

	public:
		virtual int TranslateFormatToParamFormat(int format) = 0;
	};
}

