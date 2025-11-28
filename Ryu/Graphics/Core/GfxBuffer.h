#pragma once
#include "Graphics/Core/GfxResource.h"
#include "Graphics/Core/GfxDescriptorHeap.h"

namespace Ryu::Gfx
{
	class CommandList;

	class Buffer : public Resource
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
			u32 SizeInBytes     = 0;
			u32 StrideInBytes   = 0;  // For structured buffers, index buffers, vertex buffers
			Buffer::Usage Usage = Buffer::Usage::Default;
			Buffer::Type Type   = Buffer::Type::Vertex;
			std::string Name;
		};

	public:
		Buffer(Device* parent, const Buffer::Desc& desc, DX12::Resource* uploadBuffer = nullptr);  // For static buffers
		Buffer(Device* parent, const Buffer::Desc& desc, const DescriptorHandle& destHandle, DX12::Resource* uploadBuffer = nullptr);
		virtual ~Buffer() = default;

		virtual void ReleaseObject() override;

		static constexpr u32 CalculateConstantBufferSize(u32 byteSize);

		[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
		[[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(DXGI_FORMAT format = DXGI_FORMAT_R32_UINT) const;
		[[nodiscard]] D3D12_CONSTANT_BUFFER_VIEW_DESC GetConstantBufferViewDesc() const;
		[[nodiscard]] inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const noexcept { return m_gpuAddress; }
		[[nodiscard]] inline const Buffer::Desc& GetDesc() const noexcept { return m_desc; }
		[[nodiscard]] inline const DescriptorHandle& GetDescriptorHandle() const noexcept { return m_destHandle; }

		inline bool NeedsUpload() const { return m_needsUpload; }
		inline void MarkForUpload() { m_needsUpload = true; }

		void UploadData(const CommandList& cmdList, const void* data);

		void* Map(const CD3DX12_RANGE& range = CD3DX12_RANGE(0,0));
		void Unmap();

		template <typename T>
		T* Map() { return reinterpret_cast<T*>(Map()); }

	private:
		void CreateBuffer();
		void CreateUploadBuffer();

	private:
		Buffer::Desc              m_desc;
		ComPtr<DX12::Resource>    m_uploadBuffer;
		D3D12_GPU_VIRTUAL_ADDRESS m_gpuAddress = 0;
		bool                      m_needsUpload = true;
		void*                     m_mappedData  = nullptr;
		DescriptorHandle          m_destHandle;
	};
}
