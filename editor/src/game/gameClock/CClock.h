#pragma once
#include <iostream>
#include "app/memory/address.h"
#include "app/common/types.h"

#include "scripthook/inc/natives.h"
#include "scripthook/inc/types.h"
#include "scripthook/inc/enums.h"
#include "scripthook/inc/scripthookMain.h"

class CClock
{
	static s32* Hour;
	static s32* Minute;
	static s32* Sec;
	
	static bool paused;
public:
	static void PauseClock(bool val);
	static void SetTimeSample(int index);
	static bool IsTimePaused();

	static void setCurrentHour(int h);
	static void setCurrentMin(int m);
	//static void setCurrentSec(int s);

	static int GetCurrentHour();
	static int GetCurrentMin(); 
	//static int GetCurrentSec();

	static void Init();
};

