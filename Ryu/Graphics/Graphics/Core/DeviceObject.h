#pragma once
#include "Graphics/Core/DX12.h"
#include "Utils/Constructable.h"
#include <memory>
#include <mutex>

namespace Ryu::Gfx
{
	template<typename T>
	concept DeviceObjectDerived = std::derived_from<T, DeviceObject<T>>;

	template <typename Derived>
	class DeviceObject
	{
	public:
		DeviceObject() = default;
		explicit DeviceObject(std::weak_ptr<Device> parent) : m_parent(parent)
		{
			// If we are creating the object manually then mark this object as initialized
			// It is the duty of the derived class to call OnConstruct in its constructor (if needed)
			std::call_once(m_initFlag, []() {});
		}
		virtual ~DeviceObject() { m_parent.reset(); }

		inline NODISCARD std::shared_ptr<Device> GetParent() const noexcept { return m_parent.lock(); }

		// Initialize with OnConstruct arguments
		template<typename... OnConstructArgs>
			requires Utils::HasOnConstruct<Derived, OnConstructArgs...>
		Derived& Initialize(std::weak_ptr<Device> parent, OnConstructArgs&&... args)
		{
			std::call_once(m_initFlag, [&]()
			{
				m_parent = parent;
				Utils::InvokeOnConstruct(static_cast<Derived*>(this), std::forward<OnConstructArgs>(args)...);
			});

			return static_cast<Derived&>(*this);
		}

		// Initialize with parameterless OnConstruct
		Derived& Initialize(std::weak_ptr<Device> parent)
			requires Utils::HasParameterlessOnConstruct<Derived>
		{
			std::call_once(m_initFlag, [&]()
			{
				m_parent = parent;
				Utils::InvokeOnConstruct(static_cast<Derived*>(this));
			});

			return static_cast<Derived&>(*this);
		}

		// Initialize without OnConstruct
		Derived& Initialize(std::weak_ptr<Device> parent)
			requires (!Utils::HasParameterlessOnConstruct<Derived>)
		{
			std::call_once(m_initFlag, [&]() { m_parent = parent; });
			return static_cast<Derived&>(*this);
		}

	private:
		std::once_flag m_initFlag;
		std::weak_ptr<Device> m_parent;
	};

	class DeviceObjectBase : public DeviceObject<DeviceObjectBase>
	{
	public:
		using DeviceObject<DeviceObjectBase>::DeviceObject;
	};

}
#define RYU_GFX_DEVICE_OBJ template<typename T> friend class ::Ryu::Gfx::DeviceObject
