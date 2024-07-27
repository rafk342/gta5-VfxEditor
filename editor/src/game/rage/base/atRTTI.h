#pragma once
#include <iostream>
#include "common/types.h"
#include "crypto/joaat.h"

#define DEFINE_RAGE_RTTI(type)\
private:\
	virtual void*	TypeInfo_Fn1() { return nullptr; }\
	virtual void*	TypeInfo_Fn2() { return nullptr; }\
	virtual u32		TypeInfo_GetBaseNameHash() { return rage::joaat(#type); }\
	virtual type*	TypeInfo_Fn4(void*) { return nullptr; }\
	virtual bool	TypeInfo_Fn5(void*) { return false; }\
	virtual bool	TypeInfo_Fn6(void**) { return false; }\
public:

