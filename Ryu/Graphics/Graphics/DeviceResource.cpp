#include "Graphics/DeviceResource.h"
#include <libassert/assert.hpp>

namespace Ryu::Gfx
{
	void ResourceState::Set(D3D12_RESOURCE_STATES state, u32 subResource)
	{
		if (subResource != D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			D3D12_RESOURCE_STATES current_state = m_resourceStates[0];
			DEBUG_ASSERT(subResource < m_resourceStates.size());
			if (m_allSameState)
			{
				for (D3D12_RESOURCE_STATES& s : m_resourceStates)
					s = current_state;
				m_allSameState = false;
			}
			m_resourceStates[subResource] = state;
		}
		else
		{
			m_allSameState = true;
			m_resourceStates[0] = state;
		}
	}
	
	D3D12_RESOURCE_STATES ResourceState::Get(u32 subResource) const
	{
		DEBUG_ASSERT(m_allSameState || subResource < (u32)m_resourceStates.size());
		if (m_allSameState)
			return m_resourceStates[0];
		return m_resourceStates[subResource];
	}
	
	DeviceResource::DeviceResource(Device* parent, DX12::Resource* resource)
		:DeviceObject(parent), m_resource(resource)
	{
	}
	
	DeviceResource::~DeviceResource()
	{
		if (m_resource)
		{
			if (m_ImmediateDelete)
			{
				m_resource->Release();
			}
			else
			{
				GetParent()->DeferReleaseObject(m_resource);
			}
			m_resource = nullptr;
		}
	}
	
	void DeviceResource::SetImmediateDelete(bool immediate)
	{
		m_ImmediateDelete = immediate;
	}
	
	void DeviceResource::SetName(const char* name) const
	{
		DX12::SetObjectName(m_resource, name);
	}
	
	std::string DeviceResource::GetName() const
	{
		return DX12::GetObjectName(m_resource);
	}
	
	bool DeviceResource::UseStateTracking()
	{
		return m_resourceState != nullptr;
	}
	
	DX12::Resource* DeviceResource::GetResource() const
	{
		return m_resource;
	}
	
	D3D12_GPU_VIRTUAL_ADDRESS DeviceResource::GetGpuHandle() const
	{
		return m_resource->GetGPUVirtualAddress();
	}
	
	void DeviceResource::SetResourceState(D3D12_RESOURCE_STATES state, u32 subResource)
	{
		m_resourceState->Set(state, subResource);
	}
	
	D3D12_RESOURCE_STATES DeviceResource::GetResourceState(u32 subResource) const
	{
		return m_resourceState->Get(subResource);
	}
}
