#pragma once
#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS

#include "Core/Logging/Logger.h"
#include "Core/Utils/Blob.h"
#include "Graphics/Core/ComPtr.h"
#include "Graphics/Core/RHI.h"
#include <directx/d3dx12.h>
#include <dxgi1_6.h>

struct IDxcBlobEncoding;

namespace Ryu::Gfx
{
	extern ComPtr<IDxcBlobEncoding> CreateDXCBlobFromRyuBlob(const Utils::Blob& blob, u32 codePage = 0 /* DXC_CP_ACP */);
	extern ComPtr<ID3DBlob> CreateD3DBlobFromRyuBlob(const Utils::Blob& blob);
	
	namespace DX12
	{
		using Object              = ID3D12Object;
		using Device              = ID3D12Device8;
		using CommandList         = ID3D12CommandList;
		using GraphicsCommandList = ID3D12GraphicsCommandList6;
		using Resource            = ID3D12Resource2;
		using Heap                = ID3D12Heap1;
		using InfoQueue           = ID3D12InfoQueue1;
		using Fence               = ID3D12Fence1;
		using CommandQueue        = ID3D12CommandQueue;
		using CommandAllocator    = ID3D12CommandAllocator;
		using DescriptorHeap      = ID3D12DescriptorHeap;
		using PipelineState       = ID3D12PipelineState;
		using RootSignature       = ID3D12RootSignature;

		void SetObjectName(DX12::Object* object, const char* name);
		std::string GetObjectName(DX12::Object* object);
	}

	namespace DXGI
	{
		using Factory   = IDXGIFactory7;
		using SwapChain = IDXGISwapChain4;
		using Adapter   = IDXGIAdapter4;

		DXGI_FORMAT GetFormatSRGB(DXGI_FORMAT format);
		DXGI_FORMAT GetFormatNonSRGB(DXGI_FORMAT format);
	}

	namespace Internal
	{
		std::string_view FeatureLevelToString(D3D_FEATURE_LEVEL level);
		std::string GetErrorString(HRESULT errorCode, DX12::Device* device);
		bool LogHRESULT(
			[[maybe_unused]] HRESULT hr,
			[[maybe_unused]] DX12::Device* device,
			[[maybe_unused]] const char* code,
			[[maybe_unused]] const char* fileName,
			[[maybe_unused]] const char* functionName,
			[[maybe_unused]] u32 lineNumber);
	}
}

#define DXCall(hr) ::Ryu::Gfx::Internal::LogHRESULT(hr, nullptr, #hr, __FILE__, __FUNCTION__, __LINE__)
#define DXCallEx(hr, dx12Device) ::Ryu::Gfx::Internal::LogHRESULT(hr, dx12Device, #hr, __FILE__, __FUNCTION__, __LINE__)

// Useful for wrapping D3D12 objects (assuming VarName is a ComPtr)
#define RYU_GFX_NATIVE(VarName)                                                     \
	inline void SetName(const char* name) const                                     \
	{                                                                               \
		DX12::SetObjectName(VarName.Get(), name);                                   \
		RYU_DEBUG_OP(m_debugName = name);                                           \
	}                                                                               \
	[[nodiscard]] inline auto GetNative() const noexcept { return VarName.Get(); }  \
	operator auto* () const noexcept { return VarName.Get(); }                      \
	operator bool () const noexcept { return VarName.Get() != nullptr; }
