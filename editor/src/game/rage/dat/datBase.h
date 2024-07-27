#pragma once

#include "rage/base/atRTTI.h"

namespace rage
{
	class datBase
	{
		DEFINE_RAGE_RTTI(datBase);
	public:
		datBase() = default;
		virtual ~datBase() = default;
	};
}
