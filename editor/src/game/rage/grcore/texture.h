#pragma once

#include <d3d11.h>
#include "wrl.h"

#include "rage/atl/atBitSet.h"
#include "common/types.h"
#include "rage/paging/base.h"
#include "rage/grcore/effect.h"
#include "rage/atl/atConstString.h"

using Microsoft::WRL::ComPtr;
namespace rage
{
	struct grcPoint { int x, y; };
	struct grcRect { int x1, y1, x2, y2; };

	struct grcTextureLock
	{
		int   MipLevel;
		pVoid Base;
		int   Pitch;
		int   BitsPerPixel;
		int   Width;
		int   Height;
		int   RawFormat;
		int   Layer;
	};

	enum grcTextureCreateType
	{
		grcsTextureCreate_NeitherReadNorWrite,
		grcsTextureCreate_ReadWriteHasStaging,
		grcsTextureCreate_Write,
		grcsTextureCreate_ReadWriteDynamic,
		grcsTextureCreate_WriteDynamic,
		grcsTextureCreate_WriteOnlyFromRTDynamic,
	};

	enum grcsTextureSyncType
	{
		grcsTextureSync_Mutex,
		grcsTextureSync_DirtySemaphore,
		grcsTextureSync_CopyData,
		grcsTextureSync_None,
	};

	enum eTextureSwizzle
	{
		TEXTURE_SWIZZLE_R,
		TEXTURE_SWIZZLE_G,
		TEXTURE_SWIZZLE_B,
		TEXTURE_SWIZZLE_A,
		TEXTURE_SWIZZLE_0,
		TEXTURE_SWIZZLE_1,
	};

	enum eTextureConversionFlags
	{
		TEXTURE_CONVERSION_FLAG_PROCESSED = 0x80,
		TEXTURE_CONVERSION_FLAG_SKIPPED = 0x40,
		TEXTURE_CONVERSION_FLAG_FAILED_PROCESSING = 0x20,
		TEXTURE_CONVERSION_FLAG_INVALID_METADATA = 0x10,
		TEXTURE_CONVERSION_FLAG_OPTIMISED_DXT = 0x8,
		TEXTURE_CONVERSION_FLAG_NO_PROCESSING = 0x60,
		TEXTURE_CONVERSION_FLAG_MASK = 0xF8,
	};

	enum eTextureType
	{
		TEXTURE_NORMAL,
		TEXTURE_RENDER_TARGET,
		TEXTURE_REFERENCE,
	};

	enum eTextureTemplateType
	{
		TEXTURE_TEMPLATE_TYPE_UNKNOWN,
		TEXTURE_TEMPLATE_TYPE_DEFAULT,
		TEXTURE_TEMPLATE_TYPE_TERRAIN,
		TEXTURE_TEMPLATE_TYPE_CLOUDDENSITY,
		TEXTURE_TEMPLATE_TYPE_CLOUDNORMAL,
		TEXTURE_TEMPLATE_TYPE_CABL5,
		TEXTURE_TEMPLATE_TYPE_FENC6,
		TEXTURE_TEMPLATE_TYPE_ENVEFF,
		TEXTURE_TEMPLATE_TYPE_SCRIPT,
		TEXTURE_TEMPLATE_TYPE_WATERFLOW,
		TEXTURE_TEMPLATE_TYPE_WATERFOAM,
		TEXTURE_TEMPLATE_TYPE_WATERFOG,
		TEXTURE_TEMPLATE_TYPE_WATEROCEAN,
		TEXTURE_TEMPLATE_TYPE_WATER,
		TEXTURE_TEMPLATE_TYPE_FOAMOPACITY,
		TEXTURE_TEMPLATE_TYPE_FOAM,
		TEXTURE_TEMPLATE_TYPE_DIFFUSEMIPSHARPEN,
		TEXTURE_TEMPLATE_TYPE_DIFFUSEDETAIL,
		TEXTURE_TEMPLATE_TYPE_DIFFUSEDARK,
		TEXTURE_TEMPLATE_TYPE_DIFFUSEALPHAOPAQUE,
		TEXTURE_TEMPLATE_TYPE_DIFFUSE,
		TEXTURE_TEMPLATE_TYPE_DETAIL,
		TEXTURE_TEMPLATE_TYPE_NORMAL,
		TEXTURE_TEMPLATE_TYPE_SPECULAR,
		TEXTURE_TEMPLATE_TYPE_EMISSIVE,
		TEXTURE_TEMPLATE_TYPE_TINTPALETTE,
		TEXTURE_TEMPLATE_TYPE_SKIPPROCESSING,
		TEXTURE_TEMPLATE_TYPE_DONOTOPTIMIZE,
		TEXTURE_TEMPLATE_TYPE_TEST,
		TEXTURE_TEMPLATE_TYPE_COUNT,
		TEXTURE_TEMPLATE_TYPE_MASK = 0x1F,
		TEXTURE_TEMPLATE_TYPE_FLAG_NOT_HALF = 0x20,
		TEXTURE_TEMPLATE_TYPE_FLAG_HD_SPLIT = 0x40,
		TEXTURE_TEMPLATE_TYPE_FLAG_FULL = 0x80,
		TEXTURE_TEMPLATE_TYPE_FLAG_MAPS_HALF = 0x100,
		TEXTURE_TEMPLATE_TYPE_FLAG_MASK = 0x1E0,
		TEXTURE_TEMPLATE_TYPE_VERSION_MASK = 0xE00,
		TEXTURE_TEMPLATE_TYPE_VERSION_SHIFT = 0x9,
		TEXTURE_TEMPLATE_TYPE_VERSION_CURRENT = 0x200,
	};

