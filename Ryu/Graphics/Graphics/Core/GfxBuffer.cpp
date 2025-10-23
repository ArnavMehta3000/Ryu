#include "Graphics/Core/GfxBuffer.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/CommandContext.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	GfxBuffer::GfxBuffer(GfxDevice* device, const BufferDesc& desc)
		: m_device(device)
		, m_desc(desc)
	{
		RYU_ASSERT(m_device, "GfxDevice cannot be null");
		RYU_ASSERT(m_desc.SizeInBytes > 0, "Buffer size must be greater than 0");

		CreateBuffer();

		if (m_desc.DebugName)
		{
			DX12::SetObjectName(m_buffer.Get(), m_desc.DebugName);
		}
	}

	GfxBuffer::~GfxBuffer()
	{
		if (m_mappedData)
		{
			Unmap();
		}

		ComRelease(m_buffer);
	}

	D3D12_VERTEX_BUFFER_VIEW GfxBuffer::GetVertexBufferView() const
	{
		RYU_ASSERT(m_desc.Type == BufferType::Vertex, "Buffer is not a vertex buffer");

		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = GetGPUAddress();
		view.SizeInBytes    = static_cast<u32>(m_desc.SizeInBytes);
		view.StrideInBytes  = m_desc.StrideInBytes;

		return view;
	}

	D3D12_INDEX_BUFFER_VIEW GfxBuffer::GetIndexBufferView(DXGI_FORMAT format) const
	{
		RYU_ASSERT(m_desc.Type == BufferType::Index, "Buffer is not an index buffer");

		D3D12_INDEX_BUFFER_VIEW view{};
		view.BufferLocation = GetGPUAddress();
		view.SizeInBytes    = static_cast<u32>(m_desc.SizeInBytes);
		view.Format         = format;
		
		return view;
	}

	void GfxBuffer::Upload(const void* data, u64 sizeInBytes, u64 offset)
	{
		RYU_ASSERT(data, "Data pointer cannot be null");
		RYU_ASSERT(offset + sizeInBytes <= m_desc.SizeInBytes, "Upload size exceeds buffer size");
		RYU_ASSERT(m_desc.HeapType == D3D12_HEAP_TYPE_UPLOAD, "Buffer must be on upload heap for direct uploads");

		void* mappedPtr = Map();
		std::memcpy(static_cast<byte*>(mappedPtr) + offset, data, sizeInBytes);
		Unmap();
	}

	void GfxBuffer::UploadWithContext(CommandContext* context, const void* data, u64 sizeInBytes)
	{
		RYU_ASSERT(context, "CommandContext cannot be null");
		RYU_ASSERT(data, "Data pointer cannot be null");
		RYU_ASSERT(sizeInBytes <= m_desc.SizeInBytes, "Upload size exceeds buffer size");

		// For default heap buffers, we need an intermediate upload buffer
		if (m_desc.HeapType == D3D12_HEAP_TYPE_DEFAULT)
		{
			// Create temporary upload buffer
			BufferDesc uploadDesc = m_desc;
			uploadDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
			uploadDesc.InitialState = D3D12_RESOURCE_STATE_GENERIC_READ;
			uploadDesc.DebugName = "Temp Upload Buffer";

			GfxBuffer uploadBuffer(m_device, uploadDesc);
			uploadBuffer.Upload(data, sizeInBytes);

			// Copy from upload to default buffer
			context->GetCommandList()->CopyBufferRegion(
				m_buffer.Get(),
				0,
				uploadBuffer.GetResource(),
				0,
				sizeInBytes);
		}
		else
		{
			// Direct upload for upload heap
			Upload(data, sizeInBytes);
		}
	}

	void* GfxBuffer::Map()
	{
		if (!m_mappedData)
		{
			CD3DX12_RANGE readRange(0, 0); // We don't intend to read from this resource on the CPU
			DXCall(m_buffer->Map(0, &readRange, &m_mappedData));
		}
		return m_mappedData;
	}

	void GfxBuffer::Unmap()
	{
		if (m_mappedData)
		{
			m_buffer->Unmap(0, nullptr);
			m_mappedData = nullptr;
		}
	}

	void GfxBuffer::CreateBuffer()
	{
		CD3DX12_HEAP_PROPERTIES heapProps(m_desc.HeapType);
		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_desc.SizeInBytes);

		DXCall(m_device->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			m_desc.InitialState,
			nullptr,
			IID_PPV_ARGS(&m_buffer)
		));

		// If initial data is provided, upload it
		if (m_desc.InitialData && m_desc.HeapType == D3D12_HEAP_TYPE_UPLOAD)
		{
			Upload(m_desc.InitialData, m_desc.SizeInBytes);
		}
	}
}
