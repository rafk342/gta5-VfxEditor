#pragma once
#include <map>
#include "ptxEvolution.h"
#include "rage/dat/datBase.h"
#include "rage/paging/ref.h"

namespace rage
{
	struct ptxKeyframeProp : public datBase
	{
		const char* getPropertyName() const
		{
			static std::map<u32, const char*> names = 
			{
				{ rage::joaat("ptxd_Trail:m_texInfoKFP") ,"ptxd_Trail:m_texInfoKFP"},
				{ rage::joaat("ptxu_Acceleration:m_strengthKFP") ,"ptxu_Acceleration:m_strengthKFP"},
				{ rage::joaat("ptxu_Acceleration:m_xyzMaxKFP") ,"ptxu_Acceleration:m_xyzMaxKFP"},
				{ rage::joaat("ptxu_Acceleration:m_xyzMinKFP") ,"ptxu_Acceleration:m_xyzMinKFP"},
				{ rage::joaat("ptxu_AnimateTexture:m_animRateKFP") ,"ptxu_AnimateTexture:m_animRateKFP"},
				{ rage::joaat("ptxu_Collision:m_bounceDirVarKFP") ,"ptxu_Collision:m_bounceDirVarKFP"},
				{ rage::joaat("ptxu_Collision:m_bouncinessKFP") ,"ptxu_Collision:m_bouncinessKFP"},
				{ rage::joaat("ptxu_Colour:m_emissiveIntensityKFP") ,"ptxu_Colour:m_emissiveIntensityKFP"},
				{ rage::joaat("ptxu_Colour:m_rgbaMaxKFP") ,"ptxu_Colour:m_rgbaMaxKFP"},
				{ rage::joaat("ptxu_Colour:m_rgbaMinKFP") ,"ptxu_Colour:m_rgbaMinKFP"},
				{ rage::joaat("ptxu_Dampening:m_xyzMaxKFP") ,"ptxu_Dampening:m_xyzMaxKFP"},
				{ rage::joaat("ptxu_Dampening:m_xyzMinKFP") ,"ptxu_Dampening:m_xyzMinKFP"},
				{ rage::joaat("ptxu_Decal:m_alphaKFP") ,"ptxu_Decal:m_alphaKFP"},
				{ rage::joaat("ptxu_Decal:m_dimensionsKFP") ,"ptxu_Decal:m_dimensionsKFP"},
				{ rage::joaat("ptxu_FogVolume:m_densityRangeKFP") ,"ptxu_FogVolume:m_densityRangeKFP"},
				{ rage::joaat("ptxu_FogVolume:m_rgbTintMaxKFP") ,"ptxu_FogVolume:m_rgbTintMaxKFP"},
				{ rage::joaat("ptxu_FogVolume:m_rgbTintMinKFP") ,"ptxu_FogVolume:m_rgbTintMinKFP"},
				{ rage::joaat("ptxu_FogVolume:m_rotationMaxKFP") ,"ptxu_FogVolume:m_rotationMaxKFP"},
				{ rage::joaat("ptxu_FogVolume:m_rotationMinKFP") ,"ptxu_FogVolume:m_rotationMinKFP"},
				{ rage::joaat("ptxu_FogVolume:m_scaleMaxKFP") ,"ptxu_FogVolume:m_scaleMaxKFP"},
				{ rage::joaat("ptxu_FogVolume:m_scaleMinKFP") ,"ptxu_FogVolume:m_scaleMinKFP"},
				{ rage::joaat("ptxu_Light:m_coronaFlareKFP") ,"ptxu_Light:m_coronaFlareKFP"},
				{ rage::joaat("ptxu_Light:m_coronaIntensityKFP") ,"ptxu_Light:m_coronaIntensityKFP"},
				{ rage::joaat("ptxu_Light:m_coronaRgbMaxKFP") ,"ptxu_Light:m_coronaRgbMaxKFP"},
				{ rage::joaat("ptxu_Light:m_coronaRgbMinKFP") ,"ptxu_Light:m_coronaRgbMinKFP"},
				{ rage::joaat("ptxu_Light:m_coronaSizeKFP") ,"ptxu_Light:m_coronaSizeKFP"},
				{ rage::joaat("ptxu_Light:m_intensityKFP") ,"ptxu_Light:m_intensityKFP"},
				{ rage::joaat("ptxu_Light:m_rangeKFP") ,"ptxu_Light:m_rangeKFP"},
				{ rage::joaat("ptxu_Light:m_rgbMaxKFP") ,"ptxu_Light:m_rgbMaxKFP"},
				{ rage::joaat("ptxu_Light:m_rgbMinKFP") ,"ptxu_Light:m_rgbMinKFP"},
				{ rage::joaat("ptxu_MatrixWeight:m_mtxWeightKFP") ,"ptxu_MatrixWeight:m_mtxWeightKFP"},
				{ rage::joaat("ptxu_Noise:m_posNoiseMaxKFP") ,"ptxu_Noise:m_posNoiseMaxKFP"},
				{ rage::joaat("ptxu_Noise:m_posNoiseMinKFP") ,"ptxu_Noise:m_posNoiseMinKFP"},
				{ rage::joaat("ptxu_Noise:m_velNoiseMaxKFP") ,"ptxu_Noise:m_velNoiseMaxKFP"},
				{ rage::joaat("ptxu_Noise:m_velNoiseMinKFP") ,"ptxu_Noise:m_velNoiseMinKFP"},
				{ rage::joaat("ptxu_Rotation:m_angleMaxKFP") ,"ptxu_Rotation:m_angleMaxKFP"},
				{ rage::joaat("ptxu_Rotation:m_angleMinKFP") ,"ptxu_Rotation:m_angleMinKFP"},
				{ rage::joaat("ptxu_Rotation:m_initialAngleMaxKFP") ,"ptxu_Rotation:m_initialAngleMaxKFP"},
				{ rage::joaat("ptxu_Rotation:m_initialAngleMinKFP") ,"ptxu_Rotation:m_initialAngleMinKFP"},
				{ rage::joaat("ptxu_Size:m_tblrScalarKFP") ,"ptxu_Size:m_tblrScalarKFP"},
				{ rage::joaat("ptxu_Size:m_tblrVelScalarKFP") ,"ptxu_Size:m_tblrVelScalarKFP"},
				{ rage::joaat("ptxu_Size:m_whdMaxKFP") ,"ptxu_Size:m_whdMaxKFP"},
				{ rage::joaat("ptxu_Size:m_whdMinKFP") ,"ptxu_Size:m_whdMinKFP"},
				{ rage::joaat("ptxu_Wind:m_influenceKFP") ,"ptxu_Wind:m_influenceKFP"},
				{ rage::joaat("ptxu_ZCull:m_fadeDistKFP") ,"ptxu_ZCull:m_fadeDistKFP"},
				{ rage::joaat("ptxu_ZCull:m_heightKFP") ,"ptxu_ZCull:m_heightKFP"},
			};
			auto it = names.find(m_propertyId);
			return it == names.end() ? "Unknown" : it->second;
		}


		ptxEvolvedKeyframeProp* m_pEvolvedKeyframeProp[12]{};
		u32 m_propertyId;	// hash
		bool m_invertBiasLink;
		char m_randIndex;
		u8 m_pad[2];
		ptxKeyframe m_keyframe;
	};

	struct ptxKeyframePropList
	{
		atArray<datRef<ptxKeyframeProp>> m_pKeyframeProps;
	};
}