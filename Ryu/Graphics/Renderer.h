#pragma once
#include "Asset/AssetRegistry.h"
#include "Asset/Primitives.h"
#include "Graphics/Camera.h"
#include "Graphics/Core/GfxBuffer.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxPipelineState.h"
#include "Graphics/Core/GfxRootSignature.h"
#include "Graphics/Core/GfxShader.h"
#include "Graphics/GpuResourceFactory.h"
#include "Graphics/Mesh.h"
#include "Graphics/RenderWorld.h"
#include "Graphics/SceneRenderer.h"
#include "Graphics/Shader/ShaderLibrary.h"

namespace Ryu::Utils { class FrameTimer; }

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

		[[nodiscard]] inline Asset::IGpuResourceFactory* GetGpuResourceFactory() const { return m_gpuFactory.get(); }
		[[nodiscard]] inline Asset::AssetRegistry* GetAssetRegistry() const { return m_assets.get(); }
		[[nodiscard]] inline ShaderLibrary* GetShaderLibrary() { return &m_shaderLibrary; }

		void Render();
		void RenderWorld(Game::World& world, const Utils::FrameTimer& frameTimer);
		void OnResize(u32 w, u32 h);

	private:
		void CreateResources();
		void CompileShaders();
		void CreatePipelineState();
		void CreateConstantBuffer();

	private:
		std::unique_ptr<Device>               m_device;
		std::unique_ptr<GpuResourceFactory>   m_gpuFactory;
		std::unique_ptr<Asset::AssetRegistry> m_assets;

		Gfx::RenderWorld                      m_renderWorld;
		ShaderLibrary                         m_shaderLibrary;
		SceneRenderer                         m_sceneRenderer;
		IRendererHook*                        m_hook;
		std::unique_ptr<RootSignature>        m_rootSignature;
		std::unique_ptr<PipelineState>        m_pipelineState;
		std::unique_ptr<DescriptorHeap>       m_cbvHeap;


		std::array<std::unique_ptr<Buffer>, static_cast<u64>(Asset::PrimitiveType::MAX_COUNT)> m_perObjectCBs;

		Camera                          m_camera;
		ConstantBuffer                  m_cbData{};
		Shader*                         m_vs;
		Shader*                         m_ps;
	};
}
