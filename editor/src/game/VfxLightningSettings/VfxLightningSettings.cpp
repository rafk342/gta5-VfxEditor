#include "VfxLightningSettings.h"


bool VfxLightningHandler::lightning_request = false;
bool VfxLightningHandler::DirBurstSeq_request = false;
bool VfxLightningHandler::CloudLightningSeq_request = false;
u16* VfxLightningHandler::LUpdateType = nullptr;


namespace
{
	void(*Create_DirBurst_Sequence)(u64);
	void(*Create_CloudLightning_Sequence)(u64);
	void(*Create_LightningStrike)(u64);
	void(*Create_LightningStrike2)(u64);
	int* v1 = nullptr;

	enum gLightningsTypes
	{
		NONE_TYPE = 0,
		DIRECTIONAL_BURST_TYPE,
		CLOUD_BURST_TYPE,
		STRIKE_TYPE,
	};
}


void(*orig_VfxLightnings_Update)(u64);
void VfxLightningHandler::n_VfxLightnings_Update(u64 arg)
{
	orig_VfxLightnings_Update(arg);

	if (lightning_request) 
	{
		lightning_request = false;
		
		if (*LUpdateType != 0)
			*LUpdateType = 0;

		*LUpdateType = gLightningsTypes::STRIKE_TYPE;
		Create_LightningStrike(arg);
		Create_LightningStrike2(arg);
	}

	if (DirBurstSeq_request) 
	{
		DirBurstSeq_request = false;

		if (*LUpdateType != 0)
			*LUpdateType = 0;

		*LUpdateType = gLightningsTypes::DIRECTIONAL_BURST_TYPE;
		Create_DirBurst_Sequence(arg);
		*v1 = 3;
	}

	if (CloudLightningSeq_request) 
	{
		CloudLightningSeq_request = false;

		if (*LUpdateType != 0)
			*LUpdateType = 0;

		*LUpdateType = gLightningsTypes::CLOUD_BURST_TYPE;
		Create_CloudLightning_Sequence(arg);
	}
}


//GTA5.exe+DF4EDB - 4C 8D 0D 9E998F01 - lea r9,[  <GTA5.exe>  +26EE880  ] { (7FF629B39720) }
VfxLightningHandler::VfxLightningHandler()
{
	mVfxLightningSettings = gmAddress::Scan("40 53 48 83 EC 30 48 8B D9 83 FA 01 75 ?? 48 8B 0D")
		.GetAt(27)
		.GetRef(3)
		.To<gVfxLightningSettings*>();

	mVfxLightningsMngr = gmAddress::Scan("49 8D 43 ?? 48 8D 0D ?? ?? ?? ?? 49 89 43 ?? E8 ?? ?? ?? ?? 48 83 C4 48")
		.GetRef(7)
		.To<u8*>();

	LUpdateType = (u16*)(mVfxLightningsMngr + 0x2FA0);

	auto VfxLightnings_UpdateAddr = gmAddress::Scan("48 8B C4 48 89 58 ?? 48 89 68 ?? 48 89 70 ?? 57 41 56 41 57 48 83 EC 70 F3 0F 10 05");
	Hook::Create(VfxLightnings_UpdateAddr, VfxLightningHandler::n_VfxLightnings_Update, &orig_VfxLightnings_Update, "VfxLightnings::Update");

	Create_DirBurst_Sequence = VfxLightnings_UpdateAddr.GetRef(527 + 1).ToFunc<void(u64)>();
	Create_CloudLightning_Sequence = VfxLightnings_UpdateAddr.GetRef(517 + 1).ToFunc<void(u64)>();
	Create_LightningStrike = VfxLightnings_UpdateAddr.GetRef(480 + 1).ToFunc<void(u64)>();
	Create_LightningStrike2 = VfxLightnings_UpdateAddr.GetRef(507 + 1).ToFunc<void(u64)>();
	v1 = VfxLightnings_UpdateAddr.GetRef(532 + 2).To<int*>();
}

void VfxLightningHandler::LightningRequest()            { lightning_request = true;}
void VfxLightningHandler::DirBurstSeqRequest()          { DirBurstSeq_request = true;}
void VfxLightningHandler::CloudLightningSeqRequest()    { CloudLightningSeq_request = true;}
u16  VfxLightningHandler::getCurrentUpdateType()        { return *LUpdateType; }
void VfxLightningHandler::setCurrentType(u16 t)         { *LUpdateType = t; }

