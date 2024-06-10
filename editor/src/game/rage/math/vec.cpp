#include "vec.h"


rage::Vector2::Vector2()
{
	X = Y = 0.0f;
}

rage::Vector3::Vector3()
{
	X = Y = Z = 0.0f;
}

rage::Vector3::Vector3(const Vec3V& v)
{
	X = v.X();
	Y = v.Y();
	Z = v.Z();
}

rage::Vector3::Vector3(const ScalarV& s)
{
	X = Y = Z = s.Get();
}

rage::Vector4::Vector4()
{
	X = Y = Z = W = 0.0f;
}
