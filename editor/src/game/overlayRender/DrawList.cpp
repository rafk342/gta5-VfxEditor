#include "overlayRender/Render.h"


#define SAFE_RELEASE(p) if (p) { p->Release(); p = nullptr; }
static std::recursive_mutex	s_Mtx;


enum MSAAModeEnum : u32
{
	MSAA_None = 0,
	MSAA_NonMaskAble = 1,
	MSAA_2 = 2,
	MSAA_4 = 4,
	MSAA_8 = 8,
};

inline void GetScreenSize(u32& width, u32& height)
{
	static gmAddress addr = gmAddress::Scan("44 8B 1D ?? ?? ?? ?? 48 8B 3C D0");
	height = *addr.GetRef(3).To<u32*>();
	width = *addr.GetRef(0xB + 2).To<u32*>();
}

inline MSAAModeEnum GetMSAA()
{
	static u32* pMode = gmAddress::Scan("E8 ?? ?? ?? ?? 8B 47 4C 89 05", "rage::grcDevice::SetSamplesAndFragments")
		.GetRef(1)
		.GetRef(0x2)
		.To<u32*>();

	return static_cast<MSAAModeEnum>(*pMode);
}

// DX11 state backup
struct OldState
{
	ID3D11BlendState*			BlendState;
	float						BlendFactor[4];
	UINT						BlendMask;
	ID3D11DepthStencilState*	DepthStencilState;
	UINT						StencilRef;
	UINT						NumViewports, NumScissors;
	D3D11_VIEWPORT				Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	D3D11_RECT					Scissors[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	ID3D11RasterizerState*		RasteriszerState;
	D3D11_PRIMITIVE_TOPOLOGY	PrimitiveTopology;
	ID3D11PixelShader*			PS;
	ID3D11VertexShader*			VS;
	UINT						PSInstancesCount, VSInstancesCount;
	ID3D11ClassInstance*		PSInstances[256];
	ID3D11ClassInstance*		VSInstances[256];
	ID3D11Buffer*				IndexBuffer;
	ID3D11Buffer*				VertexBuffer;
	ID3D11Buffer*				VSConstantBuffer;
	UINT						IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
	DXGI_FORMAT					IndexBufferFormat;
	ID3D11InputLayout*			InputLayout;
	ID3D11RenderTargetView*		RenderTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	ID3D11DepthStencilView*		DepthStencil;
};



void BackupOldState(OldState& oldState)
{
	memset(&oldState, 0, sizeof OldState);
	ID3D11DeviceContext* context = Renderer::GetContext();

	oldState.NumScissors = oldState.NumViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	context->OMGetBlendState(&oldState.BlendState, oldState.BlendFactor, &oldState.BlendMask);
	context->OMGetDepthStencilState(&oldState.DepthStencilState, &oldState.StencilRef);
	context->RSGetViewports(&oldState.NumViewports, oldState.Viewports);
	context->RSGetScissorRects(&oldState.NumScissors, oldState.Scissors);
	context->RSGetState(&oldState.RasteriszerState);
	context->IAGetPrimitiveTopology(&oldState.PrimitiveTopology);
	oldState.PSInstancesCount = oldState.VSInstancesCount = 256;
	context->PSGetShader(&oldState.PS, oldState.PSInstances, &oldState.PSInstancesCount);
	context->VSGetShader(&oldState.VS, oldState.VSInstances, &oldState.VSInstancesCount);
	context->VSGetConstantBuffers(1, 1, &oldState.VSConstantBuffer);
	context->IAGetInputLayout(&oldState.InputLayout);
	context->IAGetIndexBuffer(&oldState.IndexBuffer, &oldState.IndexBufferFormat, &oldState.IndexBufferOffset);
	context->IAGetVertexBuffers(0, 1, &oldState.VertexBuffer, &oldState.VertexBufferStride, &oldState.VertexBufferOffset);
	context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, oldState.RenderTargets, &oldState.DepthStencil);
}


