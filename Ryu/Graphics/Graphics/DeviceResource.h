#pragma once
#include "Common/ObjectMacros.h"
#include "Graphics/DX12.h"
#include <array>

namespace Ryu::Gfx
{
	class DeviceObject
	{
	public:
		explicit DeviceObject(Device* parent)
			: m_parent(parent)
		{}

		virtual ~DeviceObject() = default;

		inline NODISCARD Device* GetParent() const noexcept { return m_parent; }

	private:
		Device* m_parent;
	};

	constexpr D3D12_RESOURCE_STATES D3D12_RESOURCE_STATE_UNKNOWN = (D3D12_RESOURCE_STATES)-1;

	class ResourceState
	{
	public:
		ResourceState(D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_UNKNOWN)
			: m_allSameState(true)
		{
			m_resourceStates[0] = initialState;
		}

		void Set(D3D12_RESOURCE_STATES state, u32 subResource);
		D3D12_RESOURCE_STATES Get(u32 subResource) const;		

	private:
		std::array<D3D12_RESOURCE_STATES, D3D12_REQ_MIP_LEVELS> m_resourceStates;
		bool m_allSameState;
	};

	class DeviceResource : public DeviceObject
	{
	public:
		explicit DeviceResource(Device* parent, DX12::Resource* resource);
		virtual ~DeviceResource();

		void SetImmediateDelete(bool immediate);
		void SetName(const char* name) const;
		std::string GetName() const;
		bool UseStateTracking();
		DX12::Resource* GetResource() const;
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuHandle() const;
		void SetResourceState(D3D12_RESOURCE_STATES state, u32 subResource);
		D3D12_RESOURCE_STATES GetResourceState(u32 subResource = 0) const;

	protected:
		DX12::Resource*                m_resource = nullptr;
		std::unique_ptr<ResourceState> m_resourceState;
		u32						       m_ImmediateDelete : 1 = 0;
	};
}
