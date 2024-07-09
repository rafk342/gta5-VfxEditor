#include "VfxLightningUi.h"


void VfxLightningUi::window()
{
	static bool mOverride_state = false;
	static int idx;
	static const char* typeNames[4] =
	{
		"None",
		"Directional Burst",
		"Cloud Burst",
		"Strike",
	};
	
	if (ImGui::TreeNode("Override##T_override"))
	{
		PushStyleCompact();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

		if (ImGui::Checkbox("Enable override##T_override", &mOverride_state))
		{
			mVfxLightingHandler.setOverrideState(mOverride_state);
		}

		if (mOverride_state)
		{
			if (ImGui::Combo("Type Name##T_override", (int*)&idx, typeNames, 4))
			{
				mVfxLightingHandler.setCurrentUpdateType(0);
				mVfxLightingHandler.setOverrideType(idx);
			}
		}

		PopStyleCompact();
		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Requests"))
	{
		PushStyleCompact();
		float offset = /*148.0f*/ImGui::CalcTextSize("Clear Update Type").x * 1.2;
		float cursor_pos = ImGui::GetCursorPosX();


		ImGui::Text("Clear Update Type");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursor_pos + offset);
		if (ImGui::Button("Request##Force Clear")) {
			mVfxLightingHandler.setCurrentUpdateType(0);
		}
		
		ImGui::Text("Directional Burst");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursor_pos + offset);
		if (ImGui::Button("Request##Directional Burst")) {
			mVfxLightingHandler.DirBurstSeqRequest();
		}
		
		ImGui::Text("Cloud Burst");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursor_pos + offset);
		if (ImGui::Button("Request##Cloud Lightning ")) {
			mVfxLightingHandler.CloudLightningSeqRequest();
		}
		
		ImGui::Text("Lightning Strike");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursor_pos + offset);
		if (ImGui::Button("Request##Lightning")) {
			mVfxLightingHandler.LightningRequest();
		}
	
		PopStyleCompact();
		ImGui::TreePop();
	}
	ImGui::Separator();
	
	ImGui::Text("Current Update Type : %i", mVfxLightingHandler.getCurrentUpdateType());
	
	PushStyleCompact();
	
	if (!mOverride_state) 
	{
		ImGui::DragInt("Lightning Occurrance Chance", &mVfxLightingHandler.mVfxLightningSettings->lightningOccurranceChance, 1, 0, 200);
	}
	ImGui::DragFloat("Lightning Shake Intensity", &mVfxLightingHandler.mVfxLightningSettings->lightningShakeIntensity, 0.0025, 0.0f, 1.0f);
	
	PopStyleCompact();
	
	LightningTimeCycleModsWidgets();
	DirectionalBurstSettingsWidgets();
	CloudBurstSettingsWidgets();
	StrikeSettingsWidgets();
}


void VfxLightningUi::LightningTimeCycleModsWidgets()
{
#if 0
	if (ImGui::CollapsingHeader("TimeCycle Modifiers"))
	{

	}
#endif
}