void RestoreOldState(OldState& oldState)
{
	ID3D11DeviceContext* context = Renderer::GetContext();

	context->OMSetBlendState(oldState.BlendState, oldState.BlendFactor, oldState.BlendMask);
	context->OMSetDepthStencilState(oldState.DepthStencilState, oldState.StencilRef);
	context->RSSetViewports(oldState.NumViewports, oldState.Viewports);
	context->RSSetScissorRects(oldState.NumScissors, oldState.Scissors);
	context->RSSetState(oldState.RasteriszerState);
	context->IASetPrimitiveTopology(oldState.PrimitiveTopology);
	context->PSSetShader(oldState.PS, oldState.PSInstances, oldState.PSInstancesCount);
	context->VSSetShader(oldState.VS, oldState.VSInstances, oldState.VSInstancesCount);
	context->VSSetConstantBuffers(1, 1, &oldState.VSConstantBuffer);
	context->IASetInputLayout(oldState.InputLayout);
	context->IASetIndexBuffer(oldState.IndexBuffer, oldState.IndexBufferFormat, oldState.IndexBufferOffset);
	context->IASetVertexBuffers(0, 1, &oldState.VertexBuffer, &oldState.VertexBufferStride, &oldState.VertexBufferOffset);
	context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, oldState.RenderTargets, oldState.DepthStencil);
	SAFE_RELEASE(oldState.BlendState);
	SAFE_RELEASE(oldState.DepthStencilState);
	SAFE_RELEASE(oldState.RasteriszerState);
	SAFE_RELEASE(oldState.PS);
	SAFE_RELEASE(oldState.VS);
	for (UINT i = 0; i < oldState.PSInstancesCount; i++) SAFE_RELEASE(oldState.PSInstances[i]);
	for (UINT i = 0; i < oldState.VSInstancesCount; i++) SAFE_RELEASE(oldState.VSInstances[i]);
	SAFE_RELEASE(oldState.IndexBuffer);
	SAFE_RELEASE(oldState.VertexBuffer);
	SAFE_RELEASE(oldState.VSConstantBuffer);
	SAFE_RELEASE(oldState.InputLayout);
	for (ID3D11RenderTargetView*& rt : oldState.RenderTargets) SAFE_RELEASE(rt);
	SAFE_RELEASE(oldState.DepthStencil);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Vertex Buffer
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void DrawList::DrawData::VertexBuffer::Init(u32 capacity)
{
	size_t elementSize = sizeof(Vertex);

	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = elementSize * capacity;
	desc.StructureByteStride = elementSize;
	desc.MiscFlags = 0;

	Renderer::GetDevice()->CreateBuffer(&desc, NULL, Resource.GetAddressOf());
	Vertices = UPtr<Vertex[]>(new Vertex[capacity]);
	Capacity = capacity;
}


void DrawList::DrawData::VertexBuffer::Upload()
{
	auto* buffer = Resource.Get();
	void* data = Vertices.get();
	u32 dataSize = sizeof(Vertex) * Size;
	GPUSize = Size;
	
	if (!buffer) { LogInfo("VertexBuffer::Upload() buffer is nullptr"); return; }
	if (!data) { LogInfo("VertexBuffer::Upload() Vertices are nullptr"); return; }

	ID3D11DeviceContext* context = Renderer::GetContext();
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	
	if (FAILED(context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource))) {
		LogInfo("VertexBuffer::Upload() context->Map() failed"); 
		return;
	}
	memcpy(MappedResource.pData, data, dataSize);
	context->Unmap(buffer, 0);
}


