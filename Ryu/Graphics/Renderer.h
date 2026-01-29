#pragma once
#include "Asset/AssetRegistry.h"
#include "Asset/Primitives.h"
#include "Graphics/Camera.h"
#include "Graphics/GpuResourceFactory.h"
#include "Graphics/Mesh.h"
#include "Graphics/WorldRenderer.h"
#include "Graphics/Shader/ShaderLibrary.h"

namespace Ryu::Utils { class FrameTimer; }
namespace Ryu::Game { class World; }

namespace Ryu::Gfx
{
	class IRendererHook;

	class Renderer
	{
	public:
		Renderer(HWND window, IRendererHook* hook = nullptr);
		~Renderer();

		[[nodiscard]] inline Asset::IGpuResourceFactory* GetGpuResourceFactory() { return &m_gpuFactory; }
		[[nodiscard]] inline Asset::AssetRegistry* GetAssetRegistry() { return &m_assets; }
		[[nodiscard]] inline ShaderLibrary* GetShaderLibrary() { return &m_shaderLibrary; }
		[[nodiscard]] inline WorldRenderer* GetWorldRenderer() { return &m_worldRenderer; }

		void RenderWorld(Game::World& world, const Utils::FrameTimer& frameTimer);
		void OnResize(u32 w, u32 h);
	
	private:
		std::unique_ptr<Device> m_device;
		GpuResourceFactory      m_gpuFactory;
		Asset::AssetRegistry    m_assets;
		ShaderLibrary           m_shaderLibrary;
		WorldRenderer           m_worldRenderer;
		IRendererHook*          m_hook;
	};
}
