#pragma once
#include <d3d11.h>
#include "common/types.h"


using Microsoft::WRL::ComPtr;

const char* GetShaderCompileErrorString(HRESULT hr, ID3DBlob* shaderCompileErrorsBlob);

class ShaderBase
{
	const wchar_t*				m_VsName;
	const wchar_t*				m_PsName;
	ComPtr<ID3D11VertexShader>	m_VS;
	ComPtr<ID3D11PixelShader>	m_PS;
	ComPtr<ID3D11InputLayout>	m_VSLayout;

public:

	ShaderBase(const wchar_t* vsName, const wchar_t* psName);
	virtual ~ShaderBase() = default;

	virtual void Create();
	virtual void Bind();
};



class DefaultShader : public ShaderBase
{
	struct Locals
	{
		int	Unlit;
	};
	ComPtr<ID3D11Buffer> m_LocalsCB;
public:
#if 1
	DefaultShader() : ShaderBase(L"shaders1_vs.hlsl", L"shaders1_ps.hlsl") {}
#else
	DefaultShader() : ShaderBase(L"default_vs.hlsl", L"default_ps.hlsl") {}
#endif

	void Create() override;
	void Bind() override;

	Locals Locals = {};
};


class ImageBlitShader : public ShaderBase
{
	ComPtr<ID3D11Buffer> m_ScreenVerts;

public:
	ImageBlitShader() : ShaderBase(L"im_blit_vs.hlsl", L"im_blit_ps.hlsl") {}

	void Create() override;
	void Blit(ID3D11ShaderResourceView* view) const;
};