void DrawList::DrawData::VertexBuffer::Bind() const
{
	auto*	buffer = Resource.Get();
	u32     stride = sizeof(Vertex);
	u32     offset = 0;
	Renderer::GetContext()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Draw List
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool DrawList::VerifyBufferCanFitLine() const
{
	bool can = m_Lines.VTXBuffer.Size + 2 <= m_Lines.VTXBuffer.Capacity;
	if (!can)
		LogInfo("DrawList {} -> Line buffer size must be extended", m_Name);
	return can;
}


DrawList::DrawList(const char* name, u32 maxLines)
{
	m_Name = name;
	m_Lines.TopologyType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	m_Lines.VTXBuffer.Init(maxLines);
}


void DrawList::DrawLine2D_Unsafe(const rage::Vec3V& p1, const rage::Vec3V& p2, Color32 col1, Color32 col2)
{
	if (col1.getAlphaU8() == 0 && col2.getAlphaU8() == 0)
		return;

	if (!VerifyBufferCanFitLine())
		return;

	m_Lines.VTXBuffer.Vertices[m_Lines.VTXBuffer.Size++] = Vertex({ p1, col1.Getf_col4() });
	m_Lines.VTXBuffer.Vertices[m_Lines.VTXBuffer.Size++] = Vertex({ p2, col2.Getf_col4() });
}

	
void DrawList::EndFrame()
{
	std::unique_lock lock(m_Mutex);
	
	m_HasLinesToDraw = m_Lines.VTXBuffer.Size > 0;
	if (!m_HasLinesToDraw)
		return;
	
	m_Lines.VTXBuffer.Upload();
	m_Lines.VTXBuffer.Size = 0; // Resets stats for the next frame
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Draw List Executor
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


DrawListExecutor::DrawListExecutor() 
{
	LogInfo("m_BaseShader.Create();");
	m_BaseShader.Create();
	//LogInfo("m_ImBlitShader.Create();");
	//m_ImBlitShader.Create();
	//LogInfo("im_gauss_ps");

	//ID3D11Device* device = Renderer::GetDevice();
	//ComPtr<ID3DBlob> psBlob;
	//{
	//	ComPtr<ID3DBlob> shaderCompileErrorsBlob;
	//	auto hResult = D3DCompileFromFile(L"im_gauss_ps.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, psBlob.GetAddressOf(), shaderCompileErrorsBlob.GetAddressOf());

	//	if (FAILED(hResult)) {
	//		LogInfo("{}\n Shader Compiler Error", GetShaderCompileErrorString(hResult, shaderCompileErrorsBlob.Get()));
	//		return;
	//	}
	//
	//	hResult = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_GaussPS.GetAddressOf());
	//	if (FAILED(hResult)) {
	//		LogInfo("CreatePixelShader err");
	//		return;
	//	}
	//}

	//// Depth Stencil State
	//{
	//	D3D11_DEPTH_STENCIL_DESC dsDesc;
	//	dsDesc.DepthEnable = true;
	//	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	//	dsDesc.StencilEnable = true;
	//	dsDesc.StencilReadMask = 0xFF;
	//	dsDesc.StencilWriteMask = 0xFF;
	//	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//	
	//	if (FAILED(device->CreateDepthStencilState(&dsDesc, &m_DSS)))
	//		LogInfo("CreateDepthStencilState err m_DSS");

	//	dsDesc.DepthEnable = false;
	//	if (FAILED(device->CreateDepthStencilState(&dsDesc, &m_DSSNoDepth)))
	//		LogInfo("CreateDepthStencilState err m_DSSNoDepth");

	//}

	//// Resterizer State
	//{
	//	D3D11_RASTERIZER_DESC rsDesc;
	//	rsDesc.CullMode = D3D11_CULL_BACK;
	//	rsDesc.FillMode = D3D11_FILL_SOLID;
	//	rsDesc.FrontCounterClockwise = true;
	//	rsDesc.DepthBias = 8000; // To prevent z figthing on collision if it matches geometry
	//	rsDesc.DepthBiasClamp = 0;
	//	rsDesc.SlopeScaledDepthBias = 0;
	//	rsDesc.DepthClipEnable = true;
	//	rsDesc.ScissorEnable = true;
	//	rsDesc.AntialiasedLineEnable = false;
	//	rsDesc.MultisampleEnable = true;
	//	// Default
	//	{
	//		if (FAILED(device->CreateRasterizerState(&rsDesc, &m_RS)))
	//			LogInfo("CreateRasterizerState err m_RS");
	//	}
	//	// Two-sided
	//	{
	//		rsDesc.CullMode = D3D11_CULL_NONE;
	//		if (FAILED(device->CreateRasterizerState(&rsDesc, &m_RSTwoSided)))
	//			LogInfo("CreateRasterizerState err m_RSTwoSided");
	//	}
	//	// Wireframe
	//	{
	//		rsDesc.CullMode = D3D11_CULL_BACK;
	//		rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	//		if (FAILED(device->CreateRasterizerState(&rsDesc, &m_RSWireframe)))
	//			LogInfo("CreateRasterizerState err m_RSWireframe");

	//	}
	//}

	//// Blend state
	//{
	//	D3D11_BLEND_DESC blendDesc = {};
	//	blendDesc.AlphaToCoverageEnable = false;
	//	blendDesc.RenderTarget[0].BlendEnable = true;
	//	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	//	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//	
	//	if (FAILED(device->CreateBlendState(&blendDesc, &m_BlendState)))
	//		LogInfo("CreateBlendState err");
	//}

	//LogInfo("DrawListExecutor::DrawListExecutor done");


}


void DrawListExecutor::RenderDrawData(const DrawList::DrawData& drawData) const
{
	ID3D11DeviceContext* context = Renderer::GetContext();

	context->IASetPrimitiveTopology(drawData.TopologyType);
	drawData.VTXBuffer.Bind();
	context->Draw(drawData.VTXBuffer.GPUSize, 0);
}


void DrawListExecutor::CreateBackbuf()
{
	ID3D11Device* device = Renderer::GetDevice();

	D3D11_TEXTURE2D_DESC texDesc = {};
	{
		texDesc.Width = m_ScreenWidth;
		texDesc.Height = m_ScreenHeight;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.SampleDesc.Count = m_SampleCount;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
	}

	if (FAILED(device->CreateTexture2D(&texDesc, nullptr, &m_BackbufMs)))
		LogInfo("::CreateBackbuf() CreateTexture2D err");

	if (m_SampleCount > 1)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
		rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		rtDesc.Texture2DMS.UnusedField_NothingToDefine = 0;
		
		if (FAILED(device->CreateRenderTargetView(m_BackbufMs.Get(), &rtDesc, &m_BackbufMsRt)))
			LogInfo("::CreateBackbuf() CreateRenderTargetView err");
	}
	else
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
		rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtDesc.Texture2D.MipSlice = 0;
		
		if (FAILED(device->CreateRenderTargetView(m_BackbufMs.Get(), &rtDesc, &m_BackbufMsRt)))
			LogInfo("::CreateBackbuf() CreateRenderTargetView err");
	}

	// Non multi sampled for resolving 
	texDesc.SampleDesc.Count = 1;
	if (FAILED(device->CreateTexture2D(&texDesc, nullptr, &m_Backbuf)))
		LogInfo("::CreateBackbuf() CreateTexture2D err Non multi sampled");


	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	{
		viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipLevels = 1;
		viewDesc.Texture2D.MostDetailedMip = 0;
	}
	if (FAILED(device->CreateShaderResourceView(m_Backbuf.Get(), &viewDesc, &m_BackbufView)))
		LogInfo("::CreateBackbuf() CreateShaderResourceView err");

}


