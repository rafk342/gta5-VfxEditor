#pragma once


namespace rage
{
	struct ptxRenderState
	{
		int m_cullMode;
		int m_blendSet;
		int m_lightingMode;
		bool m_depthWrite;
		bool m_depthTest;
		bool m_alphaBlend;
		char pad[5];
	};
}