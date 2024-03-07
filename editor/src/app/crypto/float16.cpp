#include "float16.h"

u32 GetNonZeroMask(u32 x)
{
	return (u32)(((s32)(x) | -(s32)(x)) >> 31);
}

u16 GetFloat16_FromFloat32(f32 f)
{
	u32 i, e, l, o;

	i = *(u32*)&f;
	e = 0x7fffffffu & i;
	e = 0x38000000u - e;
	l = (s32)(0x00000000u + e) >> 31;
	o = (s32)(0x0fffffffu + e) >> 31;
	e = (u32)(-(s32)e) >> 13;
	e = e | o;
	e = e & ((0x00007fffu & l));
	e = e | ((0x80000000u & i) >> 16);

	return (u16)e;
}

f32 GetFloat32_FromFloat16(u16 m_data)
{
	u32 e, z, s;

	e = (u32)m_data << 16;
	s = 0x80000000u & e;
	e = 0x7fff0000u & e;
	z = GetNonZeroMask(e);
	z = 0x38000000u & z;
	e = ((e >> 3) + z) | s;

	return *(f32*)&e;
}
