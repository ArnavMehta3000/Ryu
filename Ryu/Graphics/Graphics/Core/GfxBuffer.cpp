#include "Graphics/Core/GfxBuffer.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxCommandList.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	Buffer::Buffer(Device* parent, const Buffer::Desc& desc)
		: Resource(parent)
		, m_desc(desc)
	{
		RYU_ASSERT(desc.SizeInBytes > 0, "Buffer size must be greater than 0");
		CreateBuffer();
		CreateUploadBuffer();
	}


	void Buffer::ReleaseObject()
	{
		//if (m_mappedData)
		{
			//Unmap();
		}

		ComRelease(m_uploadBuffer);  // Ensure that this is called after copy has been executed
		m_gpuAddress = 0;

		Resource::ReleaseObject();
	}

	[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW Buffer::GetVertexBufferView() const
	{
		RYU_ASSERT(m_desc.Type == Buffer::Type::Vertex, "Buffer is not a vertex buffer");
		RYU_ASSERT(m_desc.StrideInBytes > 0, "Vertex buffer stride must be greater than 0");

		return D3D12_VERTEX_BUFFER_VIEW
		{
			.BufferLocation = m_gpuAddress,
			.SizeInBytes    = static_cast<u32>(m_desc.SizeInBytes),
			.StrideInBytes  = static_cast<u32>(m_desc.StrideInBytes)
		};
	}

	D3D12_INDEX_BUFFER_VIEW Buffer::GetIndexBufferView(DXGI_FORMAT format) const
	{
		RYU_ASSERT(m_desc.Type == Buffer::Type::Index, "Buffer is not an index buffer");

		return D3D12_INDEX_BUFFER_VIEW
		{
			.BufferLocation = m_gpuAddress,
			.SizeInBytes    = static_cast<u32>(m_desc.SizeInBytes),
			.Format         = format
		};
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC Buffer::GetConstantBufferViewDesc() const
	{
		RYU_ASSERT(m_desc.Type == Buffer::Type::Constant, "Buffer is not a constant buffer");

		return D3D12_CONSTANT_BUFFER_VIEW_DESC
		{
			.BufferLocation = m_gpuAddress,
			.SizeInBytes    = static_cast<u32>((m_desc.SizeInBytes + 255) & ~255)  // Must be 256-byte aligned
		};
	}
	
	void Buffer::UploadData(const CommandList& cmdList, const void* data)
	{
		// Transition to copy destination
		cmdList.TransitionResource(m_resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

		D3D12_SUBRESOURCE_DATA subResourceData
		{
			.pData      = data,
			.RowPitch   = static_cast<LONG_PTR>(m_desc.SizeInBytes),
			.SlicePitch = static_cast<LONG_PTR>(m_desc.SizeInBytes)
		};

		// From D3DX12 Resource helpers
		::UpdateSubresources<1>(
			cmdList,
			m_resource.Get(), m_uploadBuffer.Get(),
			0, // Offset
			0, // First subresource
			1, // Number of subresources
			&subResourceData);

		cmdList.TransitionResource(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
		m_needsUpload = false;
	}

	void Buffer::CreateBuffer()
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		const CD3DX12_HEAP_PROPERTIES heapProps(static_cast<D3D12_HEAP_TYPE>(m_desc.Type));
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_desc.SizeInBytes);

		DXCall(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&m_resource)
		));

		m_gpuAddress = m_resource->GetGPUVirtualAddress();
	}
	
	void Buffer::CreateUploadBuffer()
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_desc.SizeInBytes);

		DXCall(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_uploadBuffer)
		));
	}
}
