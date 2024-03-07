#include "CClock.h"

bool CClock::paused = false;
s32* CClock::Hour = nullptr;
s32* CClock::Minute = nullptr;
s32* CClock::Sec = nullptr;

bool CClock::IsTimePaused() { return paused; }
void CClock::PauseClock(bool val)
{
	paused = val;
	TIME::PAUSE_CLOCK(val);
	scriptWait(200);
}

void CClock::SetTimeSample(int index)
{
	static auto fn = gmAddress::Scan("48 83 EC 28 85 C9 78 ?? 3B 0D").ToFunc<void(int)>();
	fn(index);
}

void CClock::setCurrentHour(int h){ *Hour = h;}
void CClock::setCurrentMin(int m) { *Minute = m; }
//void CClock::setCurrentSec(int s) { *Sec = s; }

int CClock::GetCurrentHour() { return *Hour; }
int CClock::GetCurrentMin()  { return *Minute; }
//int CClock::GetCurrentSec()  { return *Sec; }

void CClock::Init()
{
	static auto addr = gmAddress::Scan("66 41 0F 6E C8 0F 57 C0");
	
	Hour = addr.GetRef(8 + 2).To<s32*>();
	Minute = addr.GetRef(14 + 2).To<s32*>();
	//Sec = addr.GetRef(20 + 2).To<s32*>();
}
