#include "Game/World/Entity.h"

#define WorldCheck() RYU_ASSERT(m_world, "Entity is not attached to a world!")

namespace Ryu::Game
{
    bool Entity::IsValid() const
    {
        return m_world && m_world->GetRegistry().valid(m_handle) && !IsMarkedForDestroy();
    }

    void Entity::Destroy()
    {
        WorldCheck();
		m_world->DestroyEntity(*this);
    }

    void Entity::SetFlag(EntityFlag flag, bool value)
    {
        WorldCheck();
        auto& metadata = m_world->GetRegistry().get<EntityMetadata>(m_handle);
        metadata.Flags.set(static_cast<size_t>(flag), value);
    }

    bool Entity::HasFlag(EntityFlag flag) const
    {
		WorldCheck();
        const auto& metadata = m_world->GetRegistry().get<EntityMetadata>(m_handle);
        return metadata.Flags.test(static_cast<size_t>(flag));
    }

    void Entity::ClearFlag(EntityFlag flag)
    {
        SetFlag(flag, false);
    }

    void Entity::ClearAllFlags()
    {
        WorldCheck();
        auto& metadata = m_world->GetRegistry().get<EntityMetadata>(m_handle);
        metadata.Flags.reset();
    }

    EntityFlag Entity::GetFlags() const
    {
        WorldCheck();
		const auto& metadata = m_world->GetRegistry().get<EntityMetadata>(m_handle);
		return static_cast<EntityFlag>(metadata.Flags.to_ulong());
    }

    void Entity::MarkForDestroy()
    {
        SetFlag(EntityFlag::MarkedForDestroy, true);
    }

    bool Entity::IsMarkedForDestroy() const
    {
        return HasFlag(EntityFlag::MarkedForDestroy);
    }

    void Entity::SetEnabled(bool enabled)
    {
        SetFlag(EntityFlag::Disabled, enabled);
    }

    bool Entity::IsEnabled() const
    {
        return !HasFlag(EntityFlag::Disabled);
    }
}

#undef WorldCheck
