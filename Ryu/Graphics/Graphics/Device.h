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

		struct LiveObjectReporter
		{
			~LiveObjectReporter();
		};

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

		inline NODISCARD ComPtr<DX12::Device> GetDevice() const { return m_device; }
		inline NODISCARD ComPtr<DXGI::Factory> GetFactory() const { return m_factory; }
		inline NODISCARD std::shared_ptr<Fence> GetFrameFence() const { return m_frameFence; }

		void IdleGPU();
		void DeferReleaseObject(DX12::Object* object);

	private:
		LiveObjectReporter m_reporter;

		ComPtr<DXGI::Factory> m_factory;
		ComPtr<DX12::Device> m_device;

		std::shared_ptr<Fence> m_frameFence;
		std::array<u64, GraphicsConfig::FRAME_COUNT> m_frameFenceValues;
		u32 m_frameIndex = 0;
		DeferredDeleteQueue m_deleteQueue;
	};
}
