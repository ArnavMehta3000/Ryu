#pragma once
#include "Graphics/Core/DeviceObject.h"
#include "Graphics/Core/DescriptorHeap.h"

namespace Ryu::Gfx
{
	class Device : public std::enable_shared_from_this<Device>
	{
	public:
		static DevicePtr Create();
		static void Destroy(Device& device);

		inline NODISCARD DX12::Device* const GetDevice() const noexcept { return m_device.Get(); }
		inline NODISCARD DXGI::Factory* const GetFactory() const noexcept { return m_factory.Get(); }
		inline NODISCARD const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }

	private:
		void Initialize();
		void CreateDevice();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	private:
		CD3DX12FeatureSupport        m_featureSupport;		
		ComPtr<DXGI::Factory>        m_factory;
		ComPtr<DX12::Device>         m_device;		
	};
}
