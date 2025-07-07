#pragma once
#include "Graphics/Core/DX12.h"
#include "Utils/Constructable.h"
#include <memory>

namespace Ryu::Gfx
{
    template<typename T>
    concept DeviceObjectDerived = std::derived_from<T, DeviceObject<T>>;

    template <typename Derived>
    class DeviceObject : public Utils::Constructable<Derived, std::weak_ptr<Device>>
    {
    public:
        DeviceObject() = default;
        explicit DeviceObject(std::weak_ptr<Device> parent)
            : Utils::Constructable<Derived, std::weak_ptr<Device>>(parent)
        {
        }
        virtual ~DeviceObject() = default;

        inline NODISCARD std::shared_ptr<Device> GetParent() const noexcept
        {
            return std::get<0>(this->GetCompulsoryArgs()).lock();
        }
    };

    class DeviceObjectBase : public DeviceObject<DeviceObjectBase>
    {
    public:
        using DeviceObject<DeviceObjectBase>::DeviceObject;
    };
}

#define RYU_GFX_DEVICE_OBJ \
RYU_CONSTRUCTABLE_FRIEND; \
template<typename T> friend class ::Ryu::Gfx::DeviceObject
