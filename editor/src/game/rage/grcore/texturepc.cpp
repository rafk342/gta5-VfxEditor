#include "texturepc.h"
#include "memory/address.h"



bool rage::grcTextureDX11::LockRect(int layer, int mipLevel, grcTextureLock& lock, int lockFlags)
{
	static auto fn = gmAddress::Scan("4C 89 4C 24 20 44 89 44 24 18 89 54 24 10 48 89 4C 24 08 48 83 EC 38 C6 44 24 28", "rage::grcTextureDX11::LockRect")
		.ToFunc<bool(rage::grcTexture*, int, int, grcTextureLock&, int)>();
	return fn(this, layer, mipLevel, lock, lockFlags);
}

void rage::grcTextureDX11::UnlockRect(grcTextureLock& lock)
{
	static auto fn = gmAddress::Scan("44 88 44 24 18 48 89 54 24 10 48 89 4C 24 08 56 57 48 83 EC 78 C6", "rage::grcTextureDX11::UnlockRect_Internal")
		.ToFunc<bool(rage::grcTexture*, grcTextureLock&, bool)>();
	fn(this, lock, false /* allowContextLocks */);
}
