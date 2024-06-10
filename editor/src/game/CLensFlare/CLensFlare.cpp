#include "CLensFlare.h"

void UpdateVertexBuffer(ID3D11DeviceContext* context, ID3D11Buffer* vertexBuffer, const XMFLOAT3& start, const XMFLOAT3& end)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (SUCCEEDED(hr)) 
	{
		float* vertices = reinterpret_cast<float*>(mappedResource.pData);

		vertices[0] = start.x;
		vertices[1] = start.y;
		
		vertices[2] = 0.01f;// 0.f, 1.f, 0.f, 1.0f,
		vertices[3] = 0.0f;
		vertices[4] = 1.0f;
		vertices[5] = 1.0f;
		
		vertices[6] = end.x;
		vertices[7] = end.y;

		vertices[8] = 1.0f;		//1.f, 0.f, 0.f, 1.0f
		vertices[9] = 0.0f;
		vertices[10] = 0.0f;
		vertices[11] = 1.0f;

		context->Unmap(vertexBuffer, 0);
	}
}


const char* GetShaderCompileErrorString(HRESULT hr, ID3DBlob* shaderCompileErrorsBlob)
{
	const char* errorString = nullptr;

	if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		errorString = "Could not compile shader; file not found";

	else if (shaderCompileErrorsBlob)
		errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();

	return errorString;
}


void LensFlareHandler::EndFrame()
{

	if (self && sm_IsFlareFxRenderedOnThisFrame)
	{
		auto* p_DeviceContext = Renderer::GetContext();

		UpdateVertexBuffer(	p_DeviceContext,
							self->m_VertexBuffer.Get(),
							{ (sm_SunScreenPos.X() * 2.0f - 1.0f ), -(sm_SunScreenPos.Y() * 2.0f - 1.0f), 0.0f },
							{ (sm_result.X() * 2.0f - 1.0f), -(sm_result.Y() * 2.0f - 1.0f), 0.0f });


		p_DeviceContext->IASetVertexBuffers(0, 1, self->m_VertexBuffer.GetAddressOf(), &self->stride, &self->offset);
		p_DeviceContext->IASetInputLayout(self->m_InputLayout.Get());
		p_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


		p_DeviceContext->VSSetShader(self->m_vertexShader.Get(), nullptr, 0);
		p_DeviceContext->PSSetShader(self->m_pixelShader.Get(), nullptr, 0);

		p_DeviceContext->Draw(2, 0);
		
		self->PrepareForTheNextFrame();
	}


}



rage::ScalarV LengthVec2(rage::Vec3V V)
{
	return std::sqrt(V.X() * V.X() + V.Y() * V.Y());
}

