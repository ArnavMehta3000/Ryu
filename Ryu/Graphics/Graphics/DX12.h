#pragma once
#include "Graphics/ComPtr.h"
#include "Logger/LogCategory.h"
#include "Graphics/RHI.h"
#include <directx/d3dx12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(DX12);

	namespace DX12
	{
		using Object        = ID3D12Object;
		using Device        = ID3D12Device8;
		using GfxCmdList    = ID3D12GraphicsCommandList6;
		using Resource      = ID3D12Resource2;
		using InfoQueue     = ID3D12InfoQueue1;
		using Fence         = ID3D12Fence1;
		using CmdQueue      = ID3D12CommandQueue;
		using CmdAllocator  = ID3D12CommandAllocator;
		using DescHeap      = ID3D12DescriptorHeap;
		using PipelineState = ID3D12PipelineState;

		void SetObjectName(DX12::Object* object, const char* name);
		std::string GetObjectName(DX12::Object* object);

		constexpr D3D12_COMMAND_LIST_TYPE GetCmdListType(CmdListType type);
		constexpr D3D12_DESCRIPTOR_HEAP_TYPE GetDescHeapType(DescHeapType type);
	}

	namespace DXGI
	{
		using Factory   = IDXGIFactory7;
		using SwapChain = IDXGISwapChain4;
		using Adapter   = IDXGIAdapter4;

		constexpr DXGI_FORMAT ConvertFormat(Format format);
		constexpr DXGI_FORMAT GetFormatSRGB(DXGI_FORMAT format, bool srgb);
	}

	namespace Internal
	{
		constexpr std::string_view CommandListTypeToString(D3D12_COMMAND_LIST_TYPE type);
		constexpr std::string_view DescHeapTypeToString(D3D12_DESCRIPTOR_HEAP_TYPE type);
		constexpr std::string_view FeatureLevelToString(D3D_FEATURE_LEVEL level);
		std::string GetErrorString(HRESULT errorCode, DX12::Device* device);
		bool LogHRESULT(
			MAYBE_UNUSED HRESULT hr,
			MAYBE_UNUSED DX12::Device* device,
			MAYBE_UNUSED const char* code,
			MAYBE_UNUSED const char* fileName,
			MAYBE_UNUSED const char* functionName,
			MAYBE_UNUSED u32 lineNumber);
	}
}

#define DXCall(hr) ::Ryu::Gfx::Internal::LogHRESULT(hr, nullptr, #hr, __FILE__, __FUNCTION__, __LINE__)
#define DXCallEx(hr, dx12Device) ::Ryu::Gfx::Internal::LogHRESULT(hr, dx12Device, #hr, __FILE__, __FUNCTION__, __LINE__)
