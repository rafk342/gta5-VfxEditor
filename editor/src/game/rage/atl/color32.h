#pragma once
#include <format>
#include "common/types.h"

#include "rage/math/vec.h"

class Color32
{
	u32 color = 0;
	const static u8 r_shift = 16;
	const static u8 g_shift = 8;
	const static u8 b_shift = 0;
	const static u8 a_shift = 24;
public:

	Color32() = default;
	
	Color32(u32 value) : color(value) { }
	Color32(float _r, float _g, float _b, float _a = 1.0f) { Setf(_r,_g,_b,_a); }
	Color32(int _r, int _g, int _b, int _a = 255) { SetU8(u8(_r), u8(_g), u8(_b), u8(_a)); }
	Color32(float* p)	{ Setf (p[0], p[1], p[2], p[3]); }
	Color32(u8* p)		{ SetU8(p[0], p[1], p[2], p[3]); }

	Color32(const Color32& other) : color(other.color) { }

	Color32& operator= (Color32& other)
	{
		color = other.color;
		return *this;
	}

	Color32& operator= (std::initializer_list<float> il)
	{
		auto it = il.begin();
		if (il.size() >= 1) setRedf(*it++);
		if (il.size() >= 2) setGreenf(*it++);
		if (il.size() >= 3) setBluef(*it++);
		if (il.size() >= 4) setAlphaf(*it);
		return *this;
	}
	
	Color32& operator= (std::initializer_list<int> il)
	{
		auto it = il.begin();
		if (il.size() >= 1) setRedU8(*it++);
		if (il.size() >= 2) setGreenU8(*it++);
		if (il.size() >= 3) setBlueU8(*it++);
		if (il.size() >= 4) setAlphaU8(*it);
		return *this;
	}
	
	Color32& operator= (float* p)
	{
		Setf(p[0], p[1], p[2], p[3]);
		return *this;
	}
	
	Color32& operator= (u8* p)
	{
		SetU8(p[0], p[1], p[2], p[3]);
		return *this;
	}

	Color32& operator= (u32 value)
	{
		color = value;
		return *this;
	}

	operator u32()
	{
		return color;
	}

	Color32 operator*(const Color32& other) const
	{
		return Color32(
			(getRedU8() * other.getRedU8()) / 255,
			(getGreenU8() * other.getGreenU8()) / 255,
			(getBlueU8() * other.getBlueU8()) / 255,
			(getAlphaU8() * other.getAlphaU8()) / 255);
	}

	inline u8 getRedU8() const		{ return static_cast<u8>(color >> r_shift); }
	inline u8 getGreenU8() const	{ return static_cast<u8>(color >> g_shift); }
	inline u8 getBlueU8() const		{ return static_cast<u8>(color >> b_shift); }
	inline u8 getAlphaU8() const	{ return static_cast<u8>(color >> a_shift); }

	inline float getRedf() const	{ return static_cast<float>(getRedU8()) / 255.0f; }
	inline float getGreenf() const	{ return static_cast<float>(getGreenU8()) / 255.0f; }
	inline float getBluef() const	{ return static_cast<float>(getBlueU8()) / 255.0f; }
	inline float getAlphaf() const	{ return static_cast<float>(getAlphaU8()) / 255.0f; }

	inline void setRedf		(float v) { setRedU8(static_cast<u8>(v * 255.0f) ); }
	inline void setBluef	(float v) { setBlueU8(static_cast<u8>(v * 255.0f) ); }
	inline void setGreenf	(float v) { setGreenU8(static_cast<u8>(v * 255.0f) ); }
	inline void setAlphaf	(float v) { setAlphaU8(static_cast<u8>(v * 255.0f) ); }

	inline void setRedU8	(u8 v) { color = (color & ~(0xffu << r_shift) | (v << r_shift)); }
	inline void setBlueU8	(u8 v) { color = (color & ~(0xffu << b_shift) | (v << b_shift)); }
	inline void setGreenU8	(u8 v) { color = (color & ~(0xffu << g_shift) | (v << g_shift)); }
	inline void setAlphaU8	(u8 v) { color = (color & ~(0xffu << a_shift) | (v << a_shift)); }

	inline rage::Vector4 Getf_col4() const { return { getRedf(), getGreenf(), getBluef(), getAlphaf() }; }
	inline rage::Vector3 Getf_col3() const { return { getRedf(), getGreenf(), getBluef() };}

	inline void SetU8(u8 _r, u8 _g, u8 _b, u8 _a = 255)
	{
		color = 0 | (_a << a_shift) | (_r << r_shift) | (_g << g_shift) | (_b << b_shift);
	}

	inline void Setf(float _r, float _g, float _b, float _a = 1)
	{
		SetU8((static_cast<u8>(_r * 255.0f)),
			  (static_cast<u8>(_g * 255.0f)),
			  (static_cast<u8>(_b * 255.0f)),
			  (static_cast<u8>(_a * 255.0f)));
	}

	inline void Setf_col4(float* colorf) {
		Setf(colorf[0], colorf[1], colorf[2], colorf[3]);
	}

	inline void Setf_col3(float* colorf) {
		Setf(colorf[0], colorf[1], colorf[2]);
	}

	inline void SetRawU32Data(u32 v) { color = v; }
	inline u32  GetRawU32Data() const { return color; }
};
