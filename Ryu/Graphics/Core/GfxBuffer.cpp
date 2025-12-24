#include "Graphics/Core/GfxBuffer.h"
#include "Graphics/Core/GfxDevice.h"

#include "Graphics/Core/GfxCommandList.h"
#include "Core/Common/Assert.h"

namespace Ryu::Gfx
{
	Buffer::Buffer(Device* parent, const Buffer::Desc& desc, DX12::Resource* uploadBuffer)
		: Resource(parent)
		, m_desc(desc)
		, m_destHandle()
	{
		RYU_ASSERT(desc.SizeInBytes > 0, "Buffer size must be greater than 0");

		if (uploadBuffer)
		{
			m_uploadBuffer.Attach(uploadBuffer);
		}

		CreateBuffer();
		CreateUploadBuffer();
	}

	Buffer::Buffer(Device* parent, const Buffer::Desc& desc, const DescriptorHandle& destHandle, DX12::Resource* uploadBuffer)
		: Buffer(parent, desc, uploadBuffer)
	{
		const D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = GetConstantBufferViewDesc();
		parent->GetNativeDevice()->CreateConstantBufferView(&cbvDesc, destHandle.CPU);
		
		m_destHandle = destHandle;
	}

	void Buffer::ReleaseObject()
	{
		Unmap();

		ComRelease(m_uploadBuffer);  // Ensure that this is called after copy has been executed
		m_gpuAddress = 0;

		Resource::ReleaseObject();
	}

	constexpr u32 Buffer::CalculateConstantBufferSize(u32 byteSize)
	{
		// Round up to the nearest multiple of 256.
		// Do this by adding 255 and then masking off the lower 2 bits.
		// Ref: https://github.com/d3dcoder/d3d12book/blob/4cfd00afa59210a272f62caf0660478d18b9ffed/Common/d3dUtil.h#L99
		return (byteSize + 255) & ~255;
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

		// Use stored format if available, otherwise fall back to parameter
		DXGI_FORMAT finalFormat = (m_desc.Format != DXGI_FORMAT_UNKNOWN) ? m_desc.Format : format;

		return D3D12_INDEX_BUFFER_VIEW
		{
			.BufferLocation = m_gpuAddress,
			.SizeInBytes    = static_cast<u32>(m_desc.SizeInBytes),
			.Format         = finalFormat
		};
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC Buffer::GetConstantBufferViewDesc() const
	{
		RYU_ASSERT(m_desc.Type == Buffer::Type::Constant, "Buffer is not a constant buffer");

		return D3D12_CONSTANT_BUFFER_VIEW_DESC
		{
			.BufferLocation = m_gpuAddress,
			.SizeInBytes    = m_desc.SizeInBytes
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

	void* Buffer::Map(const CD3DX12_RANGE& range)
	{
		RYU_ASSERT(m_desc.Usage != Buffer::Usage::Default, "Cannot map a Default heap buffer");

		if (!m_mappedData)
		{
			DXCall(m_resource->Map(0, &range, &m_mappedData));
		}

		return m_mappedData;
	}

	void Buffer::Unmap()
	{
		if (m_mappedData)
		{
			m_resource->Unmap(0, nullptr);
			m_mappedData = nullptr;
		}
	}

	void Buffer::CreateBuffer()
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		const CD3DX12_HEAP_PROPERTIES heapProps(static_cast<D3D12_HEAP_TYPE>(m_desc.Usage));
		
		// Align to 256 bytes for constant buffers
		if (m_desc.Type == Buffer::Type::Constant)
		{
			m_desc.SizeInBytes = CalculateConstantBufferSize(m_desc.SizeInBytes);
		}

		const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_desc.SizeInBytes);

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
		if (m_uploadBuffer)
		{
			return;  // We already have an upload buffer
		}

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

		const std::string name = m_desc.Name + " Upload Buffer";
		DX12::SetObjectName(m_uploadBuffer.Get(), name.c_str());

		if (m_desc.Usage == Buffer::Usage::Upload)
		{
			DXCall(m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData)));
		}
	}
}
