#pragma once
#include <iostream>
#include <map>
#include <directxmath.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <d3d11.h>
#include <wrl.h>
#include <mutex>

#include "Shaders.h"
#include "memory/address.h"
#include "memory/hook.h"
#include "common/types.h"
#include "rage/math/vecv.h"
#include "rage/atl/color32.h"


using Microsoft::WRL::ComPtr;

class DrawList
{
	friend class DrawListExecutor;

	template<class T>
	using UPtr = std::unique_ptr<T>;

#pragma pack(push, 1)
	struct Vertex
	{
		rage::Vector3 Pos;
		rage::Vector4 Color;
		rage::Vector3 Normal;
	};
#pragma pack(pop)

	struct DrawData
	{
		struct VertexBuffer
		{
			ComPtr<ID3D11Buffer>	Resource;
			UPtr<Vertex[]>			Vertices;
			u32						Capacity = 0;
			u32						Size = 0;
			u32						GPUSize = 0;
			
			void Init(u32 capacity);
			void Upload();
			void Bind() const;
		};

		VertexBuffer             VTXBuffer;
		D3D11_PRIMITIVE_TOPOLOGY TopologyType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	};
	
	const char*				m_Name;
	DrawData				m_Lines;
	bool					m_HasLinesToDraw = false;
	std::recursive_mutex	m_Mutex;

	bool VerifyBufferCanFitLine() const;
	void DrawLine2D_Unsafe(const rage::Vec3V& p1, const rage::Vec3V& p2, Color32 col1, Color32 col2);
	friend void DrawLine2D(const rage::Vec3V& p1, const rage::Vec3V& p2, Color32 col1, Color32 col2);

public:

	bool NoDepth = true;
	//bool Unlit = false;
	//bool Wireframe = false;
	//bool BackfaceCull = true;

	DrawList(const char* name, u32 maxLines);
	void EndFrame();
};


class DrawListExecutor
{
	u32									m_ScreenWidth = 0, m_ScreenHeight = 0, m_SampleCount = 0;
	ComPtr<ID3D11Texture2D>				m_BackbufMs;
	ComPtr<ID3D11RenderTargetView>		m_BackbufMsRt;
	ComPtr<ID3D11Texture2D>				m_Backbuf;
	ComPtr<ID3D11ShaderResourceView>	m_BackbufView;
	ComPtr<ID3D11BlendState>			m_BlendState;
	ComPtr<ID3D11DepthStencilState>		m_DSS;
	ComPtr<ID3D11DepthStencilState>		m_DSSNoDepth;
	ComPtr<ID3D11RasterizerState>		m_RSTwoSided;
	ComPtr<ID3D11RasterizerState>		m_RSWireframe;
	ComPtr<ID3D11RasterizerState>		m_RS;

	DefaultShader						m_BaseShader;
	//ImageBlitShader						m_ImBlitShader;
	//ComPtr<ID3D11PixelShader>			m_GaussPS;

	void RenderDrawData(const DrawList::DrawData& drawData) const;
	void CreateBackbuf();

public:
	DrawListExecutor();
	void Execute(DrawList& drawList);
};



class GameDrawLists
{
	using Self_Type = GameDrawLists;
	
	static Self_Type*	self;
	DrawListExecutor	m_Executor;
	DrawList			m_Overlay;

	static void hk_CVisualEffects_RenderMarkers(u32 mask);

public:
	
	static Self_Type* Get();

	GameDrawLists();
	~GameDrawLists();
	void EndFrame();
	void ExecuteAll();	// Called from the game's draw command
	DrawList* GetOverlayList();
};


void DrawLine2D(const rage::Vec3V& p1, const rage::Vec3V& p2, Color32 col1, Color32 col2);

