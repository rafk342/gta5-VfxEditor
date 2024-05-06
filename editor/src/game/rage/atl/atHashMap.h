#pragma once
#include "rage/base/tlsContext.h"

template<typename T>
struct atMapHashFn
{
	u32 operator()(const T&) = delete;
};

template<> inline u32 atMapHashFn<u8>::operator()(const u8& value) { return value; }
template<> inline u32 atMapHashFn<s8>::operator()(const s8& value) { return static_cast<u32>(value); }
template<> inline u32 atMapHashFn<u16>::operator()(const u16& value) { return value; }
template<> inline u32 atMapHashFn<s16>::operator()(const s16& value) { return static_cast<u32>(value); }
template<> inline u32 atMapHashFn<u32>::operator()(const u32& value) { return value; }
template<> inline u32 atMapHashFn<s32>::operator()(const s32& value) { return static_cast<u32>(value); }
template<> inline u32 atMapHashFn<u64>::operator()(const u64& value) { return static_cast<u32>(value % UINT_MAX); }
template<> inline u32 atMapHashFn<s64>::operator()(const s64& value) { return static_cast<u32>(value % UINT_MAX); }
template<> inline u32 atMapHashFn<const char*>::operator()(const ConstString& str) { return rage::joaat(str); }
template<> inline u32 atMapHashFn<std::string>::operator()(const std::string& str) { return rage::joaat(str.c_str()); }

template<typename TKey, typename TData, typename THashFn = atMapHashFn<TKey>>
class atMap
{
private:
	struct Entry
	{
		u32 hash;
		TData data;
		Entry* next = nullptr;
	};

	Entry** m_Buckets = nullptr;
	u16 m_capacity = 0;
	u16 m_size = 0;
	char pad[3]{};
	bool m_AllowGrowing = false;

public:

	u16 getSize() { return m_size; }
	u16 getCapacity() { return m_capacity; }

	inline TData* find(const u32& idx)
	{
		for (Entry* i = *(m_Buckets + (idx % m_capacity)); i; i = i->next) {
			if (i->hash == idx) {
				return &i->data;
			}
		}
		return nullptr;
	}
	
	inline u32 GetHash(const TKey& value) const
	{
		THashFn fn{};
		return fn(value);
	}

	inline TData* at(const TKey& v)
	{
		return find(GetHash(v));
	}

	// pair ( hash, data ptr )
	std::vector<std::pair<u32, TData*>> toVec()
	{
		std::vector<std::pair<u32, TData*>> vec;
		std::for_each(m_Buckets, m_Buckets + m_capacity, [&vec](Entry* bucket) 
			{
				for (Entry* e = bucket; e; e = e->next) 
				{
					vec.push_back({ e->hash, &e->data });
				}
			});
		return vec;
	}
};

