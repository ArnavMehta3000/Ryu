#pragma once
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxRootSignature.h"
#include "Graphics/Core/GfxPipelineState.h"
#include "Graphics/Core/GfxShader.h"
#include "Graphics/Core/GfxBuffer.h"

namespace Ryu::Gfx
{
	class RendererNew
	{
	public:
		RendererNew(HWND window);
		~RendererNew() {}

		void Render();
		void OnResize(u32 w, u32 h);

	private:
		void CreateResources();

	private:
		std::unique_ptr<Device> m_device;
		std::unique_ptr<RootSignature> m_rootSignature;
		std::unique_ptr<PipelineState> m_pipelineState;
		std::unique_ptr<Buffer> m_vertexBuffer;
		Shader m_vs;
		Shader m_ps;
	};
}
