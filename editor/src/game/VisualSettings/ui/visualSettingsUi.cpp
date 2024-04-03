#include "visualSettingsUi.h"

bool VisualSettingsUi::check_if_category_should_be_shown(const char* category)
{
	auto& Params = mHandler.mContainer.paramsMap.at(category);

	for (auto& item : Params)
	{
		if (hide_unused)
		{
			if (item.found && item.is_used)
			{
				return true;
			}
		}
		else
		{
			if (item.found)
			{
				return true;
			}
		}
	}
	return false;
}


void VisualSettingsUi::window()
{
	static float col3[3];

	ImGui::Checkbox("Hide Unused", &hide_unused);
	ImGui::Separator();

	for (auto category : mHandler.mContainer.categoriesOrder)
	{
		if (!mHandler.mContainer.paramsMap.contains(category))
			continue;

		bool should_header_be_shown = check_if_category_should_be_shown(category);

		if (!should_header_be_shown)
			continue;

		auto& Params = mHandler.mContainer.paramsMap.at(category);

		if (ImGui::CollapsingHeader(category))
		{
			for (u32 i = 0; i < Params.size(); i++)
			{
				if (hide_unused)
				{
					if (!Params[i].is_used)
						continue;
				}

				if (!Params[i].found || !Params[i].gPtrItem)
					continue;

				switch (Params[i].vType)
				{
				case Vs_VarType_e::V_NONE:
					break;

				case Vs_VarType_e::V_FLOAT:
					
					if (ImGui::DragFloat(Params[i].labelName.c_str(), &Params[i].gPtrItem->value, 0.005f))
						mHandler.updateData(); 

					break;

				case Vs_VarType_e::V_COL3:

					for (u8 w = 0; w < 3; w++)
						col3[w] = Params[i + w].gPtrItem->value;

					if (ImGui::ColorEdit3(Params[i].labelName.c_str(), col3))
					{
						for (u8 w = 0; w < 3; w++)
							Params[i + w].gPtrItem->value = col3[w];

						mHandler.updateData();
					}
					break;

				default:
					break;
				}
			}
		}
	}
}

void VisualSettingsUi::importData(std::string path)
{
	mHandler.importData(path);
}

void VisualSettingsUi::exportData(std::string path)
{
	mHandler.exportData(path);
}
