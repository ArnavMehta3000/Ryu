#pragma once
#include "Common/ObjectMacros.h"
#include "Graphics/Core/DX12.h"
#include <memory>

namespace Ryu::Gfx
{
	class DeviceObject
	{
	public:
		explicit DeviceObject(std::weak_ptr<Device> parent)
			: m_parent(parent)
		{}

		virtual ~DeviceObject() { m_parent.reset(); }

		inline NODISCARD std::shared_ptr<Device> GetParent() const noexcept { return m_parent.lock(); }

	private:
		std::weak_ptr<Device> m_parent;
	};
}
