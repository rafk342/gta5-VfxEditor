#pragma once
#include "atArray.h"

template<typename TEntry>
class atHashMap
{
private:
	struct Entry
	{
		uint32_t hash;
		TEntry data;
		Entry* next;
	};

	atArray<Entry*> m_data;
	//char pad[3];
	//bool m_initialized;

public:

	u16 getSize() { return m_data.GetSize(); }
	u16 getCapacity() { return m_data.getCapacity(); }

	inline TEntry* find(const uint32_t& idx)
	{
		for (Entry* i = *(m_data.begin() + (idx % m_data.GetSize())); i; i = i->next) {
			if (i->hash == idx) {
				return &i->data;
			}
		}
		return nullptr;
	}
};
