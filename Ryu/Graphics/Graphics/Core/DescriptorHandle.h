#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	struct DescriptorHandle
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPU;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPU;

		DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu) : CPU(cpu), GPU(gpu) {}
		DescriptorHandle() : CPU(D3D12_DEFAULT), GPU(D3D12_DEFAULT) {}

		void Offset(u32 count, u32 incrementSize) { CPU.Offset(count, incrementSize); GPU.Offset(count, incrementSize); }

		operator CD3DX12_CPU_DESCRIPTOR_HANDLE() const { return CPU; }
		operator CD3DX12_GPU_DESCRIPTOR_HANDLE() const { return GPU; }

		bool IsValid() const { return CPU.ptr != 0; }
		bool IsShaderVisible() const { return GPU.ptr != 0; }
	};
}
