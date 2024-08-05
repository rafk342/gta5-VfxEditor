#include "stbi/stb_image.h"
#include "DirectXTK/DDSTextureLoader.h"

#include "texturepc.h"
#include "memory/address.h"


bool rage::grcTextureDX11::LockRect(int layer, int mipLevel, grcTextureLock& lock, int lockFlags)
{
	static auto fn = gmAddress::Scan("4C 89 4C 24 20 44 89 44 24 18 89 54 24 10 48 89 4C 24 08 48 83 EC 38 C6 44 24 28", "rage::grcTextureDX11::LockRect")
		.ToFunc<bool(rage::grcTexture*, int, int, grcTextureLock&, int)>();
	return fn(this, layer, mipLevel, lock, lockFlags);
}

void rage::grcTextureDX11::UnlockRect(grcTextureLock& lock)
{
	static auto fn = gmAddress::Scan("44 88 44 24 18 48 89 54 24 10 48 89 4C 24 08 56 57 48 83 EC 78 C6", "rage::grcTextureDX11::UnlockRect_Internal")
		.ToFunc<bool(rage::grcTexture*, grcTextureLock&, bool)>();
	fn(this, lock, false /* allowContextLocks */);
}

rage::grcTexture* rage::grcTextureFactory::CreateFromDDS(std::filesystem::path path, grcImage::Format img_format)
{
	if (!std::filesystem::exists(path) || path.empty() || !path.has_filename())
		return nullptr;

	if (path.filename().extension() != ".dds")
		return nullptr;

	grcTexture* texture = nullptr;
	ComPtr<ID3D11Resource> resource;
	auto hr = DirectX::CreateDDSTextureFromFile(DX11Device, path.c_str(), &resource, nullptr);

	if (FAILED(hr)) {
		LogInfo("Failed to create texture from DDS file: {}", path.string());
		return nullptr;
	}

	ComPtr<ID3D11Texture2D> pTexture;
	resource.As(&pTexture);

	D3D11_TEXTURE2D_DESC desc;
	pTexture->GetDesc(&desc);

	D3D11_TEXTURE2D_DESC stagingDesc = desc;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> StagingTexture;

	DX11Device->CreateTexture2D(&stagingDesc, nullptr, &StagingTexture);
	DX11Context->CopyResource(StagingTexture.Get(), pTexture.Get());
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	DX11Context->Map(StagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedRes);

	u8* data = (u8*)mappedRes.pData;

	rage::grcImage image;
	memset(&image, 0, sizeof(image));
	image.m_Width = desc.Width;
	image.m_Height = desc.Height;
	image.m_Depth = 1;
	image.m_Stride = desc.Width * 4;
	image.m_Format = img_format;
	image.m_Bits = data;

	texture = rage::grcTextureFactory::GetInstance()->Create(&image, nullptr);

	DX11Context->Unmap(StagingTexture.Get(), 0);
	return texture;
}

rage::grcTexture* rage::grcTextureFactory::CreateFromPath(std::filesystem::path path, grcImage::Format img_format)
{

	static const std::array extensions = { ".jpg", ".png", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic" };
	grcTexture* texture = nullptr;

	if (!std::filesystem::exists(path) || path.empty() || !path.has_filename())
		return nullptr;

	auto ext = path.extension();
	if (std::find_if(extensions.begin(), extensions.end(), [&ext](const char* _ext) { return ext == _ext; }) == extensions.end())
		return nullptr;

	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(path.string().c_str(), &image_width, &image_height, NULL, 4);
	if (image_data)
	{
		rage::grcImage image;
		memset(&image, 0, sizeof(image));
		image.m_Width = image_width;
		image.m_Height = image_height;
		image.m_Depth = 1;
		image.m_Stride = image_width * 4;
		image.m_Format = img_format;
		image.m_Bits = image_data;

		texture = rage::grcTextureFactory::GetInstance()->Create(&image, nullptr);

		stbi_image_free(image_data);
	}

	return texture;
}
