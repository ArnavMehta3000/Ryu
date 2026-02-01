#pragma once
#include "Game/World/Entity.h"
#include <typeinfo>

namespace Ryu::Editor
{
	class IComponentPanel
	{
	public:
		IComponentPanel() = default;
		~IComponentPanel() = default;

		virtual void OnImGuiRender(Game::Entity entity) = 0;

		virtual const char* GetComponentName() const = 0;
		virtual u64 GetComponentTypeHash() const = 0;
		virtual bool CanHandle(u64 componentTypeHash) const = 0;
		virtual bool HasComponent(const Game::Entity& e) const = 0;
	};

	template <typename T>
	concept HasComponentName = requires { { T::ComponentName }; };

	template <HasComponentName ComponentType>
	class ComponentPanel : public IComponentPanel
	{
	public:
		ComponentPanel() = default;
		virtual ~ComponentPanel() = default;

		inline const char* GetComponentName() const override { return ComponentType::ComponentName; }
		inline u64 GetComponentTypeHash() const override { return typeid(ComponentType).hash_code(); }
		inline bool CanHandle(u64 componentTypeHash) const override { return componentTypeHash == GetComponentTypeHash(); }		
		inline bool HasComponent(const Game::Entity& e) const override { return e.HasComponent<ComponentType>(); }

		void OnImGuiRender(Game::Entity entity) override
		{
			if (entity.IsValid())
			{
				if (auto* component = entity.TryGetComponent<ComponentType>())
				{
					DrawComponentUI(*component, entity);
				}
			}
		}

	protected:
		virtual void DrawComponentUI(ComponentType& component, Game::Entity entity) = 0;
	};
}
