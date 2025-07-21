#pragma once
#include "Graphics/Core/DX12.h"
#include "Utils/Constructable.h"
#include <memory>

namespace Ryu::Gfx
{
    template<typename T>
    concept DeviceObjectDerived = std::derived_from<T, DeviceObject<T>>;

    template <typename Derived>
    class DeviceObject : public Utils::Constructable<Derived, DeviceWeakPtr>
    {
    public:
        DeviceObject() = default;
        explicit DeviceObject(DeviceWeakPtr parent)
            : Utils::Constructable<Derived, DeviceWeakPtr>(parent)
        {
        }
        virtual ~DeviceObject() = default;

        inline NODISCARD DevicePtr GetParent() const noexcept
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
