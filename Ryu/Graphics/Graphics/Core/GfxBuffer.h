#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class GfxDevice;
	class CommandContext;

	enum class BufferType
	{
		Vertex,
		Index,
		Constant,
		Upload,
		ReadBack
	};

	struct BufferDesc
	{
		u64 SizeInBytes                    = 0;
		u32 StrideInBytes                  = 0; // For structured buffers
		BufferType Type                    = BufferType::Vertex;
		D3D12_HEAP_TYPE HeapType           = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_GENERIC_READ;
		const void* InitialData            = nullptr;
		const char* DebugName              = nullptr;
	};

	class GfxBuffer
	{
	public:
		GfxBuffer(GfxDevice* device, const BufferDesc& desc);
		~GfxBuffer();

		RYU_DISABLE_COPY(GfxBuffer)

		[[nodiscard]] inline auto GetResource() const { return m_buffer.Get(); }
		[[nodiscard]] inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return m_buffer->GetGPUVirtualAddress(); }
		[[nodiscard]] inline u64 GetSizeInBytes() const { return m_desc.SizeInBytes; }
		[[nodiscard]] inline u32 GetStrideInBytes() const { return m_desc.StrideInBytes; }

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(DXGI_FORMAT format = DXGI_FORMAT_R32_UINT) const;

		void Upload(const void* data, u64 sizeInBytes, u64 offset = 0);
		void UploadWithContext(CommandContext* context, const void* data, u64 sizeInBytes);

		void* Map();
		void Unmap();

	private:
		void CreateBuffer();
		void UploadInitialData(CommandContext* context);

	private:
		GfxDevice*             m_device = nullptr;
		BufferDesc             m_desc;
		ComPtr<DX12::Resource> m_buffer;
		void*                  m_mappedData = nullptr;
	};
}
