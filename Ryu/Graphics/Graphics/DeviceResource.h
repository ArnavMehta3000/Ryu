#pragma once
#include "Common/ObjectMacros.h"
#include "Graphics/DX12.h"
#include "Memory/Ref.h"
#include <array>

namespace Ryu::Gfx
{
	class DeviceObject : public Memory::RefCounted
	{
	public:
		explicit DeviceObject(Device* parent)
			: m_parent(parent)
		{}

		virtual ~DeviceObject() = default;

		inline NODISCARD Device* GetParent() const noexcept { return m_parent; }

	private:
		Device* m_parent;
	};
}