void DrawListExecutor::Execute(DrawList& drawList)
{
	std::unique_lock lock(drawList.m_Mutex);

	if (!drawList.m_HasLinesToDraw)
		return;
	
	m_BaseShader.Locals.Unlit = true;
	m_BaseShader.Bind();
	RenderDrawData(drawList.m_Lines);


	//u32 width = -1, height = -1, sampleCount = -1;
	//GetScreenSize(width, height);
	//sampleCount = GetMSAA();

	//// Recreate MSAA back buffer and render target
	//if (m_ScreenWidth != width || m_ScreenHeight != height || m_SampleCount != sampleCount)
	//{
	//	m_ScreenWidth = width;
	//	m_ScreenHeight = height;
	//	m_SampleCount = sampleCount;
	//	
	//	if (m_SampleCount == 0)
	//		m_SampleCount = 1; // 0 is not valid for DX11

	//	LogInfo("DrawListExecutor -> Creating backbuf for {}x{} and {} samples", m_ScreenWidth, m_ScreenHeight, m_SampleCount);
	//	CreateBackbuf();
	//}

	//// Game will update matrices constant buffer for us
	//static auto grcWorldIdentity = gmAddress::Scan("E8 ?? ?? ?? ?? 4C 63 2D", "rage::grcWorldIdentity").GetCall().ToFunc<void()>();
	//grcWorldIdentity();

	//// Prepare state & Draw
	//OldState oldState;
	//BackupOldState(oldState);
	//{
	//	ID3D11DeviceContext* context = Renderer::GetContext();

	//	context->OMSetBlendState(m_BlendState.Get(), nullptr, 0xFFFFFFFF);

	//	// Viewport
	//	D3D11_VIEWPORT viewport = {};
	//	viewport.Width = static_cast<float>(width);
	//	viewport.Height = static_cast<float>(height);
	//	viewport.MaxDepth = 1.0;
	//	context->RSSetViewports(1, &viewport);

	//	// Scissors
	//	D3D11_RECT scissors = {};
	//	scissors.right = static_cast<LONG>(width);
	//	scissors.bottom = static_cast<LONG>(height);
	//	context->RSSetScissorRects(1, &scissors);

	//	// Obtain scene depth stencil
	//	static auto getTargetView = gmAddress::Scan("0F B7 41 7C 41", "rage::grcRenderTargetDX11::GetTargetView").ToFunc<ID3D11ShaderResourceView* (pVoid rt, u32 mip, u32 layer)>();
	//	static auto getDepthBuffer = gmAddress::Scan("E8 ?? ?? ?? ?? EB 20 83 3D", "CRenderTargets::GetDepthBuffer").GetCall().ToFunc<pVoid(bool unused)>();
	//	ID3D11DepthStencilView* depthBuffer = (ID3D11DepthStencilView*)getTargetView(getDepthBuffer(false), 0, 0);

	//	// We render to our MSAA back-buffer because buffer that's set in
	//	// CVisualEffects::RenderMarkers is not g buffer and don't have multisampling
	//	float clearColor[] = { 0, 0, 0, 0 };
	//	ID3D11RenderTargetView* backbufRt = m_BackbufMsRt.Get();
	//	context->OMSetRenderTargets(1, &backbufRt, depthBuffer);
	//	context->ClearRenderTargetView(backbufRt, clearColor);

	//	context->RSSetState(m_RS.Get());


	//	if (drawList.NoDepth)
	//		context->OMSetDepthStencilState(m_DSSNoDepth.Get(), 1);
	//	else
	//		context->OMSetDepthStencilState(m_DSS.Get(), 1);

	//	m_BaseShader.Locals.Unlit = false;
	//	m_BaseShader.Bind();
	//	RenderDrawData(drawList.m_Lines);
	//
	//	// Resolve multi sampled back buffer
	//	context->ResolveSubresource(
	//		m_Backbuf.Get(), D3D11CalcSubresource(0, 0, 1),
	//		m_BackbufMs.Get(), D3D11CalcSubresource(0, 0, 1),
	//		DXGI_FORMAT_R8G8B8A8_UNORM);

	//	// Set game's render target back and blit our back buffer to it
	//	context->RSSetState(m_RS.Get()); // Make sure we don't have wireframe rasterizer set...
	//	context->OMSetDepthStencilState(m_DSSNoDepth.Get(), 1); // We don't need depth
	//	context->OMSetRenderTargets(1, oldState.RenderTargets, nullptr);

	//	m_ImBlitShader.Bind();
	//	context->PSSetShader(m_GaussPS.Get(), nullptr, 0);
	//	m_ImBlitShader.Blit(m_BackbufView.Get());
	//}
	//RestoreOldState(oldState);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Game Draw Lists
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


GameDrawLists::Self_Type* GameDrawLists::self = nullptr;

void (*g_CVisualEffects_RenderMarkers)(u32);
void GameDrawLists::hk_CVisualEffects_RenderMarkers(u32 mask)
{
	g_CVisualEffects_RenderMarkers(mask);

	// rage::DLC_AddInternal<void(*)>(void(*), const char* name)>
	static auto dlc_AddInternal = gmAddress::Scan("81 20 04 20 F3 FF", "rage::DLC_AddInternal<void(*)(void)>+0x2D")
		.GetAt(-0x2D)
		.ToFunc<void(void(*)(), const char*)>();

	dlc_AddInternal([] { if (self) self->ExecuteAll(); }, "DrawListExecuteCommand1");
}


static gmAddress s_RenderMarkersAddr;
GameDrawLists::GameDrawLists()	
	: m_Overlay("Overlay", 0x1000)
{
	self = this;
	m_Overlay.NoDepth = true;

	s_RenderMarkersAddr = gmAddress::Scan("F6 C3 04 74 05", "CVisualEffects::RenderMarkers+0x2F")
		.GetAt(-0x2F);
	Hook::Create(s_RenderMarkersAddr, hk_CVisualEffects_RenderMarkers, &g_CVisualEffects_RenderMarkers, "RenderMarkers");
}


void GameDrawLists::EndFrame()
{
	m_Overlay.EndFrame();
}


void GameDrawLists::ExecuteAll()
{
	m_Executor.Execute(m_Overlay);
}


GameDrawLists::~GameDrawLists()
{
	Hook::Remove(s_RenderMarkersAddr, "RenderMarkers");
	self = nullptr;
}

GameDrawLists::Self_Type* GameDrawLists::Get() { return self; }
DrawList* GameDrawLists::GetOverlayList() { return &m_Overlay;}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void DrawLine2D(const rage::Vec3V& p1, const rage::Vec3V& p2, Color32 col1, Color32 col2)
{
	auto* dl = GameDrawLists::Get();
	if (!dl)
		return;
	std::unique_lock lock(s_Mtx);
	dl->GetOverlayList()->DrawLine2D_Unsafe(p1, p2, col1, col2);
}

