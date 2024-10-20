#include "DX12Texture.h"
#include "Graphics/DX12/DX12Core.h"
#include "Graphics/DX12/DX12Helpers.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12
{
	DX12Texture::~DX12Texture()
	{
		Release();
	}

	DX12Texture::DX12Texture(const DX12TextureCreateDesc& desc)
		: m_resource(nullptr)
	{
		auto* const device = Core::GetDevice();
		DEBUG_ASSERT(device, "Device is invalid");

		const D3D12_CLEAR_VALUE* clearValue = (desc.ResourceDesc
				&& desc.ResourceDesc->Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
				|| desc.ResourceDesc->Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) ? &desc.ClearValue : nullptr;

		if (desc.Resource)
		{
			// Create using existing resource
			DEBUG_ASSERT(!desc.Heap, "Resource and Heap cannot be specified at the same time");
			m_resource = desc.Resource;
		}
		else if(desc.Heap && desc.ResourceDesc)
		{
			// Create using existing heap
			DEBUG_ASSERT(!desc.Resource, "Resource and Heap cannot be specified at the same time");

			DXCall(device->CreatePlacedResource(
				desc.Heap,
				desc.AllocationInfo.Offset,
				desc.ResourceDesc,
				desc.InitialState,
				clearValue,
				IID_PPV_ARGS(&m_resource)));
		}
		else if (desc.ResourceDesc)
		{
			// Create a new resource
			DEBUG_ASSERT(!desc.Resource && !desc.Heap, "Resource and Heap cannot be specified at the same time");

			
			auto heapProps = Helpers::DefaultHeapProperties();
			DXCall(device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				desc.ResourceDesc,
				desc.InitialState,
				clearValue,
				IID_PPV_ARGS(&m_resource)));
		}

		DEBUG_ASSERT(m_resource, "Resource is invalid");
		m_srv  = Core::GetSRVDescHeap().Allocate();

		device->CreateShaderResourceView(m_resource, desc.SRVDesc, m_srv.CPU);
	}

	constexpr DX12Texture::DX12Texture(DX12Texture&& other) noexcept
		: m_resource(other.m_resource)
		, m_srv(other.m_srv)
	{
		other.Reset();
	}

	constexpr DX12Texture& DX12Texture::operator=(DX12Texture&& other) noexcept
	{
		DEBUG_ASSERT(this != &other);

		if (this != &other)
		{
			Release();
			Move(other);
		}

		return *this;
	}

	void DX12Texture::Release()
	{
		Core::GetSRVDescHeap().Free(m_srv);
		Core::DeferredRelease(m_resource);
	}

	constexpr void DX12Texture::Reset()
	{
		m_resource = nullptr;
		m_srv = {};
	}

	constexpr void DX12Texture::Move(DX12Texture& other)
	{
		m_resource = other.m_resource;
		m_srv = other.m_srv;
		other.Reset();
	}
}
