#include "CloudSettings.h"
#include <iomanip>
#include "scripthookTh.h"

namespace
{
	u16(*GetNewRandomCloudhatIndex_fn)();
	int(*GetActiveCloudhatIdx_fn)(void*);
	
	struct CloudHatFragContainer
	{
	private:
		char pad01[64];
	public:
		//we need just names from here
		char Name[64]; 
	private:
		char pad02[336];
	};
}


CloudsHandler::CloudsHandler()
{
	this->gCloudsMap = gmAddress::Scan("48 83 EC 48 48 8B 05 ?? ?? ?? ?? 4C 8B 0D")
		.GetRef(14)
		.To<gCloudSettingsMap*>();

	fillCloudSettingsNamedVec(std::array
		{
			"HEAVYclouds",
			"STORMclouds",
			"default",
			"LIGHTclouds",
			"MEDIUMclouds",
			"POSTRAINclouds",
			"HALLOWEENclouds",
			"SNOWclouds"
		});
	
	gCloudsMngr = *gmAddress::Scan("44 8A 49 ?? 48 8B 0D").GetRef(7).To<void**>();
	atArray<CloudHatFragContainer>* CloudHatFrags = (decltype(CloudHatFrags))((u8*)gCloudsMngr + 0x28);
	
	for (auto& v : *(CloudHatFrags))
	{
		gCloudHatNames.push_back(v.Name);
	}
	gCloudHatNames.push_back("NONE");

	GetNewRandomCloudhatIndex_fn = gmAddress::Scan("40 53 48 83 EC 20 33 DB 48 39 1D ?? ?? ?? ?? 74 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 88").ToFunc<u16()>();
	GetActiveCloudhatIdx_fn = gmAddress::Scan("0F B7 41 ?? 33 D2 45 33 C0 44 8B C8 85 C0 7E ?? 48 8B 41").ToFunc<int(void*)>();
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
	return GetNewRandomCloudhatIndex_fn();
}

int CloudsHandler::GetActiveCloudhatIndex()
{
	return GetActiveCloudhatIdx_fn(gCloudsMngr);
}