#pragma once
#include <format>
#include "common/types.h"


class Color32
{
	u32 color;
public:
	Color32() = default;
	Color32(u32 v) : color(v) {}
	Color32(float _r, float _g, float _b, float _a) { SetHexFromFloat(_r,_g,_b,_a);}
	Color32(float* p) { SetHexFromFloat(p); }

	Color32& operator= (Color32& other)
	{
		if (this == &other)
			return *this;
		color = other.color;
		return *this;
	}

	Color32& operator= (u32 v)
	{
		color = v;
		return *this;
	}

	inline float* GetFloatRGBA() 
	{
		static float col[4];

		u8 r = (color >> 16) & 0xff;
		u8 g = (color >> 8) & 0xff;
		u8 b = (color >> 0) & 0xff;
		u8 a = (color >> 24) & 0xff;

		col[0] = static_cast<float>(r) / 255.0f;
		col[1] = static_cast<float>(g) / 255.0f;
		col[2] = static_cast<float>(b) / 255.0f;
		col[3] = static_cast<float>(a) / 255.0f;

		return &col[0];
	}

	inline void SetHexFromFloat(float _r, float _g, float _b, float _a)
	{
		u8 r = static_cast<u8>(_r * 255.0f);
		u8 g = static_cast<u8>(_g * 255.0f);
		u8 b = static_cast<u8>(_b * 255.0f);
		u8 a = static_cast<u8>(_a * 255.0f);

		color = 0 | (a << 24) | (r << 16) | (g << 8) | (b << 0) ;
	}

	inline void SetHexFromFloat(float* colorf) {
		SetHexFromFloat(colorf[0], colorf[1], colorf[2], colorf[3]);
	}

	inline void SetRawU32Data(u32 v) { color = v; }
	inline u32 GetRawU32Data() { return color; }
};