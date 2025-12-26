#pragma once
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxRootSignature.h"
#include "Graphics/Core/GfxPipelineState.h"
#include "Graphics/Core/GfxShader.h"
#include "Graphics/Core/GfxBuffer.h"
#include "Graphics/Shader/ShaderLibrary.h"
#include "Graphics/Camera.h"
#include "Graphics/Mesh.h"
#include "Graphics/GpuResourceFactory.h"
#include "Asset/AssetRegistry.h"

namespace Ryu::Gfx
{
	class IRendererHook;

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
		void CreatePrimitiveTypes();
		void CreateResources();
		void CompileShaders();
		void CreatePipelineState();
		void CreateConstantBuffer();

	private:
		ShaderLibrary                   m_shaderLibrary;
		IRendererHook*                  m_hook;
		std::unique_ptr<Device>         m_device;
		std::unique_ptr<RootSignature>  m_rootSignature;
		std::unique_ptr<PipelineState>  m_pipelineState;
		std::unique_ptr<Buffer>         m_constantBuffer;
		std::unique_ptr<DescriptorHeap> m_cbvHeap;

		std::unique_ptr<GpuResourceFactory> m_gpuFactory;
		std::unique_ptr<Asset::AssetRegistry> m_assets;

		Camera                          m_camera;
		ConstantBuffer                  m_cbData{};
		Shader*                         m_vs;
		Shader*                         m_ps;
	};
}
