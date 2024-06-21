#include "helpers/align.h"
#include "Render.h"
#include "Shaders.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Functions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UploadBuffer(const ComPtr<ID3D11Buffer>& buffer, pConstVoid data, u32 dataSize)
{
	ID3D11DeviceContext* context = Renderer::GetContext();
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (FAILED(context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
	memcpy(mapped.pData, data, dataSize);
	context->Unmap(buffer.Get(), 0);
}


std::string WCharToString(const wchar_t* wstr)
{
	int sz = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	std::string str(sz, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], sz, NULL, NULL);
	return str;
}

const char* GetShaderCompileErrorString(HRESULT hr, ID3DBlob* shaderCompileErrorsBlob)
{
	const char* errorString = nullptr;

	if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		errorString = "Could not compile shader; file wasn't found";

	else if (shaderCompileErrorsBlob)
		errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();

	return errorString;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						ShaderBase
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ShaderBase::ShaderBase(const wchar_t* vsName, const wchar_t* psName)
	: m_VsName(vsName)
	, m_PsName(psName)
{ }


void ShaderBase::Create()
{
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;

	auto* p_Device = Renderer::GetDevice();
	{
		ComPtr<ID3DBlob> shaderCompileErrorsBlob;
		auto hResult = D3DCompileFromFile(m_VsName, nullptr, nullptr, "main", "vs_5_0", 0, 0, vsBlob.GetAddressOf(), shaderCompileErrorsBlob.GetAddressOf());

		if (FAILED(hResult)) {
			LogInfo("{}\n Shader Compiler Error", GetShaderCompileErrorString(hResult, shaderCompileErrorsBlob.Get()));
			return;
		}

		hResult = p_Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_VS.GetAddressOf());
		if (FAILED(hResult)) {
			LogInfo("CreateVertexShader err");
			return;
		}
	}
	
	{
		ComPtr<ID3DBlob> shaderCompileErrorsBlob;
		auto hResult = D3DCompileFromFile(m_PsName, nullptr, nullptr, "main", "ps_5_0", 0, 0, psBlob.GetAddressOf(), shaderCompileErrorsBlob.GetAddressOf());

		if (FAILED(hResult)) {
			LogInfo("{}\n Shader Compiler Error", GetShaderCompileErrorString(hResult, shaderCompileErrorsBlob.Get()));
			return;
		}

		hResult = p_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_PS.GetAddressOf());
		if (FAILED(hResult)) {
			LogInfo("CreatePixelShader err");
			return;
		}
	}

	{
		D3D11_INPUT_ELEMENT_DESC inputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		auto hResult = p_Device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_VSLayout.GetAddressOf());
		
		if (FAILED(hResult)) {
			LogInfo("CreateInputLayout err");
			return;
		}
	}
}


void ShaderBase::Bind()
{
	ID3D11DeviceContext* context = Renderer::GetContext();
	context->IASetInputLayout(m_VSLayout.Get());
	context->VSSetShader(m_VS.Get(), NULL, 0);
	context->PSSetShader(m_PS.Get(), NULL, 0);
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						DefaultShader
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DefaultShader::Create()
{
	ShaderBase::Create();
	//size_t size = ALIGN_16(sizeof(Locals));

	//D3D11_BUFFER_DESC desc;
	//desc.ByteWidth = size;
	//desc.Usage = D3D11_USAGE_DYNAMIC;
	//desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//desc.MiscFlags = 0;
	//desc.StructureByteStride = size;

	//Renderer::GetDevice()->CreateBuffer(&desc, NULL, &m_LocalsCB);
}

void DefaultShader::Bind()
{
	ShaderBase::Bind();

	//UploadBuffer(m_LocalsCB, &Locals, sizeof Locals);
	//ID3D11Buffer* cb = m_LocalsCB.Get();
	//Renderer::GetContext()->VSSetConstantBuffers(2, 1, &cb);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						ImageBlitShader
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImageBlitShader::Create()
{
	ShaderBase::Create();

	rage::Vector3 screenVerts[] =
	{
		{-1, -1, 0 }, // Bottom Left
		{ 1,  1, 0 }, // Top Right
		{-1,  1, 0 }, // Top Left
		{ 1, -1, 0 }, // Bottom Right
		{ 1,  1, 0 }, // Top Right
		{-1, -1, 0 }, // Bottom Left
	};

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof screenVerts;
	bufferDesc.StructureByteStride = sizeof rage::Vector3;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA bufferData;
	bufferData.SysMemPitch = 0;
	bufferData.SysMemSlicePitch = 0;
	bufferData.pSysMem = screenVerts;

	Renderer::GetDevice()->CreateBuffer(&bufferDesc, &bufferData, &m_ScreenVerts);
}


void ImageBlitShader::Blit(ID3D11ShaderResourceView* view) const
{
	ID3D11DeviceContext* context = Renderer::GetContext();

	// Map texture
	ID3D11ShaderResourceView* views = view;
	context->PSSetShaderResources(0, 1, &views);

	// Blit texture on screen
	auto* vb	 = m_ScreenVerts.Get();
	u32	  stride = sizeof rage::Vector3;
	u32   offset = 0;

	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->Draw(6, 0);
}
