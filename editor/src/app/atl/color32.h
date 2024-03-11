#pragma once
#include "common/types.h"


class Color32
{
	u32 hex;
public:
	Color32(u32 _hex) : hex(_hex) {};

	Color32& operator= (Color32& other)
	{
		if (this == &other)
			return *this;
		hex = other.hex;
		return *this;
	}

	Color32& operator= (u32 num)
	{
		hex = num;
		return *this;
	}

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
		u8 red = static_cast<u8>(r * 255.0f);
		u8 green = static_cast<u8>(g * 255.0f);
		u8 blue = static_cast<u8>(b * 255.0f);
		u8 alpha = static_cast<u8>(a * 255.0f);

		hex = (red << 16) | (green << 8) | (blue << 0) | (alpha << 24);
	}

	inline void SetHexFromFloat(float* colorf) {
		SetHexFromFloat(colorf[0], colorf[1], colorf[2], colorf[3]);
	}

	inline void SetRawHexData(u32 v) { hex = v; }
	inline u32 GetRawHexData() { return hex; }
};