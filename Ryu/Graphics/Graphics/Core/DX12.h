#pragma once
#include "Graphics/Core/ComPtr.h"
#include "Graphics/Core/RHI.h"
#include "Logger/LogCategory.h"
#include <directx/d3dx12.h>
#include <dxgi1_6.h>
#include <vector>

namespace Ryu::Gfx
{
	namespace DX12
	{
		using Object              = ID3D12Object;
		using Device              = ID3D12Device8;
		using CommandList         = ID3D12CommandList;
		using GraphicsCommandList = ID3D12GraphicsCommandList6;
		using Resource            = ID3D12Resource2;
		using InfoQueue           = ID3D12InfoQueue1;
		using Fence               = ID3D12Fence1;
		using CommandQueue        = ID3D12CommandQueue;
		using CommandAllocator    = ID3D12CommandAllocator;
		using DescriptorHeap      = ID3D12DescriptorHeap;
		using PipelineState       = ID3D12PipelineState;
		using RootSignature       = ID3D12RootSignature;

		void SetObjectName(DX12::Object* object, const char* name);
		std::string GetObjectName(DX12::Object* object);

		D3D12_COMMAND_LIST_TYPE ToNative(CommandListType type);
		D3D12_COMMAND_QUEUE_PRIORITY ToNative(CommandQueuePriority priority);
		D3D12_DESCRIPTOR_HEAP_TYPE ToNative(DescriptorHeapType type);
		D3D12_FENCE_FLAGS ToNative(FenceFlag flag);
		D3D12_DESCRIPTOR_HEAP_FLAGS ToNative(DescriptorHeapFlags flags);
	}

	namespace DXGI
	{
		using Factory   = IDXGIFactory7;
		using SwapChain = IDXGISwapChain4;
		using Adapter   = IDXGIAdapter4;

		DXGI_FORMAT ToNative(Format format);
		DXGI_FORMAT GetFormatSRGB(DXGI_FORMAT format);
	}

	namespace Internal
	{
		std::string_view FeatureLevelToString(D3D_FEATURE_LEVEL level);
		std::string GetErrorString(HRESULT errorCode, DX12::Device* device);
		bool LogHRESULT(
			MAYBE_UNUSED HRESULT hr,
			MAYBE_UNUSED DX12::Device* device,
			MAYBE_UNUSED const char* code,
			MAYBE_UNUSED const char* fileName,
			MAYBE_UNUSED const char* functionName,
			MAYBE_UNUSED u32 lineNumber);
	}

	class FreeList
	{
	public:
		explicit FreeList(u32 size);		
		~FreeList();

		u32 Allocate();
		void Free(u32 index);		
		bool CanAllocate() const noexcept;

	private:
		std::vector<u32> m_freeList;
		u32 m_numAllocations;
	};
}

#define DXCall(hr) ::Ryu::Gfx::Internal::LogHRESULT(hr, nullptr, #hr, __FILE__, __FUNCTION__, __LINE__)
#define DXCallEx(hr, dx12Device) ::Ryu::Gfx::Internal::LogHRESULT(hr, dx12Device, #hr, __FILE__, __FUNCTION__, __LINE__)

#define RYU_GFX_NATIVE(VarName)                                                                 \
	inline NODISCARD auto* Get() const noexcept { return VarName.Get(); }                       \
	inline void SetName(const char* name) const { DX12::SetObjectName(VarName.Get(), name); }   \
	operator auto* () const noexcept { return VarName.Get(); }                                  \
	operator bool () const noexcept { return VarName.Get() != nullptr; }
