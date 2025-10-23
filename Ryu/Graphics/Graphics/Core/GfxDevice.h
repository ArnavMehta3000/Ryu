#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	struct GfxDeviceConfig
	{
		bool EnableDebugLayer = false;
		bool EnableValidation = true;
		bool UseWarpDevice = false;
		bool SetStablePowerState = false;
		D3D_FEATURE_LEVEL MinFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	};

	class GfxDevice
	{
	public:
		explicit GfxDevice(const GfxDeviceConfig& config);
		~GfxDevice();

		RYU_DISABLE_COPY(GfxDevice)

		[[nodiscard]] inline auto GetDevice() const noexcept { return m_device.Get(); }
		[[nodiscard]] inline auto GetFactory() const noexcept { return m_factory.Get(); }
		[[nodiscard]] inline D3D_FEATURE_LEVEL GetFeatureLevel() const noexcept { return m_featureLevel; }

		u32 GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

	private:
		void CreateFactory(u32 flags);
		void SelectAdapter();
		void CreateDevice();

	private:
		ComPtr<DXGI::Factory> m_factory;
		ComPtr<DXGI::Adapter> m_adapter;
		ComPtr<DX12::Device>  m_device;

		GfxDeviceConfig m_config;
		D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	};
}