void VfxLightningUi::DirectionalBurstSettingsWidgets()
{
	static auto& dirBurstSettings = mVfxLightingHandler.mVfxLightningSettings->m_DirectionalBurstSettings;

	if (ImGui::CollapsingHeader("Directional Burst Settings"))
	{
		PushStyleCompact();
		if (ImGui::TreeNode("Main Settings ##__DirBurst"))
		{
			ImGui::DragFloat("Intensity Min ##__DirBurstSettings", &dirBurstSettings.BurstIntensityMin, 0.1f, 0.0f, 3000.0f);
			ImGui::DragFloat("Intensity Max ##__DirBurstSettings", &dirBurstSettings.BurstIntensityMax, 0.1f, 0.0f, 3000.0f);
			ImGui::DragFloat("Duration Min ##__DirBurstSettings", &dirBurstSettings.BurstDurationMin, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("Duration Max ##__DirBurstSettings", &dirBurstSettings.BurstDurationMax, 0.01f, 0.0f, 10.0f);
			ImGui::DragInt("Max Sequence Count ##__DirBurstSettings", &dirBurstSettings.BurstSeqCount, 0.5f, 1, 10);
			ImGui::DragFloat("Root Orbit X Variance ##__DirBurstSettings", &dirBurstSettings.RootOrbitXVariance, 0.1f, 0.0f, 180.0f);
			ImGui::DragFloat("Root Orbit Y Min ##__DirBurstSettings", &dirBurstSettings.RootOrbitYVarianceMin, 0.1f, -180.0f, 180.0f);
			ImGui::DragFloat("Root Orbit Y Max ##__DirBurstSettings", &dirBurstSettings.RootOrbitYVarianceMax, 0.1f, -180.0f, 180.0f);
			ImGui::DragFloat("Sequence Orbit X Variance ##__DirBurstSettings", &dirBurstSettings.BurstSeqOrbitXVariance, 0.1f, -180.0f, 180.0f);
			ImGui::DragFloat("Sequence Orbit Y Variance ##__DirBurstSettings", &dirBurstSettings.BurstSeqOrbitYVariance, 0.1f, -180.0f, 180.0f);
			
			ImGui::TreePop();
		}
		PopStyleCompact();
	}
}


void VfxLightningUi::CloudBurstCommonSettingsWidgets(CloudBurstCommonSettings& rCloudBurstCommonSettings, const char* treeName)
{
	if (ImGui::TreeNode(treeName))
	{
		ImGui::DragFloat(vfmt("Burst Duration Min ## {}", treeName), &rCloudBurstCommonSettings.BurstDurationMin, 0.1f, 0.0f, 10.f);
		ImGui::DragFloat(vfmt("Burst Duration Max ## {}", treeName), &rCloudBurstCommonSettings.BurstDurationMax, 0.1f, 0.0f, 10.f);
		ImGui::DragInt(vfmt("Burst Sequence Count ## {}", treeName), &rCloudBurstCommonSettings.BurstSeqCount, 1, 1, 10);
		ImGui::DragFloat(vfmt("Root Orbit X Variance ## {}", treeName), &rCloudBurstCommonSettings.RootOrbitXVariance, 0.1f, 0.0f, 180.0f);
		ImGui::DragFloat(vfmt("Root Orbit Y Min ## {}", treeName), &rCloudBurstCommonSettings.RootOrbitYVarianceMin, 0.1f, 0.0f, 90.0f);
		ImGui::DragFloat(vfmt("Root Orbit Y Max ## {}", treeName), &rCloudBurstCommonSettings.RootOrbitYVarianceMax, 0.1f, 0.0f, 90.0f);
		ImGui::DragFloat(vfmt("Local Orbit X Variance ## {}", treeName), &rCloudBurstCommonSettings.LocalOrbitXVariance, 0.1f, -180.0f, 180.0f);
		ImGui::DragFloat(vfmt("Local Orbit Y Variance ## {}", treeName), &rCloudBurstCommonSettings.LocalOrbitYVariance, 0.1f, -180.0f, 180.0f);
		ImGui::DragFloat(vfmt("Sequence Orbit X Variance ## {}", treeName), &rCloudBurstCommonSettings.BurstSeqOrbitXVariance, 0.1f, -180.0f, 180.0f);
		ImGui::DragFloat(vfmt("Sequence Orbit Y Variance ## {}", treeName), &rCloudBurstCommonSettings.BurstSeqOrbitYVariance, 0.1f, -180.0f, 180.0f);
		ImGui::DragFloat(vfmt("Delay Min ## {}", treeName), &rCloudBurstCommonSettings.DelayMin, 0.1f, 0.0f, 90.0f);
		ImGui::DragFloat(vfmt("Delay Max ## {}", treeName), &rCloudBurstCommonSettings.DelayMax, 0.1f, 0.0f, 90.0f);
		ImGui::DragFloat(vfmt("Size Min ## {}", treeName), &rCloudBurstCommonSettings.SizeMin, 0.1f, 0.0f, 5000.0f);
		ImGui::DragFloat(vfmt("Size Max ## {}", treeName), &rCloudBurstCommonSettings.SizeMax, 0.1f, 0.0f, 5000.0f);

		ImGui::TreePop();
	}
}


void VfxLightningUi::CloudBurstSettingsWidgets()
{
	static auto& rCloudBurstSettings = mVfxLightingHandler.mVfxLightningSettings->m_CloudBurstSettings;
	static auto& rCloudBurstCommonSettings = mVfxLightingHandler.mVfxLightningSettings->m_CloudBurstSettings.m_CloudBurstCommonSettings;
	float col[4];

	if (ImGui::CollapsingHeader("Cloud Burst Settings"))
	{
		PushStyleCompact();
		
		if (ImGui::TreeNode("Main Settings ##__ClBurst"))
		{
			ImGui::DragFloat("Intensity Min ##__CLBurstSettings", &rCloudBurstSettings.BurstIntensityMin, 0.1f, 0.0f, 3000.0f);
			ImGui::DragFloat("Intensity Max ##__CLBurstSettings", &rCloudBurstSettings.BurstIntensityMax, 0.1f, 0.0f, 3000.0f);
			ImGui::DragFloat("Light Source Exponent Fall Off ##__CLBurstSettings", &rCloudBurstSettings.LightSourceExponentFalloff, 0.1f, 0.0f, 128.0f);
			ImGui::DragFloat("Light Source Delta Position ##__CLBurstSettings", &rCloudBurstSettings.LightDeltaPos, 0.1f, -5000.f, 5000.f);
			ImGui::DragFloat("Light Source Distance ##__CLBurstSettings", &rCloudBurstSettings.LightDistance, 0.1f, 0.f, 5000.f);

			rCloudBurstSettings.LightColor.Getf_col4().ToArray(col);

			if (ImGui::ColorEdit4("Light Color", col)) {
				rCloudBurstSettings.LightColor.Setf_col4(col);
			}
			ImGui::TreePop();
		}
		CloudBurstCommonSettingsWidgets(rCloudBurstCommonSettings, "Cloud Burst Common Settings");
		
		PopStyleCompact();
	}
}


void VfxLightningUi::StrikeSettingsWidgets()
{
	static auto& rStrikeSettings = mVfxLightingHandler.mVfxLightningSettings->m_StrikeSettings;
	float col[4];

	if (ImGui::CollapsingHeader("Strike Settings"))
	{
		PushStyleCompact();

		if (ImGui::TreeNode("Main Settings ## __CLStrikeSettings"))
		{
			ImGui::DragFloat("Intensity Min ##__CLStrikeSettings", &rStrikeSettings.IntensityMin, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("Intensity Max ##__CLStrikeSettings", &rStrikeSettings.IntensityMax, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("Intensity Min Clamp ##__CLStrikeSettings", &rStrikeSettings.IntensityMinClamp, 0.1f, 0.0f, 50.0f);

			ImGui::DragFloat("Width Min ##__CLStrikeSettings", &rStrikeSettings.WidthMin, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Width Max ##__CLStrikeSettings", &rStrikeSettings.WidthMax, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Width Min Clamp ##__CLStrikeSettings", &rStrikeSettings.WidthMinClamp, 0.1f, 0.0f, 100.0f);

			ImGui::DragFloat("Intensity Flicker Min ##__CLStrikeSettings", &rStrikeSettings.IntensityFlickerMin, 0.1f, 0.0f, 2.0f);
			ImGui::DragFloat("Intensity Flicker Max ##__CLStrikeSettings", &rStrikeSettings.IntensityFlickerMax, 0.1f, 0.0f, 2.0f);
			ImGui::DragFloat("Intensity Flicker Frequency ##__CLStrikeSettings", &rStrikeSettings.IntensityFlickerFrequency, 0.1f, 1.0f, 200.0f);

			ImGui::DragFloat("Intensity Level Decay Min ##__CLStrikeSettings", &rStrikeSettings.IntensityLevelDecayMin, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Intensity Level Decay Max ##__CLStrikeSettings", &rStrikeSettings.IntensityLevelDecayMax, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Width Level Decay Min ##__CLStrikeSettings", &rStrikeSettings.WidthLevelDecayMin, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Width Level Decay Max ##__CLStrikeSettings", &rStrikeSettings.WidthLevelDecayMax, 0.1f, 0.0f, 1.0f);

			ImGui::DragFloat("Noise Tex Scale ##__CLStrikeSettings", &rStrikeSettings.NoiseTexScale, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Noise Amplitude ##__CLStrikeSettings", &rStrikeSettings.NoiseAmplitude, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Noise Amp Min Dist Lerp ##__CLStrikeSettings", &rStrikeSettings.NoiseAmpMinDistLerp, 0.1f, 0.0f, 10000.0);
			ImGui::DragFloat("Noise Amp Max Dist Lerp ##__CLStrikeSettings", &rStrikeSettings.NoiseAmpMaxDistLerp, 0.1f, 0.0f, 10000.0);

			int subdiv_count = static_cast<int>(rStrikeSettings.SubdivisionCount);
			if (ImGui::DragInt("Subdivisions Count  ##__CLStrikeSettings", &subdiv_count, 1.0f, 1, 5)) {
				if (subdiv_count > 5) subdiv_count = 5;
				if (subdiv_count < 1) subdiv_count = 1;
				rStrikeSettings.SubdivisionCount = static_cast<u8>(subdiv_count);
			}

			ImGui::DragFloat("Orbit Direction X Variance  ##__CLStrikeSettings", &rStrikeSettings.OrbitDirXVariance, 0.1f, 0.0f, 180.0f);
			ImGui::DragFloat("Orbit Direction Y Min  ##__CLStrikeSettings", &rStrikeSettings.OrbitDirYVarianceMin, 0.1f, 0.0, 90.0f);
			ImGui::DragFloat("Orbit Direction Y Max  ##__CLStrikeSettings", &rStrikeSettings.OrbitDirYVarianceMax, 0.1f, 0.0, 90.0f);
			ImGui::DragFloat("Max Distance for Exposure Reset  ##__CLStrikeSettings", &rStrikeSettings.MaxDistanceForExposureReset, 0.1f, 0.0f, 10000.0f);
			ImGui::DragFloat("Angular Width Factor  ##__CLStrikeSettings", &rStrikeSettings.AngularWidthFactor, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Max Height For Strikes  ##__CLStrikeSettings", &rStrikeSettings.MaxHeightForStrike, 0.1f, 0.0f, 2000.0f);
			ImGui::DragFloat("Corona Intensity Multiplier  ##__CLStrikeSettings", &rStrikeSettings.CoronaIntensityMult, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("Corona Size Factor  ##__CLStrikeSettings", &rStrikeSettings.BaseCoronaSize, 0.1f, 0.0, 100.0);

			rStrikeSettings.BaseColor.Getf_col4().ToArray(col);
			if (ImGui::ColorEdit4("Base Color  ##__CLStrikeSettings", col)) {
				rStrikeSettings.BaseColor.Setf_col4(col);
			}

			rStrikeSettings.FogColor.Getf_col4().ToArray(col);
			if (ImGui::ColorEdit4("Fog Color  ##__CLStrikeSettings", col)) {
				rStrikeSettings.FogColor.Setf_col4(col);
			}
			ImGui::DragFloat("Cloud Light Radius  ##__CLStrikeSettings", &rStrikeSettings.CloudLightRadius, 0.1f, 0.0, 5000.0f);
			ImGui::DragFloat("Cloud Light Delta Position  ##__CLStrikeSettings", &rStrikeSettings.CloudLightDeltaPos, 0.1f, -5000.0f, 5000.0f);
			ImGui::DragFloat("Cloud Light Intensity Multiplier  ##__CLStrikeSettings", &rStrikeSettings.CloudLightIntensityMult, 0.1f, 0.0f, 2000.0f);
			ImGui::DragFloat("Cloud Light Exponent Fall Off  ##__CLStrikeSettings", &rStrikeSettings.CloudLightFallOffExponent, 0.1f, 0.0f, 128.0f);
			ImGui::DragFloat("Max Distance for Burst  ##__CLStrikeSettings", &rStrikeSettings.MaxDistanceForBurst, 0.1f, 0.0f, 10000.0f);
			ImGui::DragFloat("Burst Threshold Intensity  ##__CLStrikeSettings", &rStrikeSettings.BurstThresholdIntensity, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Lightning Fade Width  ##__CLStrikeSettings", &rStrikeSettings.LightningFadeWidth, 0.1f, 1.0f, 100.0f);
			ImGui::DragFloat("Lightning Fade Width Falloff Exponent  ##__CLStrikeSettings", &rStrikeSettings.LightningFadeWidthFalloffExp, 0.1f, 0.0f, 128.0f);
		
			ImGui::TreePop();
		}
		
		CloudBurstCommonSettingsWidgets(rStrikeSettings.m_CloudBurstCommonSettings, "Cloud Burst Common Settings ## StrikeSettings ");

		if (ImGui::TreeNode("Strike Variations"))
		{
			for (u8 i = 0; i < 3; i++)
			{
				StrikeVariationsWidgets(i);
			}
			ImGui::TreePop();
		}

		PopStyleCompact();
	}
}


void VfxLightningUi::StrikeVariationsWidgets(u8 idx)
{
	auto& var = mVfxLightingHandler.mVfxLightningSettings->m_StrikeSettings.m_Variations[idx];
	
	if (ImGui::TreeNode(vfmt("Variation : {} ##__CLStrikeSettings", idx)))
	{
		ImGui::DragFloat(vfmt("Duration Min ##__CLStrikeSettings {}", idx), &var.DurationMin, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat(vfmt("Duration Max ##__CLStrikeSettings {}", idx), &var.DurationMax, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat(vfmt("Animation Time Min ##__CLStrikeSettings {}", idx), &var.AnimationTimeMin, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat(vfmt("Animation Time Max ##__CLStrikeSettings {}", idx), &var.AnimationTimeMax, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat(vfmt("End Point Offset X Variance ##__CLStrikeSettings {}", idx), &var.EndPointOffsetXVariance, 0.01f, -1.0f, 1.0f);

		int v = static_cast<int>(var.SubdivisionPatternMask);
		
		if (ImGui::DragInt(vfmt("Subdivision Pattern Mask ##__CLStrikeSettings {}", idx), &v, 0.5f, 0, 255)) {
			if (v > 255) v = 255;
			if (v < 0) v = 0;
			var.SubdivisionPatternMask = static_cast<u8>(v);
		}

		if (ImGui::TreeNode(vfmt("Zig Zag Split Point ##__CLStrikeSettings {}", idx)))
		{
			ImGui::DragFloat(vfmt("Fraction Min ##__CLStrikeSettings {}", idx), &var.mZigZagSplitPoint.FractionMin, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(vfmt("Fraction Max ##__CLStrikeSettings {}", idx), &var.mZigZagSplitPoint.FractionMax, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(vfmt("Deviation Decay ##__CLStrikeSettings {}", idx), &var.mZigZagSplitPoint.DeviationDecay, 0.1f, 0.0f, 32.0f);
			ImGui::DragFloat(vfmt("Deviation Right Variance ##__CLStrikeSettings {}", idx), &var.mZigZagSplitPoint.DeviationRightVariance, 0.01f, 0.0f, 1.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(vfmt("Fork Zig Zag Split Point ##__CLStrikeSettingsFork {}", idx)))
		{
			ImGui::DragFloat(vfmt("Fraction Min ##__CLStrikeSettingsFork {}", idx), &var.mForkZigZagSplitPoint.FractionMin, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(vfmt("Fraction Max ##__CLStrikeSettingsFork {}", idx), &var.mForkZigZagSplitPoint.FractionMax, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(vfmt("Deviation Decay ##__CLStrikeSettingsFork {}", idx), &var.mForkZigZagSplitPoint.DeviationDecay, 0.1f, 0.0f, 32.0f);
			ImGui::DragFloat(vfmt("Deviation Right Variance ##__CLStrikeSettingsFork {}", idx), &var.mForkZigZagSplitPoint.DeviationRightVariance, 0.01f, 0.0f, 1.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(vfmt("Fork Point ##__CLStrikeSettings ForkP {}", idx)))
		{
			ImGui::DragFloat(vfmt("Deviation Right Variance ##__CLStrikeSettings ForkP {}", idx), &var.mForkPoint.DeviationRightVariance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(vfmt("Deviation Forward Min ##__CLStrikeSettings ForkP {}", idx), &var.mForkPoint.DeviationForwardMin, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(vfmt("Deviation Forward Max ##__CLStrikeSettings ForkP {}", idx), &var.mForkPoint.DeviationForwardMax, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(vfmt("Length Min ##__CLStrikeSettings ForkP {}", idx), &var.mForkPoint.LengthMin, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(vfmt("Length Max ##__CLStrikeSettings ForkP {}", idx), &var.mForkPoint.LengthMax, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(vfmt("Length Decay ##__CLStrikeSettings ForkP {}", idx), &var.mForkPoint.LengthDecay, 0.01f, 0.0f, 32.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(vfmt("Lightning Key Frames ##__CLStrikeSettings {}", idx)))
		{
			if (ImGui::TreeNode(vfmt("Lightning Main Intensity ##__CLStrikeSettings {}", idx)))
			{
				auto& keyframe = var.mKeyFrameData.LightningMainIntensity;
				//keyframePlot(data, buff);
				keyframeTable(vfmt("Lightning Main Intensity ##__CLStrikeSettings table {}", idx), keyframe, idx);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode(vfmt("Lightning Branch Intensity ##__CLStrikeSettings {}", idx)))
			{
				auto& keyframe = var.mKeyFrameData.LightningBranchIntensity;

				//keyframePlot(data, buff);
				keyframeTable(vfmt("## Lightning Branch__CLStrikeSettings table {}", idx), keyframe, idx);

				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
		
	}
}


void keyframeTable(const char* title, ptxKeyframe& keyframe, u8 idx)
{
	auto& data = keyframe.data;

	if (ImGui::BeginTable(title, 3, ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 45);
		ImGui::TableSetupColumn("Min");
		ImGui::TableSetupColumn("Max");
		ImGui::TableHeadersRow();

		for (u16 i = 0; i < data.size(); i++)
		{
			ImGui::TableNextRow();

			for (u8 j = 0; j < 3; j++)
			{
				ImGui::TableNextColumn();

				switch (j)
				{
				case 0:
					ImGui::Text(vfmt(" {:.3f} ", data[i].vTime[0], idx));
					break;
				case 1:
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					ImGui::DragFloat(vfmt("##BranchIntensity{}_{}_{}__keyframe_Min", i, idx, j), &data[i].vValue[0], 0.015f, 0.0f, 1.0f);
					break;
				case 2:
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					ImGui::DragFloat(vfmt("##BranchIntensity{}_{}_{}__keyframe_Max", i, idx, j), &data[i].vValue[1], 0.015f, 0.0f, 1.0f);
					break;
				default:
					break;
				}
			}
		}
		ImGui::EndTable();
	}
}


void VfxLightningUi::importData(std::filesystem::path path)
{
	mXmlParser.mImportLightningData(path, mVfxLightingHandler.mVfxLightningSettings);
}

void VfxLightningUi::exportData(std::filesystem::path path)
{
	mXmlParser.mExportLightningData(path, mVfxLightingHandler.mVfxLightningSettings);
}









#if 0

enum class scaleAx
{
	X,
	Y
};

template<scaleAx T>
double* draw_scale_lines(std::vector<double>& dots_min, std::vector<double>& dots_max);

void VfxLightningUi::keyframePlot(ptxKeyframe& keyframe, const char* title)
{
	static float point_size = 4.5;

	static float col_dots_min[4] = { 0.761, 0.679, 1.000, 1.000 };
	static float col_line_min[4] = { 0.348, 0.333, 0.523, 1.000 };

	static float col_dots_max[4] = { 1.000, 0.582, 0.582, 1.000 };
	static float col_line_max[4] = { 0.629, 0.313, 0.313, 1.000 };
	
	static float valuesForInfLines[4] = { 1,0,1,0 };
	
	static std::vector<double> dots_x;
	static std::vector<double> dots_min;
	static std::vector<double> dots_max;
	
	dots_x.clear();
	dots_min.clear();
	dots_max.clear();

	for (u16 i = 0; i < keyframe.data.GetSize(); i++)
	{
		dots_x.push_back(keyframe.data[i].vTime[0]);
		dots_min.push_back(keyframe.data[i].vValue[0]);
		dots_max.push_back(keyframe.data[i].vValue[1]);
	}



	if (ImPlot::BeginPlot("mPlot", { -1, 400 }, ImPlotFlags_NoTitle | ImPlotFlags_NoMouseText | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoFrame | ImPlotFlags_NoMenus))
	{
		ImPlot::SetupAxes("key", "value");
		ImPlot::SetupAxesLimits(0, 1, -0.201, 1.38);

		ImPlot::SetNextLineStyle({ col_line_min[0], col_line_min[1], col_line_min[2], col_line_min[3] }, 3.0f);
		ImPlot::PlotLine("min", dots_x.data(), dots_min.data(), dots_min.size());

		ImPlot::SetNextLineStyle({ col_line_max[0], col_line_max[1], col_line_max[2], col_line_max[3] }, 3.0f);
		ImPlot::PlotLine("max", dots_x.data(), dots_max.data(), dots_max.size());

		ImPlot::PlotInfLines("##Inf", dots_x.data(), dots_x.size());
		ImPlot::PlotInfLines("##Inf", valuesForInfLines, 2);
		ImPlot::PlotInfLines("##Inf", valuesForInfLines + 2, 2, ImPlotInfLinesFlags_Horizontal);

		static double* l_x;
		static double* l_y;

		for (uint_fast16_t i = 0; i < dots_min.size(); i++)
		{
			l_x = draw_scale_lines<scaleAx::X>(dots_x, dots_x);
			l_y = draw_scale_lines<scaleAx::Y>(dots_min, dots_max);

			if (ImPlot::DragPoint(i + dots_min.size(), (double*)&dots_x[i], (double*)&dots_max[i], { col_dots_max[0], col_dots_max[1], col_dots_max[2], col_dots_max[3] }, point_size, ImPlotDragToolFlags_None))
			{
				if (dots_max[i] < dots_min[i])
					dots_max[i] = dots_min[i];
				if (dots_min[i] > dots_max[i])
					dots_min[i] = dots_max[i];
			}
			if (ImPlot::DragPoint(i, (double*)&dots_x[i], (double*)&dots_min[i], { col_dots_min[0], col_dots_min[1], col_dots_min[2], col_dots_min[3] }, point_size, ImPlotDragToolFlags_None))
			{
				if (dots_min[i] > dots_max[i])
					dots_min[i] = dots_max[i];
				if (dots_max[i] < dots_min[i])
					dots_max[i] = dots_min[i];
			}

			std::cout << l_y[0] << " " << l_y[1] << '\n';

			if (dots_x[i] < l_x[0])
				dots_x[i] = l_x[0];
			if (dots_x[i] > l_x[1])
				dots_x[i] = l_x[1];

			if (dots_min[i] > l_y[1])
				dots_min[i] = l_y[1];
			if (dots_min[i] < l_y[0])
				dots_min[i] = l_y[0];

			if (dots_max[i] > l_y[1])
				dots_max[i] = l_y[1];
			if (dots_max[i] < l_y[0])
				dots_max[i] = l_y[0];

			if (i != 0 && i != dots_x.size() - 1)
			{
				if (dots_x[i] > dots_x[i + 1] - 0.005)
					dots_x[i] = dots_x[i + 1] - 0.005;
				if (dots_x[i] < dots_x[i - 1] + 0.005)
					dots_x[i] = dots_x[i - 1] + 0.005;
			}
			else if (i == dots_x.size() - 1)
			{
				if (dots_x[i] < dots_x[i - 1] + 0.005)
					dots_x[i] = dots_x[i - 1] + 0.005;
			}
			else if (i == 0)
			{
				if (dots_x[i] > dots_x[i + 1] - 0.005)
					dots_x[i] = dots_x[i + 1] - 0.005;
			}
		}
		ImPlot::EndPlot();
	}


	for (u16 i = 0; i < keyframe.data.GetSize(); i++)
	{
		keyframe.data[i].vTime[0] = dots_x[i];
		keyframe.data[i].vValue[0] = dots_min[i];
		keyframe.data[i].vValue[1] = dots_max[i];
	}
}


template<scaleAx T>
double* draw_scale_lines(std::vector<double>& dots_min, std::vector<double>& dots_max)
{

	static ImVec4 lines_col = { 0.5, 0.5, 0.5, 1 };
	static float lines_thickness = 1;

	static double prev_pos1 = 1;
	static double curr_pos1 = 1;

	prev_pos1 = curr_pos1;

	static double prev_pos2 = 1;
	static double curr_pos2 = 0;

	prev_pos2 = curr_pos2;

	switch (T)
	{
	case scaleAx::X:

		if (ImPlot::DragLineX(1, &curr_pos1, lines_col, lines_thickness))
		{
			if (curr_pos1 < curr_pos2 + 0.2)
				curr_pos1 = curr_pos2 + 0.2;
		}
		if (ImPlot::DragLineX(2, &curr_pos2, lines_col, lines_thickness))
		{
			if (curr_pos2 > curr_pos1 - 0.2)
				curr_pos2 = curr_pos1 - 0.2;
		}
		break;

	case scaleAx::Y:

		if (ImPlot::DragLineY(1, &curr_pos1, lines_col, lines_thickness))
		{
			if (curr_pos1 < curr_pos2 + 0.05)
				curr_pos1 = curr_pos2 + 0.05;
		}
		if (ImPlot::DragLineY(2, &curr_pos2, lines_col, lines_thickness))
		{
			if (curr_pos2 > curr_pos1 - 0.05)
				curr_pos2 = curr_pos1 - 0.05;
		}
		break;

	default:
		break;
	}

	if (curr_pos1 > 1)
		curr_pos1 = 1;
	if (curr_pos1 < 0)
		curr_pos1 = 0 + 0.05;

	if (curr_pos2 < 0)
		curr_pos2 = 0;
	if (curr_pos2 > 1)
		curr_pos2 = 1 - 0.05;

	static double scale_f;

	scale_f = fabs((curr_pos2 - curr_pos1) / (prev_pos2 - prev_pos1));

	for (uint_fast16_t i = 0; i < dots_min.size(); i++)
	{
		dots_min[i] = (dots_min[i] - prev_pos1) * scale_f + curr_pos1;
		if (T == scaleAx::Y) dots_max[i] = (dots_max[i] - prev_pos1) * scale_f + curr_pos1;
	}

	static double lines_pos[2]{};

	lines_pos[0] = curr_pos2;   //right / lower  line
	lines_pos[1] = curr_pos1;   //left / upper  line

	return lines_pos;
}
#endif