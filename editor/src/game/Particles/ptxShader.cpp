#include "ptxUiUniqueNums.h"
#include "ImGui/imgui.h"
#include "ptxKeyframe.h"
#include "helpers/helpers.h"
#include "ptxShader.h"


void rage::ptxShaderVar::UiWidgets(size_t entryIdx, size_t varIdx)
{
	size_t Packed_N = 0 | entryIdx << 32 | varIdx;

#define verify_ptr(ptr) ptr ? ptr : "(NULL)"

	const char* treeNodeName;
	if (m_pInfo){
		treeNodeName = verify_ptr(m_pInfo->m_Name);
	} else {
		treeNodeName = "Unknown";
	}

	if (ImGui::TreeNode(vfmt("Name: {} ## {}", treeNodeName, Packed_N)))
	{
		ImGui::Text("Is Keyframeable: %s", m_isKeyframeable ? "true" : "false");

		float step = 0.01, _min = 0, _max = 0;
		if (m_pInfo)
		{
			ImGui::Text("Info: %s", verify_ptr(m_pInfo->m_UiName));

			//ImGui::Text("UiWidget:             %s", verify_ptr(m_pInfo->m_UiWidget));
			//ImGui::Text("UiHelp:               %s", verify_ptr(m_pInfo->m_UiHelp));
			//ImGui::Text("UiHint:               %s", verify_ptr(m_pInfo->m_UiHint));
			//ImGui::Text("AssetName:            %s", verify_ptr(m_pInfo->m_AssetName));
			//ImGui::Text("GeoValueSource:       %s", verify_ptr(m_pInfo->m_GeoValueSource));
			//ImGui::Text("UiMin:                %f", m_pInfo->m_UiMin);
			//ImGui::Text("UiMax:                %f", m_pInfo->m_UiMax);
			//ImGui::Text("UiStep:               %f", m_pInfo->m_UiStep);
			//ImGui::Text("UiHidden:             %s", m_pInfo->m_UiHidden ? "true" : "false");
			//ImGui::Text("UvSetIndex:           %i", m_pInfo->m_UvSetIndex);
			//ImGui::Text("UvSetName:            %s", verify_ptr(m_pInfo->m_UvSetName));
			//ImGui::Text("TextureOutputFormats: %s", verify_ptr(m_pInfo->m_TextureOutputFormats));
			//ImGui::Text("NoOfMipLevels:        %i", m_pInfo->m_iNoOfMipLevels);
			//ImGui::Text("MaterialDataUvSetIndex: %i", m_pInfo->m_MaterialDataUvSetIndex);
			//ImGui::Text("ArrayCount:           %i", m_pInfo->m_ArrayCount);
			//ImGui::Text("IsMaterial:           %s", m_pInfo->m_IsMaterial ? "true" : "false");
			step = m_pInfo->m_UiStep / 5;
			_min = m_pInfo->m_UiMin;
			_max = m_pInfo->m_UiMax;
		}
		
		switch (m_type)
		{
		case rage::PTXSHADERVAR_FLOAT:
		{
			auto* ptr = reinterpret_cast<ptxShaderVarVector*>(this);
			ImGui::DragFloat(vfmt("FloatV##{}", NextUniqueNum()), (float*)&ptr->m_vector);

			break;
		}
		case rage::PTXSHADERVAR_FLOAT2:
		{
			auto* ptr = reinterpret_cast<ptxShaderVarVector*>(this);
			ImGui::DragFloat2(vfmt("Vector2##{}", NextUniqueNum()), (float*)&ptr->m_vector, step, _min, _max);
			break;
		}
		case rage::PTXSHADERVAR_FLOAT3:
		{
			auto* ptr = reinterpret_cast<ptxShaderVarVector*>(this);
			ImGui::DragFloat3(vfmt("Vector3##{}", NextUniqueNum()), (float*)&ptr->m_vector, step, _min, _max);
			break;
		}
		case rage::PTXSHADERVAR_FLOAT4:
		{
			auto* ptr = reinterpret_cast<ptxShaderVarVector*>(this);
			ImGui::DragFloat4(vfmt("Vector4##{}", NextUniqueNum()), (float*)&ptr->m_vector, step, _min, _max);
			break;
		}
		
		case rage::PTXSHADERVAR_TEXTURE:
		{
			auto* ptr = reinterpret_cast<ptxShaderVarTexture*>(this);
		
			auto* view = ptr->m_pTexture->GetTextureView();

			if (!view)
			{
				ImGui::Text("Texture : NULL");
				break;
			}

			if (strlen(ptr->m_textureName) > 0) {
				ImGui::Text("Texture : %s", ptr->m_textureName);
			} else {
				ImGui::Text("Texture : %s", "NULL");
			}

			ImVec2 TexSize(ptr->m_pTexture->GetWidth(), ptr->m_pTexture->GetHeight());
			ImVec2 UiTextureSize{};
			float fAspectRatio = TexSize.x / TexSize.y;
		
			if (TexSize.x > 512 || TexSize.y > 512) {
				UiTextureSize = ImVec2(512, 512 * fAspectRatio);
			} else {
				UiTextureSize = TexSize;
			}
			
			ImGui::Checkbox(vfmt("External Reference##{}", NextUniqueNum()), &ptr->m_externalReference);
			ImGui::Text("Size = %f x %f", TexSize.x, TexSize.y);
			ImGui::Image((void*)view, UiTextureSize);
			
			break;
		}
		case rage::PTXSHADERVAR_KEYFRAME:
		{
			auto* ptr = reinterpret_cast<ptxShaderVarKeyframe*>(this);

			ptxKeyframeTable(ptr->m_keyframe, Packed_N, PTX_KF_FLOAT4);

			break;
		}
		default:

			ImGui::Text("%s type Is not implemented", GetTypeStr());
			break;
		}

		ImGui::TreePop();
	}
	
	
	//ImGui::Text("Address: %p", this);
	//ImGui::SameLine();
	//if (ImGui::Button(vfmt("Copy to clipboard## {}", NextUniqueNum())))
	//{
	//	TextToClipboard(vfmt("{:016X}", u64(this)));
	//}

}

const char* rage::ptxShaderVar::GetTypeStr()
{
	static std::map<ptxShaderVarType, const char*> typesMap = {
			{ PTXSHADERVAR_BOOL, "bool" },
			{ PTXSHADERVAR_INT, "int" },
			{ PTXSHADERVAR_FLOAT, "float" },
			{ PTXSHADERVAR_FLOAT2, "float2" },
			{ PTXSHADERVAR_FLOAT3, "float3" },
			{ PTXSHADERVAR_FLOAT4, "float4" },
			{ PTXSHADERVAR_TEXTURE, "texture" },
			{ PTXSHADERVAR_KEYFRAME, "keyframe" }
	};

	auto it = typesMap.find(m_type);
	return it == typesMap.end() ? "unknown" : it->second;
}
