#pragma once

#include <d3d11.h>
#include "common/types.h"

namespace rage
{
	struct grcVertexDeclaration
	{
		u32 ElementCount;
		u32 RefCount;
		u32 Stride;

		D3D11_INPUT_ELEMENT_DESC Elements[1];
	};
}
