#include "Graphics/Core/GfxBuffer.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxCommandList.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	Buffer::Buffer(Device* parent, const Buffer::Desc& desc, const void* initialData)
		: DeviceChild(parent)
		, m_desc(desc)
	{
		RYU_ASSERT(desc.SizeInBytes > 0, "Buffer size must be greater than 0");

		CreateBuffer(initialData);

		if (initialData)
		{
			UploadInitialData(initialData);
		}
	}
	
	Buffer::Buffer(Device* parent, const Buffer::Desc& desc)
		: DeviceChild(parent)
		, m_desc(desc)
	{
		RYU_ASSERT(desc.SizeInBytes > 0, "Buffer size must be greater than 0");
		CreateBuffer(nullptr);
	}


	inline void Buffer::ReleaseObject()
	{
		if (m_mappedData)
		{
			Unmap();
		}

		ComRelease(m_uploadBuffer);
		ComRelease(m_resource);
		m_gpuAddress = 0;
	}

	[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW Buffer::GetVertexBufferView() const
	{
		RYU_ASSERT(m_desc.Type == Buffer::Type::Vertex, "Buffer is not a vertex buffer");
		RYU_ASSERT(m_desc.StrideInBytes > 0, "Vertex buffer stride must be greater than 0");

		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = m_gpuAddress;
		view.SizeInBytes    = static_cast<u32>(m_desc.SizeInBytes);
		view.StrideInBytes  = static_cast<u32>(m_desc.StrideInBytes);
		return view;
	}

	D3D12_INDEX_BUFFER_VIEW Buffer::GetIndexBufferView(DXGI_FORMAT format) const
	{
		RYU_ASSERT(m_desc.Type == Buffer::Type::Index, "Buffer is not an index buffer");

		D3D12_INDEX_BUFFER_VIEW view{};
		view.BufferLocation = m_gpuAddress;
		view.SizeInBytes    = static_cast<u32>(m_desc.SizeInBytes);
		view.Format         = format;
		return view;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC Buffer::GetConstantBufferViewDesc() const
	{
		RYU_ASSERT(m_desc.Type == Buffer::Type::Constant, "Buffer is not a constant buffer");

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
		desc.BufferLocation = m_gpuAddress;
		desc.SizeInBytes    = static_cast<UINT>((m_desc.SizeInBytes + 255) & ~255);  // Must be 256-byte aligned
		return desc;
	}
	
	void* Buffer::Map()
	{
		RYU_ASSERT(m_desc.Usage != Buffer::Usage::Default, "Cannot map a Default heap buffer");

		if (!m_mappedData)
		{
			const CD3DX12_RANGE readRange(0, 0);  // We don't intend to read from this resource on the CPU
			DXCall(m_resource->Map(0, &readRange, &m_mappedData));
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
	
	void Buffer::UpdateData(const void* data, u64 sizeInBytes, u64 offset)
	{
		RYU_ASSERT(m_desc.Usage == Buffer::Usage::Upload, "UpdateData can only be used with Upload heap buffers");
		RYU_ASSERT(data, "Data cannot be null");
		RYU_ASSERT(offset + sizeInBytes <= m_desc.SizeInBytes, "Update would exceed buffer size");

		if (!m_mappedData)
		{
			Map();
		}

		std::memcpy(static_cast<u8*>(m_mappedData) + offset, data, sizeInBytes);
	}
	
	void Buffer::UpdateDataDeferred(CommandList& cmdList, const void* data, u64 sizeInBytes, u64 offset)
	{
		RYU_ASSERT(m_desc.Usage == Buffer::Usage::Default, "UpdateDataDeferred should be used with Default heap buffers");
		RYU_ASSERT(data, "Data cannot be null");
		RYU_ASSERT(offset + sizeInBytes <= m_desc.SizeInBytes, "Update would exceed buffer size");

		DX12::Device* device = GetDevice()->GetNativeDevice();

		// Create or reuse upload buffer
		if (!m_uploadBuffer || m_uploadBuffer->GetDesc().Width < sizeInBytes)
		{
			m_uploadBuffer.Reset();

			const CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
			const CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes);

			DXCall(device->CreateCommittedResource(
				&uploadHeapProps,
				D3D12_HEAP_FLAG_NONE,
				&uploadBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_uploadBuffer)
			));
		}

		// Copy data to upload buffer
		void* mappedUploadData = nullptr;
		const CD3DX12_RANGE readRange(0, 0);

		DXCall(m_uploadBuffer->Map(0, &readRange, &mappedUploadData));
		std::memcpy(mappedUploadData, data, sizeInBytes);
		m_uploadBuffer->Unmap(0, nullptr);

		// Transition buffer to copy dest if needed
		RYU_TODO("Improve resource tracking here")
		const D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_COMMON;
		if (currentState != D3D12_RESOURCE_STATE_COPY_DEST)
		{
			cmdList.TransitionResource(m_resource.Get(), currentState, D3D12_RESOURCE_STATE_COPY_DEST);
		}

		// Copy from upload buffer to default buffer
		cmdList.GetNative()->CopyBufferRegion(m_resource.Get(), offset, m_uploadBuffer.Get(), 0, sizeInBytes);

		D3D12_RESOURCE_STATES targetState = static_cast<D3D12_RESOURCE_STATES>(m_desc.Type);
		
		cmdList.TransitionResource(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, targetState);
	}
	
	void Buffer::FinishUpload(CommandList& cmdList)
	{
		if (!m_needsUpload || !m_uploadBuffer)
		{
			return;
		}

		// Copy from upload buffer to default buffer
		cmdList.GetNative()->CopyBufferRegion(m_resource.Get(), 0, m_uploadBuffer.Get(), 0, m_desc.SizeInBytes);

		D3D12_RESOURCE_STATES targetState = static_cast<D3D12_RESOURCE_STATES>(m_desc.Type);
		cmdList.TransitionResource(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, targetState);

		m_needsUpload = false;
		
		// We can release the upload buffer after GPU finishes, but we'll keep it in case we want to update again
	}
	
	void Buffer::CreateBuffer(const void* initialData)
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		D3D12_HEAP_TYPE heapType = static_cast<D3D12_HEAP_TYPE>(m_desc.Type);
		D3D12_RESOURCE_STATES initialState;

		switch (m_desc.Usage)
		{
			using enum Buffer::Usage;

		case Default:
		{
			// If we have initial data, we'll copy to it, so start in COPY_DEST
			// Otherwise start in COMMON state
			initialState = initialData ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_COMMON;
			break;
		}

		case Upload:
		{
			initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
			break;
		}

		case Readback:
		{
			initialState = D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		}

		default:
			RYU_ASSERT(false, "Invalid buffer usage");
			return;
		}

		// Constant buffers must be 256-byte aligned
		u64 alignedSize = m_desc.SizeInBytes;
		if (m_desc.Type == Buffer::Type::Constant)
		{
			alignedSize = (m_desc.SizeInBytes + 255) & ~255;
		}

		// Create the buffer resource
		const CD3DX12_HEAP_PROPERTIES heapProps(heapType);
		const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedSize);

		DXCall(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			initialState,
			nullptr,
			IID_PPV_ARGS(&m_resource)
		));
		DX12::SetObjectName(m_resource.Get(), m_desc.Name.c_str());

		m_gpuAddress = m_resource->GetGPUVirtualAddress();

		// For Upload heap buffers, keep them persistently mapped
		if (m_desc.Usage == Buffer::Usage::Upload)
		{
			Map();
		}
	}
	
	void Buffer::UploadInitialData(const void* initialData)
	{
		RYU_ASSERT(initialData, "Initial data cannot be null");
		
		DX12::Device* device = GetDevice()->GetNativeDevice();


		if (m_desc.Usage == Buffer::Usage::Upload)
		{
			// For upload buffers, we can directly copy
			RYU_ASSERT(m_mappedData, "Upload buffer should be mapped");
			std::memcpy(m_mappedData, initialData, m_desc.SizeInBytes);
		}
		else if (m_desc.Usage == Buffer::Usage::Default)
		{
			// For default heap buffers, we need to use an intermediate upload buffer
			// Create temporary upload buffer
			const CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
			const CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_desc.SizeInBytes);

			DXCall(device->CreateCommittedResource(
				&uploadHeapProps,
				D3D12_HEAP_FLAG_NONE,
				&uploadBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_uploadBuffer)
			));

			// Copy data to upload buffer
			void* mappedUploadData = nullptr;
			const CD3DX12_RANGE readRange(0, 0);  // We don't intend to read from this resource on the CPU
			
			DXCall(m_uploadBuffer->Map(0, &readRange, &mappedUploadData));
			std::memcpy(mappedUploadData, initialData, m_desc.SizeInBytes);
			m_uploadBuffer->Unmap(0, nullptr);

			// Mark that we need to perform the GPU copy
			m_needsUpload = true;
		}
	}
}
