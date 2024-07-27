#pragma once

#include "common/types.h"
#include "rage/atl/atArray.h"
#include "rage/paging/base.h"
#include "rage/paging/ref.h"

namespace rage
{
	template<class T>
	class pgDictionary : public rage::pgBase
	{
	public:

		pgDictionary<T>* m_Parent;
		u32 m_RefCount;
		u32 pad;
		atArray<u32> m_Codes;
		atArray<datOwner<T>> m_Entries;
	};
}