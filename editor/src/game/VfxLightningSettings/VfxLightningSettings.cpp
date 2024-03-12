#include "VfxLightningSettings.h"


bool VfxLightningHandler::lightning_request = false;
bool VfxLightningHandler::DirBurstS_request = false;
bool VfxLightningHandler::CloudLightningS_request = false;
bool VfxLightningHandler::override_flag = false;
u16  VfxLightningHandler::requested_override_type = 0;
u16* VfxLightningHandler::LUpdateType = nullptr;
int  VfxLightningHandler::savedOccurranceChance = 0;

VfxLightningHandler* VfxLightningHandler::selfInstance = nullptr;

namespace
{
	void(*Create_DirBurst_Sequence)(u64);
	void(*Create_CloudLightning_Sequence)(u64);
	void(*Create_LightningStrike)(u64);
	void(*Create_LightningStrike2)(u64);
	int* v1 = nullptr;
}

bool VfxLightningHandler::getOverrideState() { return override_flag; }
void VfxLightningHandler::setOverrideState(bool state) 
{
	if (state == true)
	{
		savedOccurranceChance = this->mVfxLightningSettings->lightningOccurranceChance;
		override_flag = true;
	}
	if (state == false)
	{
		override_flag = false;
		this->mVfxLightningSettings->lightningOccurranceChance = savedOccurranceChance;
	}
}

void VfxLightningHandler::setOverrideType(u16 type)
{
	if (!override_flag)
		return;
	requested_override_type = type;
}

void VfxLightningHandler::GenStrike(u64& arg)
{
	*LUpdateType = gLightningsTypes::STRIKE_TYPE;
	Create_LightningStrike(arg);
	Create_LightningStrike2(arg);
}

void VfxLightningHandler::GenDirBurst(u64& arg)
{
	*LUpdateType = gLightningsTypes::DIRECTIONAL_BURST_TYPE;
	Create_DirBurst_Sequence(arg);
	*v1 = 3;
}

void VfxLightningHandler::GenCloudBurst(u64& arg)
{
	*LUpdateType = gLightningsTypes::CLOUD_BURST_TYPE;
	Create_CloudLightning_Sequence(arg);
}


void(*orig_VfxLightnings_Update)(u64);
void VfxLightningHandler::n_VfxLightnings_Update(u64 arg)
{
	orig_VfxLightnings_Update(arg);

	if (override_flag)
	{	
		if (!selfInstance)
			return;

		selfInstance->mVfxLightningSettings->lightningOccurranceChance = 0;
		
		if (*LUpdateType != 0)
			return;
	
		switch (requested_override_type)
		{
		case gLightningsTypes::NONE_TYPE:
			break;
		case gLightningsTypes::STRIKE_TYPE:
			GenStrike(arg);
			break;
		case gLightningsTypes::DIRECTIONAL_BURST_TYPE:
			GenDirBurst(arg);
			break;
		case gLightningsTypes::CLOUD_BURST_TYPE:
			GenCloudBurst(arg);
			break;
		default:
			break;
		}

		return;
	}

	if (lightning_request) 
	{
		lightning_request = false;
		
		if (*LUpdateType != 0)
			*LUpdateType = 0;

		GenStrike(arg);
	}

	if (DirBurstS_request) 
	{
		DirBurstS_request = false;

		if (*LUpdateType != 0)
			*LUpdateType = 0;

		GenDirBurst(arg);
	}

	if (CloudLightningS_request) 
	{
		CloudLightningS_request = false;

		if (*LUpdateType != 0)
			*LUpdateType = 0;

		GenCloudBurst(arg);
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

	selfInstance = this;
}

void VfxLightningHandler::LightningRequest()            { lightning_request = true;}
void VfxLightningHandler::DirBurstSeqRequest()          { DirBurstS_request = true;}
void VfxLightningHandler::CloudLightningSeqRequest()    { CloudLightningS_request = true;}
u16  VfxLightningHandler::getCurrentUpdateType()        { return *LUpdateType; }
void VfxLightningHandler::setCurrentUpdateType(u16 t)   { *LUpdateType = t; }

