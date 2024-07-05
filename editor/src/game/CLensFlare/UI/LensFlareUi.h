#pragma once
#include <iostream>

#include "../CLensFlare.h"

#include "ImGui/imgui.h"
#include "uiBase/uiBaseWindow.h"
#include "uiBase/ImguiHelpers.h"
#include "scripthookTh.h"


class LensFlareUi : public App
{
	LensFlareHandler m_Handler;

	void TreeNodeForAnimorphicType	(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex);
	void TreeNodeForArtefactType	(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex);
	void TreeNodeForChromaticType	(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex);
	void TreeNodeForCoronaType		(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex);

public:

	LensFlareUi(const char* title);

	virtual void window() override;
	virtual void importData(std::filesystem::path path) override;
	virtual void exportData(std::filesystem::path path) override;

};