rage::ScalarV LengthVec3(rage::Vec3V V)
{
	return std::sqrt(V.X() * V.X() + V.Y() * V.Y() + V.Z() * V.Z());
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



LensFlareHandler* LensFlareHandler::self = nullptr;

namespace
{
	gmAddress gRenderFlareFxAddr;
}

bool LensFlareHandler::sm_IsFlareFxRenderedOnThisFrame = false;
bool LensFlareHandler::sm_DrawingFlares = false;
rage::Vec3V LensFlareHandler::sm_SunScreenPos = { 0.0f, 0.0f, 0.0f };
rage::Vec3V LensFlareHandler::sm_result = { 0.0f, 0.0f, 0.0f };



void(*RenderFlareFxfn)(u64, u64, u64, u64, float*);
void LensFlareHandler::n_RenderFlareFx(u64 arg1, u64 arg2, u64 arg3, u64 arg4, float* SunPos)
{
	using namespace rage;

	RenderFlareFxfn(arg1, arg2, arg3, arg4, SunPos);
	

	sm_IsFlareFxRenderedOnThisFrame = true;
	sm_DrawingFlares = true;

	sm_SunScreenPos = { SunPos[0], SunPos[1] };


	ScalarV fDistScale{};

	Vec3V vFlareDirNorm = Vec3V(0.5f, 0.5f) - sm_SunScreenPos;
	ScalarV fLen = LengthVec2(vFlareDirNorm);

	Vec3V vDist = vFlareDirNorm.Abs() + Vec3V(0.5f, 0.5f);
	vFlareDirNorm /= fLen;

	ScalarV fDistToEdge = LengthVec2(vDist);

	Vec3V vFlareDir = vFlareDirNorm * fDistToEdge;
	sm_result = sm_SunScreenPos + vFlareDir;



	sm_DrawingFlares = false;
}


LensFlareHandler::LensFlareHandler() 
{
	self = this;

	m_CLensFlare = gmAddress::Scan("48 8D 0D ?? ?? ?? ?? 75 ?? 33 D2").GetRef(3).To<gCLensFlare*>();
	gRenderFlareFxAddr = gmAddress::Scan("48 8B C4 F3 0F 11 58 ?? 4C 89 40 ?? 55");
	
	Hook::Create(gRenderFlareFxAddr, n_RenderFlareFx, &RenderFlareFxfn, "RenderFlareFx");
	
	
	auto* p_Device = Renderer::GetDevice();
	
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;

	{
		ComPtr<ID3DBlob> shaderCompileErrorsBlob;
		auto hResult = D3DCompileFromFile(L"shaders1.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, vsBlob.GetAddressOf(), shaderCompileErrorsBlob.GetAddressOf());

		if (FAILED(hResult))
			mlogger("{} Shader Compiler Error", GetShaderCompileErrorString(hResult, shaderCompileErrorsBlob.Get()));

		if (FAILED(p_Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf())))
			mlogger("CreateVertexShader err");
	}

	{
		ComPtr<ID3DBlob> shaderCompileErrorsBlob;
		auto hResult = D3DCompileFromFile(L"shaders1.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, psBlob.GetAddressOf(), shaderCompileErrorsBlob.GetAddressOf());
		
		if (FAILED(hResult))
			mlogger("{} Shader Compiler Error", GetShaderCompileErrorString(hResult, shaderCompileErrorsBlob.Get()));

		if (FAILED(p_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf())))
			mlogger("CreatePixelShader err");
	}


	{
		D3D11_INPUT_ELEMENT_DESC LayoutDesc[] =
		{
			{ "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		if (FAILED(p_Device->CreateInputLayout(	LayoutDesc, ARRAYSIZE(LayoutDesc), vsBlob->GetBufferPointer(),vsBlob->GetBufferSize(), m_InputLayout.GetAddressOf())))
			mlogger("CreateInputLayout err");	
	}


	stride = 6 * sizeof(float);
	offset = 0;

	D3D11_BUFFER_DESC vBufferDesc{};


	vBufferDesc.ByteWidth = sizeof(VertexData);
	vBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = VertexData;


	if (FAILED(p_Device->CreateBuffer(&vBufferDesc, &initData, m_VertexBuffer.GetAddressOf())))
		mlogger("CreateBuffer err");

}


LensFlareHandler::~LensFlareHandler()
{
	Hook::Remove(gRenderFlareFxAddr, "RenderFlareFx");
	self = nullptr;
}



void LensFlareHandler::Update()
{
	if (!sm_IsFlareFxRenderedOnThisFrame)
		return;


}


void LensFlareHandler::PrepareForTheNextFrame()
{
	sm_IsFlareFxRenderedOnThisFrame = false;
}


const char* LensFlareHandler::GetFileNameAtIndex(size_t idx) 
{
	static std::map<LensFlareFile_e, const char*> FileNamesMap
	{
		{ lensflare_m, "M" },
		{ lensflare_f, "F" },
		{ lensflare_t, "T" },
	};

	auto fName_e = static_cast<LensFlareFile_e>(idx);
	if (!FileNamesMap.contains(fName_e)) {
		return "fNone";
	} else {
		return FileNamesMap.at(fName_e);
	}
}

const char* LensFlareHandler::GetTextureTypeName(u8 NumTexture)
{
	static std::map<FlareFxTextureType_e, const char*> NamesMap
	{
		{ AnimorphicType,"AnimorphicFx" },
		{ ArtefactType,	 "ArtefactFx" },
		{ ChromaticType, "ChromaticFx" },
		{ CoronaType,	 "CoronaFx"	},
	};
	
	auto name_e = static_cast<FlareFxTextureType_e>(NumTexture);
	if (!NamesMap.contains(name_e)) {
		return "Unknown";
	} else {
		return NamesMap.at(name_e);
	}
}


CFlareFX::CFlareFX(FlareFxTextureType_e type)
{
	switch (type)
	{
	case AnimorphicType:

		m_fDistFromLight = 0.2f;
		m_fSize = 0.2f;
		m_fWidthRotate = 0.0f;
		m_fScrollSpeed = 0.0f;
		m_fCurrentScroll = 0.0f;
		m_fDistanceInnerOffset = 0.0f;
		m_fIntensityScale = 100.f;
		m_fIntensityFade = 0.0f;
		m_fAnimorphicScaleFactorU = 0.0f;
		m_fAnimorphicScaleFactorV = 0.0f;
		m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_fTextureColorDesaturate = 0.0f;
		m_fGradientMultiplier = 0.0f;
		m_nTexture = 0;
		m_nSubGroup = 0;
		m_bAnimorphicBehavesLikeArtefact = false;
		m_bAlignRotationToSun = false;
		m_fPositionOffsetU = 0.0f;

		break;
	case ArtefactType:


		break;
	case ChromaticType:


		break;
	case CoronaType:


		break;
	default:
		break;
	}
}
