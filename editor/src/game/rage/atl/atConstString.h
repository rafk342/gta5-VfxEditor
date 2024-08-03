#pragma once

#include "common/types.h"
#include "rage/atl/atArray.h"	//rage_malloc
#include <cstring>

#include "rage/paging/compiler/snapshotallocator.h"

namespace rage
{
	class atConstString
	{
		char* m_String = nullptr;
	public:
		void vCopy(const char* str)
		{
			if (m_String)
			{
				rage_free(m_String);
				m_String = nullptr;
			}
			if (str == nullptr){
				return;
			} else {
				m_String = (char*)rage_malloc(strlen(str) + 1);
				memcpy(m_String, str, strlen(str) + 1);
			}
		}

		atConstString(const char* str)
		{
			vCopy(str);
		}

		~atConstString()
		{
			if (m_String)
				rage_free(m_String);
		}

		atConstString& operator=(const char* str)
		{
			vCopy(str);
			return *this;
		}

		const char* c_str() const { return m_String; }
		operator const char*() const { return m_String; }
		operator char*() { return m_String; }
	
	};
}
