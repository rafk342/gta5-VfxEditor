#pragma once
#include "rage/math/vec.h"
#include "common/types.h"

namespace rage
{
	struct ptxDrawable
	{
		rage::Vector4 m_vBoundInfo;
		const char* m_name;
		void* m_pDrawable;	//TODO
		u32 m_hashName;
		u8 pad[12];
	};
}