	struct grcTexture : public pgBase
	{
		struct 
		{
			u8  Format;
			u8  Mipmap;
			u8  Dimension;
			u8  Cubemap;
			u32 Remap;
			u16 Width;
			u16 Height;
			u16 Depth;
			u8  Location;
			u8	Padding;
			u32 Pitch;
			u32 Offset;
		} m_Texture;	// Unused (leftover cellGcmTexture PS3 type)

		atConstString			 m_Name;
		u16						 m_RefCount;
		u8						 m_ResourceTypeAndConversionFlags;
		u8						 m_LayerCount;
		u8 						 pad[4];
		ComPtr<ID3D11Resource>   m_CachedTexture;
		u32						 m_PhysicalSizeAndTemplateType;
		u32						 m_HandleIndex;

		virtual void Download() const { }
		virtual int  GetDownloadSize() const { return 0; }

		virtual u16 GetWidth() const = 0;
		virtual u16 GetHeight() const = 0;
		virtual u16 GetDepth() const = 0;
		virtual u8  GetMipMapCount() const = 0;
		virtual u8  GetArraySize() const { return 1; }
		virtual u8  GetBitsPerPixel() const = 0;

		virtual bool IsValid() const { return true; }

		virtual u32 GetMSAAMode(int* msaaMode) const { return 0; }

		virtual bool IsGammaEnabled() const { return false; }
		virtual void SetGammaEnabled(bool on) {}

		virtual int  GetTextureSignedMask() const { return 0; }
		virtual void SetTextureSignedMask(int mask) {}

		virtual const grcTexture* GetReference() const { return this; }
		virtual grcTexture* GetReference() { return this; }

		virtual bool IsSRGB() const = 0;

		virtual const void* GetTexturePtr() const { return m_CachedTexture.Get(); }
		virtual void* GetTexturePtr() { return m_CachedTexture.Get(); }
		virtual void* GetTextureView() const = 0;

		virtual void UpdateGPUCopy() {}

		virtual u32 GetImageFormat() const = 0;

		virtual bool LockRect(int layer, int mipLevel, grcTextureLock& lock, int lockFlags) = 0;
		virtual void UnlockRect(grcTextureLock& lock) = 0;

		virtual void SwizzleTexture2D(grcTextureLock& lock) {}
		virtual void UnswizzleTexture2D(grcTextureLock& lock) {}
		virtual void Resize(int newWidth, int newHeight) {}
		virtual void SetEffectInfo(grcEffect* effect, grcEffectVar* effectVar) {}
		virtual void AddWidgets(pVoid bank) {}
		virtual bool Save() { return true; }

		virtual bool Copy(const pVoid src, u32 width, u32 height) { return true; }
		virtual bool Copy(const grcTexture* src, int dstSliceIndex, int dstMipIndex, int srcSliceIndex, int srcMipIndex) { return true; }
		virtual bool Copy(const pVoid/*grcImage*/* src) { return true; }
		virtual bool Copy2D(const pVoid src, const grcPoint& srcDim, const grcRect& dstRect, grcTextureLock& lock, int mipLevel) { return true; }
		virtual bool Copy2D(const pVoid src, int format, u32 width, u32 height, u32 mipCount) { return true; }

		virtual void Tile(int n) {}

		virtual atFixedBitSet<8, u8> FindUsedChannels() const = 0;

		virtual void SetTextureSwizzle(eTextureSwizzle r, eTextureSwizzle g, eTextureSwizzle b, eTextureSwizzle a, bool) {}
		virtual void GetTextureSizzle(eTextureSwizzle& r, eTextureSwizzle& g, eTextureSwizzle& b, eTextureSwizzle& a)
		{
			r = TEXTURE_SWIZZLE_R;
			g = TEXTURE_SWIZZLE_G;
			b = TEXTURE_SWIZZLE_B;
			a = TEXTURE_SWIZZLE_A;
		}


	};


}