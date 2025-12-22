#pragma once
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxRootSignature.h"
#include "Graphics/Core/GfxPipelineState.h"
#include "Graphics/Core/GfxShader.h"
#include "Graphics/Core/GfxBuffer.h"
#include "Graphics/Shader/ShaderLibrary.h"
#include "Graphics/Camera.h"

namespace Ryu::Gfx
{
	class IRendererHook;
	class ShaderLibrary;

	struct ConstantBuffer
	{
		Math::Matrix WVP;
	};

	class Renderer
	{
	public:
		Renderer(HWND window, IRendererHook* hook = nullptr);
		~Renderer();

		void Render();
		void OnResize(u32 w, u32 h);

	private:
		void CreateResources();
		void CreateRootSignature();
		void CompileShaders();
		void CreatePipelineState();
		void CreateConstantBuffer();
		void CreateMeshBuffers();

	private:
		ShaderLibrary                   m_shaderLibrary;
		IRendererHook*                  m_hook;
		std::unique_ptr<Device>         m_device;
		std::unique_ptr<RootSignature>  m_rootSignature;
		std::unique_ptr<PipelineState>  m_pipelineState;
		std::unique_ptr<Buffer>         m_vertexBuffer;
		std::unique_ptr<Buffer>         m_indexBuffer;
		std::unique_ptr<Buffer>         m_constantBuffer;
		std::unique_ptr<DescriptorHeap> m_cbvHeap;

		Camera                          m_camera;
		ConstantBuffer                  m_cbData{};
		Shader*                         m_vs;
		Shader*                         m_ps;
	};
}
