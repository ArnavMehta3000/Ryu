#pragma once
#include "Graphics/RenderData.h"
#include "Graphics/Core/GfxRootSignature.h"
#include "Graphics/Core/GfxPipelineState.h"
#include "Graphics/Core/GfxDescriptorHeap.h"
#include "Graphics/ConstantBufferPool.h"

namespace Ryu::Asset { class AssetRegistry; class IGpuResourceFactory; }

namespace Ryu::Gfx
{
	class Device;
	class ShaderLibrary;

	class WorldRenderer
	{
		RYU_DISABLE_COPY(WorldRenderer)
	public:
		struct Config
		{
			u32 MaxRenderItems     = 10000;
			u32 MaxConstantBuffers = 10000;
			bool EnableWireframe   = false;
		};

	public:
		WorldRenderer() = default;
		WorldRenderer(Device* device, ShaderLibrary* shaderLib, Asset::AssetRegistry* registry, const Config& config = {});
		~WorldRenderer() = default;

		void OnResize(u32 width, u32 height);
		[[nodiscard]] inline u32 GetScreenWidth() const { return m_screenWidth; }
		[[nodiscard]] inline u32 GetScreenHeight() const { return m_screenHeight; }

		void SetDefaultCamera(const CameraData& camera);
		[[nodiscard]] inline const CameraData& GetDefaultCamera() const { return m_defaultCamera; }

		void RenderFrame(const Gfx::RenderFrame& frame, Asset::IGpuResourceFactory* gpuFactory);
		void RenderView(const Gfx::RenderView& view);

		void SetConfig(const Config& config);
		[[nodiscard]] const Config& GetConfig() const { return m_config; }

	private:
		void CreateResources();
		void CreatePipelineState();
		void InitializeDefaultCamera();
		void UpdateDefaultCameraProjection();

		void BeginFrame();
		void EndFrame();

		void RenderOpaquePass(const Gfx::RenderView& view);
		void RenderTransparentPass(const Gfx::RenderView& view);

		void BindPerFrameData(const CameraData& camera, f32 deltaTime, f32 totalTime);
		void DrawRenderItem(const RenderItem& item, const CameraData& camera);

	private:
		Device*                         m_device        = nullptr;
		ShaderLibrary*                  m_shaderLib     = nullptr;
		Asset::AssetRegistry*           m_assetRegistry = nullptr;
		Config                          m_config{};

		std::unique_ptr<RootSignature>  m_rootSignature;
		std::unique_ptr<PipelineState>  m_opaquePipeline;
		std::unique_ptr<PipelineState>  m_transparentPipeline;
		std::unique_ptr<DescriptorHeap> m_cbvHeap;
		ConstantBufferPool              m_cbPool;

		CameraData                      m_defaultCamera{};
		u32                             m_screenWidth   = 1920;
		u32                             m_screenHeight  = 1080;
		u64                             m_currentFrame  = 0;
	};
}
