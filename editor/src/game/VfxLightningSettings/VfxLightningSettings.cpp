#include "VfxLightningSettings.h"



VfxLightningOwner::VfxLightningOwner()
{
	mVfxLightningSettings = gmAddress::Scan("40 53 48 83 EC 30 48 8B D9 83 FA 01 75 ?? 48 8B 0D")
		.GetAt(27)
		.GetRef(3)
		.To<VfxLightningSettings*>();
}

//GTA5.exe+DF4EDB - 4C 8D 0D 9E998F01 - lea r9,[  <GTA5.exe>  +26EE880  ] { (7FF629B39720) }


