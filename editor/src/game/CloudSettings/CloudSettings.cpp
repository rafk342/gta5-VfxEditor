#include "CloudSettings.h"
#include <iomanip>
#include "scripthookTh.h"

namespace
{
	u16(*_GetNewRandomCloudhatIndex)();
	int (*_getActiveCloudhatIdx)(u64*);
}


CloudsHandler::CloudsHandler()
{
	CloudNames = {
		{rage::joaat("HEAVYclouds")		, "HEAVYclouds"		},
		{rage::joaat("STORMclouds")		, "STORMclouds"		},
		{rage::joaat("default")			, "default"			},
		{rage::joaat("LIGHTclouds")		, "LIGHTclouds"		},
		{rage::joaat("MEDIUMclouds")	, "MEDIUMclouds"	},
		{rage::joaat("POSTRAINclouds")	, "POSTRAINclouds"	},
		{rage::joaat("HALLOWEENclouds")	, "HALLOWEENclouds"	},
		{rage::joaat("SNOWclouds")		, "SNOWclouds"		},
	};

	this->gCloudsMap = gmAddress::Scan("48 83 EC 48 48 8B 05 ?? ?? ?? ?? 4C 8B 0D")
		.GetRef(14)
		.To<gCloudSettingsMap*>();

	CloudsSettingsVec.reserve(gCloudsMap->CloudSettings.getSize());

	for (auto& [hash, name] : CloudNames)
	{
		auto* settings = gCloudsMap->CloudSettings.find(hash);
		
		if (settings) {
			CloudsSettingsVec.push_back(CloudSettingsNamed(hash, name.c_str(), settings, *(settings->bits.getRawPtr())));
		} else {
			mlogger("CloudsHandler::CloudsHandler() could not find {}", name);
		}
	}

	gCloudsMngr = *gmAddress::Scan("44 8A 49 ?? 48 8B 0D").GetRef(7).To<u8**>();
	gCloudHatNames = (atArray<CloudHatFragContainer>*)(gCloudsMngr + 0x28);

	_GetNewRandomCloudhatIndex = gmAddress::Scan("40 53 48 83 EC 20 33 DB 48 39 1D ?? ?? ?? ?? 74 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 88")
		.ToFunc<u16()>();
	
	_getActiveCloudhatIdx = gmAddress::Scan("0F B7 41 ?? 33 D2 45 33 C0 44 8B C8 85 C0 7E ?? 48 8B 41")
		.ToFunc<int(u64*)>();
}


CloudSettingsNamed* CloudsHandler::FindCloudSettings(u32 hash)
{
	for (size_t i = 0; i < CloudsSettingsVec.size(); i++) {
		if (CloudsSettingsVec[i].hash_name == hash) {
			return (CloudSettingsNamed*)&CloudsSettingsVec[i];
		}
	}
	return nullptr;
}


CloudSettingsNamed* CloudsHandler::FindCloudSettings(const char* name)
{
	return FindCloudSettings(rage::joaat(name));
}


void CloudsHandler::RequestCloudHat(const char* name, float time)
{
	if (name == "NONE") {
		RequestClearCloudHat();
	} else {
		scrInvoke([=] 
			{
				GAMEPLAY::LOAD_CLOUD_HAT(name, time);
			});
	}
}


void CloudsHandler::RequestClearCloudHat()
{
	scrInvoke([]
		{ 
			GAMEPLAY::_CLEAR_CLOUD_HAT(); 
		});
}


u16 CloudsHandler::GetNewRandomCloudhatIndex()
{
	return _GetNewRandomCloudhatIndex();
}


int CloudsHandler::GetActiveCloudhatIndex()
{
	return _getActiveCloudhatIdx((u64*)(gCloudsMngr));
}