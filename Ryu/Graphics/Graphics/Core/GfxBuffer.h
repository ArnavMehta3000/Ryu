#pragma once
#include "Graphics/Core/GfxDeviceChild.h"

namespace Ryu::Gfx
{
	class CommandList;

	class Buffer : public DeviceChild
	{
	public:
		enum class Usage : u8
		{
			Default  = D3D12_HEAP_TYPE_DEFAULT,  // GPU-only memory (best performance for static data)
			Upload   = D3D12_HEAP_TYPE_UPLOAD,   // CPU-writable, GPU-readable (for dynamic data updated per frame)
			Readback = D3D12_HEAP_TYPE_READBACK  // GPU-writable, CPU-readable (for reading back GPU results)
		};

		enum class Type : u8
		{
			Vertex     = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			Index      = D3D12_RESOURCE_STATE_INDEX_BUFFER,
			Constant   = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			Structured = D3D12_RESOURCE_STATE_COMMON,
			Raw        = D3D12_RESOURCE_STATE_COMMON
		};

		struct Desc
		{
			u64 SizeInBytes     = 0;
			u64 StrideInBytes   = 0;  // For structured buffers, index buffers, vertex buffers
			Buffer::Usage Usage = Buffer::Usage::Default;
			Buffer::Type Type   = Buffer::Type::Vertex;
			std::string Name;
		};

	public:
		Buffer(Device* parent, const Buffer::Desc& desc, const void* initialData);  // For static buffers
		Buffer(Device* parent, const Buffer::Desc& desc);  // For dynamic buffers
		virtual ~Buffer() = default;

		virtual inline void ReleaseObject() override;

		RYU_GFX_NATIVE(m_resource)

		[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
		[[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(DXGI_FORMAT format = DXGI_FORMAT_R32_UINT) const;
		[[nodiscard]] D3D12_CONSTANT_BUFFER_VIEW_DESC GetConstantBufferViewDesc() const;
		[[nodiscard]] inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const noexcept { return m_gpuAddress; }
		[[nodiscard]] inline u64 GetSize() const noexcept { return m_desc.SizeInBytes; }
		[[nodiscard]] inline u64 GetStride() const noexcept { return m_desc.StrideInBytes; }
		[[nodiscard]] inline Buffer::Usage GetUsage() const noexcept { return m_desc.Usage; }
		[[nodiscard]] inline Buffer::Type GetType() const noexcept { return m_desc.Type; }

		void* Map();
		void Unmap();

		void UpdateData(const void* data, u64 sizeInBytes, u64 offset = 0);  // For Upload heap buffers
		void UpdateDataDeferred(CommandList& cmdList, const void* data, u64 sizeInBytes, u64 offset = 0);  // For Default heap buffers
		void FinishUpload(CommandList& cmdList);

	private:
		void CreateBuffer(const void* initialData);
		void UploadInitialData(const void* initialData);

	private:
		Buffer::Desc              m_desc;
		ComPtr<DX12::Resource>    m_resource;
		ComPtr<DX12::Resource>    m_uploadBuffer;
		D3D12_GPU_VIRTUAL_ADDRESS m_gpuAddress = 0;

		void*                     m_mappedData = nullptr;
		bool                      m_needsUpload = false;
	};
}
