#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class CommandAllocator : public DeviceObject<CommandAllocator>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		RYU_GFX_NATIVE(m_allocator)
			
		CommandAllocator() = default;
		explicit CommandAllocator(DeviceWeakPtr parent, CommandListType type);
		~CommandAllocator();

		void Reset() const;

	private:
		void OnConstruct(CommandListType type);
		void OnDestruct();

	private:
		ComPtr<DX12::CommandAllocator> m_allocator;
	};
}
