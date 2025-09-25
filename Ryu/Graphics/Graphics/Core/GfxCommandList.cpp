#include "Graphics/Core/GfxCommandList.h"
#include "Graphics/Core/GfxDevice.h"

namespace Ryu::Gfx
{
	GfxCommandList::GfxCommandList(GfxDevice* device, CommandListType type, std::string_view name)
		: m_device(device)
		, m_type(type)
		, m_cmdQueue(device->GetCommandQueue(type))
	{
		D3D12_COMMAND_LIST_TYPE nativeType = DX12::ToNative(type);

		DX12::Device* dx12Device = m_device->GetDevice();

		DXCallEx(dx12Device->CreateCommandAllocator(nativeType, IID_PPV_ARGS(&m_cmdAllocator)), device->GetDevice());
		DXCallEx(dx12Device->CreateCommandList(0, nativeType, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cmdList)), device->GetDevice());

		SetName(name.data());
		m_cmdList->Close();
	}
}
