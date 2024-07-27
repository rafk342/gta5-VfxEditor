#include "gBloodFxReloader.h"


bool gBloodfxReloader::invalid_path = false;
bool gBloodfxReloader::bloodfx_loaded = true;
std::filesystem::path gBloodfxReloader::new_path;

bool gBloodfxReloader::IsLoaded() { return (!invalid_path && bloodfx_loaded); }


void* (*g_asset_open_f)(u64, char*, char*, bool, bool);
void* gBloodfxReloader::n_asset_open_f(u64 arg1, char* fname, char* ext, bool arg4, bool arg5)
{
	void* result = nullptr;
	result = g_asset_open_f(arg1, const_cast<char*>(new_path.string().c_str()), const_cast<char*>(new_path.extension().string().substr(1).c_str()), arg4, arg5);

	if (!result) {
		bloodfx_loaded = false;
	} else {
		bloodfx_loaded = true;
	}
	return result;
}


void gBloodfxReloader::reload_bloodfx(char* path)
{
	new_path = path;

	if (!new_path.has_filename() || new_path.empty() || !std::filesystem::exists(new_path) || !std::filesystem::is_regular_file(new_path)) {
		invalid_path = true;
		return;
	} else {
		invalid_path = false;
	}

	if (!new_path.has_extension())
		new_path.replace_extension(".dat");

	static auto asset_open_addr = gmAddress::Scan("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 7C 24 ?? 41 56 48 81 EC 30 01 00 00");
	static auto blood_init = gmAddress::Scan("40 57 48 83 EC 20 4C 8B C1 83 FA 01").ToFunc<void(void*, int)>();
	static void* bloodfx_ptr = gmAddress::Scan("48 8D 0D ?? ?? ?? ?? 48 8B D3 F3 0F 10 00").GetRef(3).To<void*>();

	Hook::Create(asset_open_addr, n_asset_open_f, &g_asset_open_f, "open asset func");
	blood_init(bloodfx_ptr, 4);
	Hook::Remove(asset_open_addr);
}


gBloodfxUi::gBloodfxUi(const char* title) : App(title)
{
	memset(buff, 0, std::size(buff));
	strcpy_s(buff, Preload::Get()->getConfigParser()->GetString("Settings", "Bloodfx_reload_path", "E:\\bloodfx.dat").c_str());
}


void gBloodfxUi::window()
{
	ImGui::Text("Note : Import/export buttons do not work for this App");
	ImGui::Separator();
	ImGui::InputText("Path (Including filename)", buff, std::size(buff));

	if (ImGui::Button("Reload"))
		reload_bloodfx(buff);

	if (!IsLoaded())
		ImGui::Text("Failed to load bloodfx from the given path");
}

