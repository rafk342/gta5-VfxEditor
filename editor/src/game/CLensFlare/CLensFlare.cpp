#include "CLensFlare.h"


LensFlareHandler::LensFlareHandler()
{
	// 7FF6E925C230
	m_CLensFlare = gmAddress::Scan("48 8D 0D ?? ?? ?? ?? 75 ?? 33 D2")
		.GetRef(3)
		.To<gCLensFlare*>();

	//std::cout << m_CLensFlare << std::endl;
	//std::cout << &(m_CLensFlare->m_Settings[0].m_arrFlareFX) << std::endl;
	//

	//for (size_t i = 0; i < 17; i++)
	//{
	//	std::cout << *(((float*)(&m_CLensFlare->m_Settings[0].m_fSunVisibilityFactor)) + i) << '\n';
	//}



}
