#pragma once
#include "common/types.h"
#include "memory/address.h"
#include "memory/hook.h"
#include "compiler/compiler.h"

class CamCinematicContext
{
public:
	static inline bool DISABLE_IDLE_CAM = false;

	static void Init()
	{									
		Hook::Create(
			gmAddress::Scan("40 53 48 83 EC ?? 48 8B D9 E8 ?? ?? ?? ?? 48 8B 15 ?? ?? ?? ?? 48 8B 8A"), 
			hk_CamCinematicOnFootIdleContext_CanUpdate, 
			&gImpl_CamCinematicOnFootIdleContext_CanUpdate, 
			"_CamCinematicOnFootIdleContext_CanUpdate");
	}

private:
	static inline bool(*gImpl_CamCinematicOnFootIdleContext_CanUpdate)(void*) = nullptr;
	static bool hk_CamCinematicOnFootIdleContext_CanUpdate(void* that)
	{
		if (!DISABLE_IDLE_CAM)
		{
			return gImpl_CamCinematicOnFootIdleContext_CanUpdate(that);
		}
		return false;
	}
};
