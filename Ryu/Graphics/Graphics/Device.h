#pragma once
#include "Graphics/DeviceResource.h"
#include "Graphics/GraphicsConfig.h"
#include "Graphics/Fence.h"
#include <queue>

namespace Ryu::Gfx
{
	class Device : public DeviceObject
	{
		RYU_LOG_DECLARE_CATEGORY(GraphicsDevice);

		struct LiveObjectReporter { ~LiveObjectReporter(); };

		class DeferredDeleteQueue : public DeviceObject
		{
		private:
			struct FencedObject
			{
				Fence* Fence;
				u64 FenceValue;
				DX12::Object* Resource;
			};

		public:
			DeferredDeleteQueue(Device* parent) : DeviceObject(parent) {}
			~DeferredDeleteQueue();

			void Enqueue(DX12::Object* resource, Fence* fence);
			void Clean();

		private:
			std::mutex m_queueMutex;
			std::queue<FencedObject> m_deletionQueue;
		};

	public:
		Device();
		~Device();

		inline NODISCARD CmdQueue* GetCmdQueue(D3D12_COMMAND_LIST_TYPE type) const noexcept { return m_cmdQueues.at(type).Get(); }
		inline NODISCARD DX12::Device* GetDevice() const noexcept { return m_device.Get(); }
		inline NODISCARD DXGI::Factory* GetFactory() const noexcept { return m_factory.Get(); }
		inline NODISCARD Fence* const GetFrameFence() const noexcept { return m_frameFence.Get(); }

		void IdleGPU();
		void DeferReleaseObject(DX12::Object* object);

	private:
		void GetHardwareAdapter(IDXGIFactory7* pFactory, IDXGIAdapter4** ppAdapter);

	private:
		LiveObjectReporter                                                      m_reporter;
		ComPtr<DXGI::Factory>                                                   m_factory;
		ComPtr<DX12::Device>                                                    m_device;
		Memory::Ref<Fence>                                                      m_frameFence;
		std::array<Memory::Ref<CmdQueue>, D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE> m_cmdQueues;
		std::array<u64, GraphicsConfig::FRAME_COUNT>                            m_frameFenceValues;
		u32                                                                     m_frameIndex = 0;
		DeferredDeleteQueue                                                     m_deleteQueue;
	};
}
