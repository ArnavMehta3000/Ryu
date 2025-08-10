#include "Graphics/Resources/Texture.h"
#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Core/Device.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(Texture);

	Texture::Texture(DeviceWeakPtr parent, const TextureInitInfo& info, DescriptorHeap& srvHeap)
		: DeviceObject(parent)
		, m_srvHeap(&srvHeap)
	{
		OnConstruct(info, srvHeap);
	}

	Texture::~Texture()
	{
		OnDestruct();

		m_srvHeap = nullptr;
	}

	void Texture::OnConstruct(const TextureInitInfo& info, DescriptorHeap& srvHeap)
	{
		if (!m_srvHeap)
		{
			m_srvHeap = &srvHeap;
		}

		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();

			const CD3DX12_CLEAR_VALUE* const clearValue = (info.ResourceDesc
				&& (info.ResourceDesc->Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
				|| info.ResourceDesc->Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)) ? &info.ClearValue : nullptr;

			if (info.Resource.has_value())
			{
				m_resource.Attach(info.Resource.value());
			}
			else if (info.Heap.has_value() && info.ResourceDesc)
			{
				RYU_ASSERT(!info.Resource, "Trying to create placed resource, but a resource was specified");

				DXCallEx(device->CreatePlacedResource(
					info.Heap.value(),
					info.AllocationInfo.Offset,
					info.ResourceDesc,
					info.InitialState,
					clearValue,
					IID_PPV_ARGS(&m_resource)),
				device);
			}
			else
			{
				RYU_ASSERT(!info.Resource && !info.Heap, "Trying to create committed resource, but a resource & heap was specified!");

				CD3DX12_HEAP_PROPERTIES props(DX12::ToNative(HeapType::Default), 0, 0);

				DXCallEx(device->CreateCommittedResource(
					&props,
					D3D12_HEAP_FLAG_NONE,
					info.ResourceDesc,
					info.InitialState,
					clearValue,
					IID_PPV_ARGS(&m_resource)),
				device);
			}

			RYU_ASSERT(m_resource);
			m_srv = m_srvHeap->Allocate();

			device->CreateShaderResourceView(m_resource.Get(), &info.SRVDesc, m_srv.CPU);
		}
	}

	void Texture::OnDestruct()
	{
		// Heap should be valid during destruction
		RYU_ASSERT(m_srvHeap);

		if (m_srvHeap)
		{
			m_srvHeap->Free(m_srv);
		}

		if (auto parent = GetParent())
		{
			parent->DeferredRelease(m_resource.Detach());
		}
	}
}
