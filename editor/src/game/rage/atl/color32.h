#pragma once
#include <format>
#include "common/types.h"


class Color32
{
	u32 color = 0;
	const static u8 r_shift = 16;
	const static u8 g_shift = 8;
	const static u8 b_shift = 0;
	const static u8 a_shift = 24;

public:
	Color32() = default;
	Color32(u32 v) : color(v) {}
	Color32(float _r, float _g, float _b, float _a = 1) { Setf(_r,_g,_b,_a);}
	Color32(float* p) { Setf_col4(p); }

	Color32(const Color32& other) : color(other.color)
	{ }

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

	inline u8 getRed()		{ return (u8)(color >> r_shift); }
	inline u8 getGreen()	{ return (u8)(color >> g_shift); }
	inline u8 getBlue()		{ return (u8)(color >> b_shift); }
	inline u8 getAlpha()	{ return (u8)(color >> a_shift); }

	inline float getRedf()	{ return static_cast<float>(getRed()) / 255.0f; }
	inline float getGreenf(){ return static_cast<float>(getGreen()) / 255.0f; }
	inline float getBluef() { return static_cast<float>(getBlue()) / 255.0f; }
	inline float getAlphaf(){ return static_cast<float>(getAlpha()) / 255.0f; }

	inline void setRedf		(float v) { color = (color & ~(0xffu << r_shift) | (static_cast<u8>(v * 255.0f) << r_shift)); }
	inline void setBluef	(float v) { color = (color & ~(0xffu << b_shift) | (static_cast<u8>(v * 255.0f) << b_shift)); }
	inline void setGreenf	(float v) { color = (color & ~(0xffu << g_shift) | (static_cast<u8>(v * 255.0f) << g_shift)); }
	inline void setAlphaf	(float v) { color = (color & ~(0xffu << a_shift) | (static_cast<u8>(v * 255.0f) << a_shift)); }

	inline float* Getf_col4() 
	{
		static float col[4];

		u8 r = (color >> r_shift);
		u8 g = (color >> g_shift);
		u8 b = (color >> b_shift);
		u8 a = (color >> a_shift);

		col[0] = static_cast<float>(r) / 255.0f;
		col[1] = static_cast<float>(g) / 255.0f;
		col[2] = static_cast<float>(b) / 255.0f;
		col[3] = static_cast<float>(a) / 255.0f;

		return &col[0];
	}

	inline float* Getf_col3()
	{
		static float col[3];

		u8 r = (color >> r_shift);
		u8 g = (color >> g_shift);
		u8 b = (color >> b_shift);
	
		col[0] = static_cast<float>(r) / 255.0f;
		col[1] = static_cast<float>(g) / 255.0f;
		col[2] = static_cast<float>(b) / 255.0f;

		return &col[0];
	}

	inline void Setf(float _r, float _g, float _b, float _a = 1)
	{
		u8 r = static_cast<u8>(_r * 255.0f);
		u8 g = static_cast<u8>(_g * 255.0f);
		u8 b = static_cast<u8>(_b * 255.0f);
		u8 a = static_cast<u8>(_a * 255.0f);

		color = 0 | (a << a_shift) | (r << r_shift) | (g << g_shift) | (b << b_shift);
	}

	inline void Setf_col4(float* colorf) {
		Setf(colorf[0], colorf[1], colorf[2], colorf[3]);
	}

	inline void Setf_col3(float* colorf) {
		Setf(colorf[0], colorf[1], colorf[2]);
	}

	inline void SetRawU32Data(u32 v) { color = v; }
	inline u32 GetRawU32Data() { return color; }
};
