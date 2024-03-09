#pragma once
#include "common/types.h"


class Color32
{
	u32 hex;
public:
	Color32(u32 _col) : hex(_col) {};

	inline float* GetFloatRGBA() 
	{
		static float col[4];

		u8 r = (hex >> 16) & 0xFF;
		u8 g = (hex >> 8) & 0xFF;
		u8 b = (hex >> 0) & 0xFF;
		u8 a = (hex >> 24) & 0xFF;

		col[0] = static_cast<float>(r) / 255.0f;
		col[1] = static_cast<float>(g) / 255.0f;
		col[2] = static_cast<float>(b) / 255.0f;
		col[3] = static_cast<float>(a) / 255.0f;

		return &col[0];
	}

	inline void SetHexFromFloat(float r, float g, float b, float a)
	{
		float scale = 255.f;

		u8 red = static_cast<u8>(r * scale);
		u8 green = static_cast<u8>(g * scale);
		u8 blue = static_cast<u8>(b * scale);
		u8 alpha = static_cast<u8>(a * scale);

		hex = (red << 16) | (green << 8) | (blue << 0) | (alpha << 24);
	}

	inline void SetHexFromFloat(float* colorf) {
		SetHexFromFloat(colorf[0], colorf[1], colorf[2], colorf[3]);
	}

	inline void SetRawHexData(u32 v) { hex = v; }
	inline u32 GetRawHexData() { return hex; }
};