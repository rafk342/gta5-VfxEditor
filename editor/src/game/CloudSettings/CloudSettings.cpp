#include "CloudSettings.h"
#include <iomanip>
#include "scripthookTh.h"

namespace
{
	u16(*_GetNewRandomCloudhatIndex)();
	int(*_getActiveCloudhatIdx)(void*);
}


CloudsHandler::CloudsHandler()
{
	std::array CloudNames = {
		"HEAVYclouds",
		"STORMclouds",
		"default",
		"LIGHTclouds",
		"MEDIUMclouds",
		"POSTRAINclouds",
		"HALLOWEENclouds",
		"SNOWclouds",
	};

	this->gCloudsMap = gmAddress::Scan("48 83 EC 48 48 8B 05 ?? ?? ?? ?? 4C 8B 0D")
		.GetRef(14)
		.To<gCloudSettingsMap*>();

	for (auto [hash, settings_ptr] : gCloudsMap->CloudSettings.toVec())
	{
		auto it = std::find_if(CloudNames.begin(), CloudNames.end(), [hash](const char* name) { return rage::joaat(name) == hash; });

		if (it != CloudNames.end()) {
			NamedCloudsSettingsVec.push_back({ hash,*it,settings_ptr });
		} else {
			NamedCloudsSettingsVec.push_back({ hash,std::format("Unknown /hash : 0x{:08X}",hash),settings_ptr });
		}
	}

	gCloudsMngr = *gmAddress::Scan("44 8A 49 ?? 48 8B 0D").GetRef(7).To<u8**>();
	gCloudHatNames = (atArray<CloudHatFragContainer>*)(gCloudsMngr + 0x28);

	_GetNewRandomCloudhatIndex = gmAddress::Scan("40 53 48 83 EC 20 33 DB 48 39 1D ?? ?? ?? ?? 74 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 88")
		.ToFunc<u16()>();
	
	_getActiveCloudhatIdx = gmAddress::Scan("0F B7 41 ?? 33 D2 45 33 C0 44 8B C8 85 C0 7E ?? 48 8B 41")
		.ToFunc<int(void*)>();
}


CloudSettingsNamed* CloudsHandler::FindCloudSettings(u32 hash)
{
	auto it = std::find_if(NamedCloudsSettingsVec.begin(), NamedCloudsSettingsVec.end(), [hash](CloudSettingsNamed& item)
		{ 
			return item.hash_name == hash; 
		});

	return it == NamedCloudsSettingsVec.end() ? nullptr : &(*it);
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
		scrInvoke([=] { GAMEPLAY::LOAD_CLOUD_HAT(name, time); });
	}
}


void CloudsHandler::RequestClearCloudHat()
{
	scrInvoke([]{ GAMEPLAY::_CLEAR_CLOUD_HAT(); });
}


u16 CloudsHandler::GetNewRandomCloudhatIndex()
{
	return _GetNewRandomCloudhatIndex();
}


int CloudsHandler::GetActiveCloudhatIndex()
{
	return _getActiveCloudhatIdx((void*)(gCloudsMngr));
}