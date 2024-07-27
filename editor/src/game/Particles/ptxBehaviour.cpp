#include "ParticlesUi.h"
#include "ptxBehaviour.h"

void rage::ptxBehaviour::UiWidgets(size_t EntryIdx)
{
    if (ImGui::TreeNode(vfmt("{}## {}", GetName(), EntryIdx)))
    {
        ImGui::Text("Order : %i", GetOrder());

        switch (static_cast<DerivedType>(m_hashName))
        {
        case DerivedType::ptxd_Model:
        {
            ptxd_Model* ptxdModel = reinterpret_cast<ptxd_Model*>(this);

            ImGui::InputInt(vfmt("Colour Control Shader Id##{}", NextUniqueNum()), (int*)&ptxdModel->m_colourControlShaderId);
            ImGui::DragFloat(vfmt("Camera Shrink##{}", NextUniqueNum()), &ptxdModel->m_cameraShrink);
            ImGui::DragFloat(vfmt("Shadow Cast Intensity##{}", NextUniqueNum()), &ptxdModel->m_shadowCastIntensity);
            ImGui::Checkbox(vfmt("Disable Draw##{}", NextUniqueNum()), &ptxdModel->m_disableDraw);

            break;
        }
        case DerivedType::ptxu_Acceleration:
        {
            ptxu_Acceleration* ptxuAcceleration = reinterpret_cast<ptxu_Acceleration*>(this);

            ptxKeyframePropTreeNode(ptxuAcceleration->m_xyzMinKFP, "Acceleration Min", EntryIdx, PTX_KF_FLOAT3, "MIN X", "MIN Y", "MIN Z");
            ptxKeyframePropTreeNode(ptxuAcceleration->m_xyzMaxKFP, "Acceleration Max", EntryIdx, PTX_KF_FLOAT3, "MAX X", "MAX Y", "MAX Z");

            ImGui::InputInt(vfmt("Reference Space##{}", NextUniqueNum()), &ptxuAcceleration->m_referenceSpace);
            ImGui::Checkbox(vfmt("Is Affected By Zoom##{}", NextUniqueNum()), &ptxuAcceleration->m_isAffectedByZoom);
            ImGui::Checkbox(vfmt("Enable Gravity##{}", NextUniqueNum()), &ptxuAcceleration->m_enableGravity);

            break;
        }
        case DerivedType::ptxd_Sprite:
        {
            ptxd_Sprite* ptxdSprite = reinterpret_cast<ptxd_Sprite*>(this);
 
            ImGui::DragFloat3(vfmt("Align Axis##{}", NextUniqueNum()), (float*)&ptxdSprite->m_alignAxis);
            ImGui::InputInt(vfmt("Align Mode##{}", NextUniqueNum()), &ptxdSprite->m_alignmentMode);

            ImGui::DragFloat(vfmt("Flip Chance U##{}", NextUniqueNum()), &ptxdSprite->m_flipChanceU);
            ImGui::DragFloat(vfmt("Flip Chance V##{}", NextUniqueNum()), &ptxdSprite->m_flipChanceV);

            ImGui::DragFloat(vfmt("Near Clip Distance##{}", NextUniqueNum()), &ptxdSprite->m_nearClipDist);
            ImGui::DragFloat(vfmt("Far Clip Distance##{}", NextUniqueNum()), &ptxdSprite->m_farClipDist);

            ImGui::DragFloat(vfmt("Projection Depth##{}", NextUniqueNum()), &ptxdSprite->m_projectionDepth);
            ImGui::DragFloat(vfmt("Shadow Cast Intensity##{}", NextUniqueNum()), &ptxdSprite->m_shadowCastIntensity);

            ImGui::Checkbox(vfmt("Is Screen Space##{}", NextUniqueNum()), &ptxdSprite->m_isScreenSpace);
            ImGui::Checkbox(vfmt("Is Hi Res##{}", NextUniqueNum()), &ptxdSprite->m_isHiRes);
            ImGui::Checkbox(vfmt("Near Clip##{}", NextUniqueNum()), &ptxdSprite->m_nearClip);
            ImGui::Checkbox(vfmt("Far Clip##{}", NextUniqueNum()), &ptxdSprite->m_farClip);
            ImGui::Checkbox(vfmt("UV Clip##{}", NextUniqueNum()), &ptxdSprite->m_uvClip);

            break;
        }
        case DerivedType::ptxd_Trail:
        {
            ptxd_Trail* ptxdTrail = reinterpret_cast<ptxd_Trail*>(this);
 
            ptxKeyframePropTreeNode(ptxdTrail->m_texInfoKFP, "Texture Info", EntryIdx, PTX_KF_FLOAT4, "Wrap Length Min", "Wrap Length Max", "Offset Min", "Offset Max");

            ImGui::DragFloat3(vfmt("Align Axis##{}", NextUniqueNum()), (float*)&ptxdTrail->m_alignAxis);
            ImGui::InputInt(vfmt("Align Mode##{}", NextUniqueNum()), &ptxdTrail->m_alignmentMode);

            ImGui::DragInt2(vfmt("TessellationUV##{}", NextUniqueNum()), (int*)&ptxdTrail->m_tessellationU);

            ImGui::DragFloat2(vfmt("SmoothnessXY##{}", NextUniqueNum()), (float*)&ptxdTrail->m_smoothnessX);

            ImGui::DragFloat(vfmt("Projection Depth##{}", NextUniqueNum()), &ptxdTrail->m_projectionDepth);
            ImGui::DragFloat(vfmt("Shadow Cast Intensity##{}", NextUniqueNum()), &ptxdTrail->m_shadowCastIntensity);

            ImGui::Checkbox(vfmt("Flip U##{}", NextUniqueNum()), &ptxdTrail->m_flipU);
            ImGui::Checkbox(vfmt("Flip V##{}", NextUniqueNum()), &ptxdTrail->m_flipV);
            ImGui::Checkbox(vfmt("Wrap Texture Over Particle Life##{}", NextUniqueNum()), &ptxdTrail->m_wrapTextureOverParticleLife);
            ImGui::Checkbox(vfmt("Disable Draw##{}", NextUniqueNum()), &ptxdTrail->m_disableDraw);

            break;
        }
        case DerivedType::ptxu_Age:
        {
            ptxu_Age* ptxuAge = reinterpret_cast<ptxu_Age*>(this);

            break;
        }
        case DerivedType::ptxu_AnimateTexture:
        {
            ptxu_AnimateTexture* ptxuAnimateTexture = reinterpret_cast<ptxu_AnimateTexture*>(this);
            ptxu_AnimateTexture& ref = (*ptxuAnimateTexture);

            ptxKeyframePropTreeNode(ref.m_animRateKFP, "Anim Rate", EntryIdx, PTX_KF_FLOAT2, "MIN FPS", "MAX FPS");

            ImGui::InputInt(vfmt("Keyframe Mode##{}", NextUniqueNum()), &ref.m_keyframeMode);
            ImGui::InputInt(vfmt("Last Frame Id##{}", NextUniqueNum()), &ref.m_lastFrameId);
            ImGui::InputInt(vfmt("Loop Mode##{}", NextUniqueNum()), &ref.m_loopMode);

            ImGui::Checkbox(vfmt("Is Randomised##{}", NextUniqueNum()), &ref.m_isRandomised);
            ImGui::Checkbox(vfmt("Is Scaled Over Particle Life##{}", NextUniqueNum()), &ref.m_isScaledOverParticleLife);
            ImGui::Checkbox(vfmt("Is Held On Last Frame##{}", NextUniqueNum()), &ref.m_isHeldOnLastFrame);
            ImGui::Checkbox(vfmt("Do Frame Blending##{}", NextUniqueNum()), &ref.m_doFrameBlending);

            break;
        }
        case DerivedType::ptxu_Attractor:
        {
            ptxu_Attractor* ptxuAttractor = reinterpret_cast<ptxu_Attractor*>(this);
            ptxu_Attractor& ref = (*ptxuAttractor);

            ptxKeyframePropTreeNode(ref.m_strengthKFP, "Strength", EntryIdx, PTX_KF_FLOAT2, "MIN", "MAX");

            break;
        }
        case DerivedType::ptxu_Collision:
        {
            ptxu_Collision* ptxuCollision = reinterpret_cast<ptxu_Collision*>(this);
            ptxu_Collision& ref = (*ptxuCollision);

            ptxKeyframePropTreeNode(ref.m_bouncinessKFP, "Bounce Factor", EntryIdx, PTX_KF_FLOAT2, "Percent Min", "Percent Max");
            ptxKeyframePropTreeNode(ref.m_bounceDirVarKFP, "Bounce Direction Variance", EntryIdx, PTX_KF_FLOAT4, "Rotation Min", "Rotation Max", "Elevation Min", "Elevation Max");

            ImGui::DragFloat(vfmt("Radius Mult##{}", NextUniqueNum()), &ref.m_radiusMult);
            ImGui::DragFloat(vfmt("Rest Speed##{}", NextUniqueNum()), &ref.m_restSpeed);

            ImGui::DragInt(vfmt("Collision Chance##{}", NextUniqueNum()), &ref.m_colnChance);
            ImGui::DragInt(vfmt("Kill Chance##{}", NextUniqueNum()), &ref.m_killChance);

            ImGui::Checkbox(vfmt("Debug Draw##{}", NextUniqueNum()), &ref.m_debugDraw);

            ImGui::DragFloat(vfmt("Override Min Radius##{}", NextUniqueNum()), &ref.m_overrideMinRadius);

            break;
        }
        case DerivedType::ptxu_Colour:
        {
            ptxu_Colour* ptxuColour = reinterpret_cast<ptxu_Colour*>(this);
            ptxu_Colour& ref = (*ptxuColour);

            ptxKeyframePropTreeNode(ref.m_rgbaMinKFP, "RGBA Min", EntryIdx, PTX_KF_COL4);
            ptxKeyframePropTreeNode(ref.m_rgbaMaxKFP, "RGBA Max", EntryIdx, PTX_KF_COL4);
            ptxKeyframePropTreeNode(ref.m_rgbaMaxKFP, "Emissive Intensity", EntryIdx, PTX_KF_FLOAT2, "MIN", "MAX");

            ImGui::InputInt(vfmt("Keyframe Mode##{}", NextUniqueNum()), &ref.m_keyframeMode);

            ImGui::Checkbox(vfmt("RGBA Max Enable##{}", NextUniqueNum()), &ref.m_rgbaMaxEnable);
            ImGui::Checkbox(vfmt("RGBA Proportional##{}", NextUniqueNum()), &ref.m_rgbaProportional);
            ImGui::Checkbox(vfmt("RGB Can Tint##{}", NextUniqueNum()), &ref.m_rgbCanTint);

            break;
        }
        case DerivedType::ptxu_MatrixWeight:
        {
            ptxu_MatrixWeight* ptxuMatrixWeight = reinterpret_cast<ptxu_MatrixWeight*>(this);
            ptxu_MatrixWeight& ref = (*ptxuMatrixWeight);

            ptxKeyframePropTreeNode(ref.m_mtxWeightKFP, "Matrix Weight", EntryIdx, PTX_KF_FLOAT2, "MIN", "MAX");
            ImGui::InputInt(vfmt("Reference Space##{}", NextUniqueNum()), &ref.m_referenceSpace);

            break;
        }
        case DerivedType::ptxu_Noise:
        {
            ptxu_Noise* ptxuNoise = reinterpret_cast<ptxu_Noise*>(this);
            auto& ref = (*ptxuNoise);

            ptxKeyframePropTreeNode(ref.m_posNoiseMinKFP, "Position Noise Min", EntryIdx, PTX_KF_FLOAT3, "Min X", "Min Y", "Min Z");
            ptxKeyframePropTreeNode(ref.m_posNoiseMaxKFP, "Position Noise Max", EntryIdx, PTX_KF_FLOAT3, "Max X", "Max Y", "Max Z");
            ptxKeyframePropTreeNode(ref.m_velNoiseMinKFP, "Velocity Noise Min", EntryIdx, PTX_KF_FLOAT3, "Min X", "Min Y", "Min Z");
            ptxKeyframePropTreeNode(ref.m_posNoiseMaxKFP, "Velocity Noise Max", EntryIdx, PTX_KF_FLOAT3, "Max X", "Max Y", "Max Z");

            ImGui::InputInt(vfmt("Reference Space##{}", NextUniqueNum()), &ref.m_referenceSpace);
            ImGui::Checkbox(vfmt("Keep Constant Speed##{}", NextUniqueNum()), &ref.m_keepConstantSpeed);

            break;
        }
        case DerivedType::ptxu_Pause:
        {
            ptxu_Pause* ptxuPause = reinterpret_cast<ptxu_Pause*>(this);
            ptxu_Pause& ref = (*ptxuPause);

            ImGui::DragFloat(vfmt("Pause Life Ratio##{}", NextUniqueNum()), &ref.m_pauseLifeRatio);
            ImGui::DragFloat(vfmt("Unpause Time##{}", NextUniqueNum()), &ref.m_unpauseTime);
            ImGui::DragFloat(vfmt("Unpause Effect Dist##{}", NextUniqueNum()), &ref.m_unpauseEffectDist);
            ImGui::DragFloat(vfmt("Unpause Cam Dist##{}", NextUniqueNum()), &ref.m_unpauseCamDist);

            break;
        }
        case DerivedType::ptxu_Rotation:
        {
            ptxu_Rotation* ptxuRotation = reinterpret_cast<ptxu_Rotation*>(this);
            ptxu_Rotation& ref = (*ptxuRotation);

            ptxKeyframePropTreeNode(ref.m_initialAngleMinKFP, "Initial Angle Min", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");
            ptxKeyframePropTreeNode(ref.m_initialAngleMaxKFP, "Initial Angle Max", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");

            ptxKeyframePropTreeNode(ref.m_angleMinKFP, "Angle Min", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");
            ptxKeyframePropTreeNode(ref.m_angleMaxKFP, "Angle Max", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");

            ImGui::InputInt(vfmt("Init Rotation Mode##{}", NextUniqueNum()), &ref.m_initRotationMode);
            ImGui::InputInt(vfmt("Update Rotation Mode##{}", NextUniqueNum()), &ref.m_updateRotationMode);

            ImGui::Checkbox(vfmt("Accumulate Angle##{}", NextUniqueNum()), &ref.m_accumulateAngle);
            ImGui::Checkbox(vfmt("Rotate Angle Axes##{}", NextUniqueNum()), &ref.m_rotateAngleAxes);
            ImGui::Checkbox(vfmt("Rotate Init Angle Axes##{}", NextUniqueNum()), &ref.m_rotateInitAngleAxes);

            ImGui::DragFloat(vfmt("Speed Fade Threshold##{}", NextUniqueNum()), &ref.m_speedFadeThresh);

            break;
        }
        case DerivedType::ptxu_Size:
        {
            ptxu_Size* ptxuSize = reinterpret_cast<ptxu_Size*>(this);
            ptxu_Size& ref = (*ptxuSize);

            ptxKeyframePropTreeNode(ref.m_whdMinKFP, "WHD Min", EntryIdx, PTX_KF_FLOAT3, "Width Min", "Height Min", "Depth Min");
            ptxKeyframePropTreeNode(ref.m_whdMaxKFP, "WHD Max", EntryIdx, PTX_KF_FLOAT3, "Width Max", "Height Max", "Depth Max");

            ptxKeyframePropTreeNode(ref.m_tblrScalarKFP, "TBLR Scalar", EntryIdx, PTX_KF_FLOAT4, "Top", "Bottom", "Left", "Right");
            ptxKeyframePropTreeNode(ref.m_tblrVelScalarKFP, "TBLR Velocity Scalar", EntryIdx, PTX_KF_FLOAT4, "Top", "Bottom", "Left", "Right");

            ImGui::InputInt(vfmt("Keyframe Mode##{}", NextUniqueNum()), &ref.m_keyframeMode);
            ImGui::Checkbox(vfmt("Is Proportional##{}", NextUniqueNum()), &ref.m_isProportional);

            break;
        }
        case DerivedType::ptxu_Velocity:
        {
            ptxu_Velocity* ptxuVelocity = reinterpret_cast<ptxu_Velocity*>(this);
            break;
        }
        case DerivedType::ptxu_Wind:
        {
            ptxu_Wind* ptxuWind = reinterpret_cast<ptxu_Wind*>(this);
            ptxu_Wind& ref = (*ptxuWind);

            ptxKeyframePropTreeNode(ref.m_influenceKFP, "Wind Influence", EntryIdx, PTX_KF_FLOAT2, "MIN", "MAX");

            ImGui::DragFloat(vfmt("High LOD Range##{}", NextUniqueNum()), &ref.m_highLodRange);
            ImGui::DragFloat(vfmt("Low LOD Range##{}", NextUniqueNum()), &ref.m_lowLodRange);

            ImGui::InputInt(vfmt("High LOD Disturbance Mode##{}", NextUniqueNum()), &ref.m_highLodDisturbanceMode);
            ImGui::InputInt(vfmt("Low LOD Disturbance Mode##{}", NextUniqueNum()), &ref.m_lowLodDisturbanceMode);
            ImGui::Checkbox(vfmt("Ignore Matrix Weight##{}", NextUniqueNum()), &ref.m_ignoreMtxWeight);

            break;
        }
        case DerivedType::ptxu_Decal:
        {
            ptxu_Decal* ptxuDecal = reinterpret_cast<ptxu_Decal*>(this);
            ptxu_Decal& ref = (*ptxuDecal);

            ptxKeyframePropTreeNode(ref.m_dimensionsKFP, "Dimensions", EntryIdx, PTX_KF_FLOAT4, "Width Min", "Width Max", "Height Min", "Height Max");
            ptxKeyframePropTreeNode(ref.m_alphaKFP, "Alpha (Not Used)", EntryIdx, PTX_KF_FLOAT2, "Alpha Min", "Alpha Max");

            ImGui::InputInt(vfmt("Decal Id##{}", NextUniqueNum()), &ref.m_decalId);
            ImGui::DragFloat(vfmt("Velocity Threshold##{}", NextUniqueNum()), &ref.m_velocityThresh);
            ImGui::DragFloat(vfmt("Total Life##{}", NextUniqueNum()), &ref.m_totalLife);
            ImGui::DragFloat(vfmt("Fade In Time##{}", NextUniqueNum()), &ref.m_fadeInTime);
            ImGui::DragFloat(vfmt("UV Mult Start##{}", NextUniqueNum()), &ref.m_uvMultStart);
            ImGui::DragFloat(vfmt("UV Mult End##{}", NextUniqueNum()), &ref.m_uvMultEnd);
            ImGui::DragFloat(vfmt("UV Mult Time##{}", NextUniqueNum()), &ref.m_uvMultTime);
            ImGui::DragFloat(vfmt("Duplicate Reject Dist##{}", NextUniqueNum()), &ref.m_duplicateRejectDist);

            ImGui::Checkbox(vfmt("Flip U##{}", NextUniqueNum()), &ref.m_flipU);
            ImGui::Checkbox(vfmt("Flip V##{}", NextUniqueNum()), &ref.m_flipV);
            ImGui::Checkbox(vfmt("Proportional Size##{}", NextUniqueNum()), &ref.m_proportionalSize);
            ImGui::Checkbox(vfmt("Use Complex Collision##{}", NextUniqueNum()), &ref.m_useComplexColn);

            ImGui::DragFloat(vfmt("Projection Depth##{}", NextUniqueNum()), &ref.m_projectionDepth);
            ImGui::DragFloat(vfmt("Distance Scale##{}", NextUniqueNum()), &ref.m_distanceScale);

            ImGui::Checkbox(vfmt("Is Directional##{}", NextUniqueNum()), &ref.m_isDirectional);

            break;
        }
        case DerivedType::ptxu_DecalPool:
        {
            ptxu_DecalPool* ptxuDecalPool = reinterpret_cast<ptxu_DecalPool*>(this);
            ptxu_DecalPool& ref = (*ptxuDecalPool);

            ImGui::DragFloat(vfmt("Velocity Threshold##{}", NextUniqueNum()), &ref.m_velocityThresh);
            ImGui::InputInt(vfmt("Liquid Type##{}", NextUniqueNum()), &ref.m_liquidType);
            ImGui::InputInt(vfmt("Decal Id##{}", NextUniqueNum()), &ref.m_decalId);
            ImGui::DragFloat(vfmt("Start Size##{}", NextUniqueNum()), &ref.m_startSize);
            ImGui::DragFloat(vfmt("End Size##{}", NextUniqueNum()), &ref.m_endSize);
            ImGui::DragFloat(vfmt("Growth Rate##{}", NextUniqueNum()), &ref.m_growthRate);

            break;
        }
        case DerivedType::ptxu_FogVolume:
        {
            ptxu_FogVolume* ptxuFogVolume = reinterpret_cast<ptxu_FogVolume*>(this);
            ptxu_FogVolume& ref = (*ptxuFogVolume);

            ptxKeyframePropTreeNode(ref.m_rgbTintMinKFP, "RGB Tint Min", EntryIdx, PTX_KF_COL3);
            ptxKeyframePropTreeNode(ref.m_rgbTintMaxKFP, "RGB Tint Max", EntryIdx, PTX_KF_COL3);

            ptxKeyframePropTreeNode(ref.m_densityRangeKFP, "Density/Range", EntryIdx, PTX_KF_FLOAT4, "Density Min", "Density Max", "Range Min", "Range Max");
            ptxKeyframePropTreeNode(ref.m_scaleMinKFP, "Scale Min", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");
            ptxKeyframePropTreeNode(ref.m_scaleMaxKFP, "Scale Max", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");

            ptxKeyframePropTreeNode(ref.m_rotationMinKFP, "Rotation Min", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");
            ptxKeyframePropTreeNode(ref.m_rotationMaxKFP, "Rotation Max", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");

            ImGui::DragFloat(vfmt("Falloff##{}", NextUniqueNum()), &ref.m_falloff);
            ImGui::DragFloat(vfmt("HDR Mult##{}", NextUniqueNum()), &ref.m_hdrMult);
            ImGui::InputInt(vfmt("Lighting Type##{}", NextUniqueNum()), &ref.m_lightingType);

            ImGui::Checkbox(vfmt("Colour Tint From Particle##{}", NextUniqueNum()), &ref.m_colourTintFromParticle);
            ImGui::Checkbox(vfmt("Sort With Particles##{}", NextUniqueNum()), &ref.m_sortWithParticles);
            ImGui::Checkbox(vfmt("Use Ground Fog Colour##{}", NextUniqueNum()), &ref.m_useGroundFogColour);
            ImGui::Checkbox(vfmt("Use Effect Evo Values##{}", NextUniqueNum()), &ref.m_useEffectEvoValues);

            break;
        }
        case DerivedType::ptxu_Light:
        {
            ptxu_Light* ptxuLight = reinterpret_cast<ptxu_Light*>(this);
            ptxu_Light& ref = (*ptxuLight);

            ptxKeyframePropTreeNode(ref.m_rgbMinKFP, "Light RGB Min", EntryIdx, PTX_KF_COL3);
            ptxKeyframePropTreeNode(ref.m_rgbMaxKFP, "Light RGB Max", EntryIdx, PTX_KF_COL3);

            ptxKeyframePropTreeNode(ref.m_intensityKFP, "Light Intensity", EntryIdx, PTX_KF_FLOAT4, "Intensity Min", "Intensity Max", "Falloff Min", "Falloff Max");
            ptxKeyframePropTreeNode(ref.m_rangeKFP, "Light Range", EntryIdx, PTX_KF_FLOAT4, "Range Min", "Range Max", "Angle Min", "Angle Max");

            ptxKeyframePropTreeNode(ref.m_coronaRgbMinKFP, "Corona RGB Min", EntryIdx, PTX_KF_COL3);
            ptxKeyframePropTreeNode(ref.m_coronaRgbMaxKFP, "Corona RGB Max", EntryIdx, PTX_KF_COL3);

            ptxKeyframePropTreeNode(ref.m_coronaIntensityKFP, "Corona Intensity", EntryIdx, PTX_KF_FLOAT2, "MIN", "MAX");
            ptxKeyframePropTreeNode(ref.m_coronaSizeKFP, "Corona Size", EntryIdx, PTX_KF_FLOAT2, "MIN", "MAX");
            ptxKeyframePropTreeNode(ref.m_coronaFlareKFP, "Corona Flare", EntryIdx, PTX_KF_FLOAT2, "MIN", "MAX");

            ImGui::DragFloat(vfmt("Corona Z Bias##{}", NextUniqueNum()), &ref.m_coronaZBias);
            ImGui::Checkbox(vfmt("Corona Use Light Colour##{}", NextUniqueNum()), &ref.m_coronaUseLightCol);

            ImGui::Checkbox(vfmt("Colour From Particle##{}", NextUniqueNum()), &ref.m_colourFromParticle);
            ImGui::Checkbox(vfmt("Colour Per Frame##{}", NextUniqueNum()), &ref.m_colourPerFrame);
            ImGui::Checkbox(vfmt("Intensity Per Frame##{}", NextUniqueNum()), &ref.m_intensityPerFrame);
            ImGui::Checkbox(vfmt("Range Per Frame##{}", NextUniqueNum()), &ref.m_rangePerFrame);
            ImGui::Checkbox(vfmt("Casts Shadows##{}", NextUniqueNum()), &ref.m_castsShadows);
            ImGui::Checkbox(vfmt("Corona Not In Reflection##{}", NextUniqueNum()), &ref.m_coronaNotInReflection);
            ImGui::Checkbox(vfmt("Corona Only In Reflection##{}", NextUniqueNum()), &ref.m_coronaOnlyInReflection);

            ImGui::InputInt(vfmt("Light Type##{}", NextUniqueNum()), &ref.m_lightType);

            break;
        }
        case DerivedType::ptxu_Liquid:
        {
            ptxu_Liquid* ptxuLiquid = reinterpret_cast<ptxu_Liquid*>(this);
            ptxu_Liquid& ref = (*ptxuLiquid);

            ImGui::DragFloat(vfmt("Velocity Threshold##{}", NextUniqueNum()), &ref.m_velocityThresh);
            ImGui::InputInt(vfmt("Liquid Type##{}", NextUniqueNum()), &ref.m_liquidType);
            ImGui::DragFloat(vfmt("Pool Start Size##{}", NextUniqueNum()), &ref.m_poolStartSize);
            ImGui::DragFloat(vfmt("Pool End Size##{}", NextUniqueNum()), &ref.m_poolEndSize);
            ImGui::DragFloat(vfmt("Pool Growth Rate##{}", NextUniqueNum()), &ref.m_poolGrowthRate);
            ImGui::DragFloat(vfmt("Trail Width Min##{}", NextUniqueNum()), &ref.m_trailWidthMin);
            ImGui::DragFloat(vfmt("Trail Width Max##{}", NextUniqueNum()), &ref.m_trailWidthMax);

            break;
        }
        case DerivedType::ptxu_River:
        {
            ptxu_River* ptxuRiver = reinterpret_cast<ptxu_River*>(this);
            ptxu_River& ref = (*ptxuRiver);

            ImGui::DragFloat(vfmt("Velocity Mult##{}", NextUniqueNum()), &ref.m_velocityMult);
            ImGui::DragFloat(vfmt("Influence##{}", NextUniqueNum()), &ref.m_influence);

            break;
        }
        case DerivedType::ptxu_ZCull:
        {
            ptxu_ZCull* ptxuZCull = reinterpret_cast<ptxu_ZCull*>(this);
            ptxu_ZCull& ref = (*ptxuZCull);

            ptxKeyframePropTreeNode(ref.m_heightKFP, "Height", EntryIdx, PTX_KF_FLOAT2, "MIN", "MAX");
            ptxKeyframePropTreeNode(ref.m_fadeDistKFP, "Fade Distance", EntryIdx, PTX_KF_FLOAT2, "MIN", "MAX");

            ImGui::InputInt(vfmt("Mode##{}", NextUniqueNum()), &ref.m_mode);
            ImGui::InputInt(vfmt("Reference Space##{}", NextUniqueNum()), &ref.m_referenceSpace);

            break;
        }
        case DerivedType::ptxu_Dampening:
        {
            ptxu_Dampening* ptxuDampening = reinterpret_cast<ptxu_Dampening*>(this);
            ptxu_Dampening& ref = (*ptxuDampening);

            ptxKeyframePropTreeNode(ref.m_xyzMinKFP, "XYZ Min", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");
            ptxKeyframePropTreeNode(ref.m_xyzMaxKFP, "XYZ Max", EntryIdx, PTX_KF_FLOAT3, "X", "Y", "Z");

            ImGui::InputInt(vfmt("Reference Space##{}", NextUniqueNum()), &ref.m_referenceSpace);
            ImGui::Checkbox(vfmt("Enable Air Resistance##{}", NextUniqueNum()), &ref.m_enableAirResistance);

            break;
        }
        default:
            ImGui::Text("Unknown Behaviour");
            break;

        }

        ImGui::TreePop();
    }
}